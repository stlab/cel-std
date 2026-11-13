// SPDX-License-Identifier: BSL-1.0
#include <array>
#include <cel/parser/compiler.hpp>
#include <cel/std.hpp>
#include <cfenv>
#include <cmath>
#include <limits>
#include <string>
#include <string_view>
#include <tuple>
#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest/doctest.h>

namespace {
/// Compiles an installed-library expression and executes it after destroying its parser.
///
/// Complexity: O(compilation plus program execution) time and program storage.
template <class R = double>
R evaluate(std::string_view text) {
    auto program = [&] {
        cel::op_lookup lookup;
        cel::stdlib::install(lookup);
        cel::parser parser(std::move(lookup));
        return parser.parse_str(text);
    }();
    cel::raw_stack stack;
    return program.template prepare<R>().call(stack);
}

/// Specifies one ordinary and receiver call with its independently derived result.
struct call_case {
    const char* ordinary; ///< CEL ordinary call.
    const char* receiver; ///< Equivalent CEL receiver call.
    double expected;      ///< Expected result, within transcendental precision.
};
} // namespace

TEST_CASE("each scalar operation preserves its name signature and argument order") {
    const call_case cases[]{
        {"min(3.0, -5.0)", "3.0.min(-5.0)", -5.0},
        {"max(3.0, -5.0)", "3.0.max(-5.0)", 3.0},
        {"clamp(7.0, 1.0, 4.0)", "7.0.clamp(1.0, 4.0)", 4.0},
        {"abs(-3.0)", "(-3.0).abs()", 3.0},
        {"signum(-3.0)", "(-3.0).signum()", -1.0},
        {"sqrt(9.0)", "9.0.sqrt()", 3.0},
        {"floor(-1.25)", "(-1.25).floor()", -2.0},
        {"ceil(-1.25)", "(-1.25).ceil()", -1.0},
        {"trunc(-1.25)", "(-1.25).trunc()", -1.0},
        {"fract(-1.25)", "(-1.25).fract()", -0.25},
        {"round_ties_even(2.5)", "2.5.round_ties_even()", 2.0},
        {"round(-2.5)", "(-2.5).round()", -3.0},
        {"sin(0.5)", "0.5.sin()", 0.479425538604203},
        {"cos(0.5)", "0.5.cos()", 0.8775825618903728},
        {"tan(0.5)", "0.5.tan()", 0.5463024898437905},
        {"asin(0.5)", "0.5.asin()", 0.5235987755982989},
        {"acos(0.5)", "0.5.acos()", 1.0471975511965979},
        {"atan(1.0)", "1.0.atan()", 0.7853981633974483},
        {"atan2(1.0, -1.0)", "1.0.atan2(-1.0)", 2.356194490192345},
        {"to_radians(180.0)", "180.0.to_radians()", 3.141592653589793},
        {"to_degrees(0.5)", "0.5.to_degrees()", 28.64788975654116},
        {"hypot(3.0, 4.0)", "3.0.hypot(4.0)", 5.0},
        {"cbrt(-8.0)", "(-8.0).cbrt()", -2.0},
        {"powf(9.0, 0.5)", "9.0.powf(0.5)", 3.0},
        {"exp(1.0)", "1.0.exp()", 2.718281828459045},
        {"exp2(3.0)", "3.0.exp2()", 8.0},
        {"ln(2.0)", "2.0.ln()", 0.6931471805599453},
        {"log(8.0, 2.0)", "8.0.log(2.0)", 3.0},
        {"log2(8.0)", "8.0.log2()", 3.0},
        {"log10(100.0)", "100.0.log10()", 2.0},
        {"recip(4.0)", "4.0.recip()", 0.25},
        {"mul_add(2.0, 3.0, 4.0)", "2.0.mul_add(3.0, 4.0)", 10.0},
        {"rem_euclid(-7.0, 4.0)", "(-7.0).rem_euclid(4.0)", 1.0},
        {"copysign(2.0, -3.0)", "2.0.copysign(-3.0)", -2.0},
    };
    // Exercise every signature through both source call forms.
    for (const auto& test : cases) {
        CAPTURE(test.ordinary);
        CHECK(evaluate(test.ordinary) == doctest::Approx(test.expected).epsilon(1e-12));
        CHECK(evaluate(test.receiver) == doctest::Approx(test.expected).epsilon(1e-12));
    }
}

TEST_CASE("sin_cos returns a structural sine then cosine tuple") {
    const auto ordinary = evaluate<std::tuple<double, double>>("sin_cos(0.0)");
    const auto receiver = evaluate<std::tuple<double, double>>("0.0.sin_cos()");
    CHECK(ordinary == std::tuple{0.0, 1.0});
    CHECK(receiver == ordinary);
    CHECK(evaluate("sin_cos(0.0).1") == 1.0);
    const auto nonzero = evaluate<std::tuple<double, double>>("sin_cos(0.5)");
    CHECK(std::get<0>(nonzero) == doctest::Approx(0.479425538604203));
    CHECK(std::get<1>(nonzero) == doctest::Approx(0.8775825618903728));
}

TEST_CASE("persistent library operations are visible in closure bodies") {
    CHECK(evaluate("(|| sqrt(9.0))()") == 3.0);
    CHECK(evaluate("(|x: f64| x.clamp(0.0, 4.0))(9.0)") == 4.0);
}

TEST_CASE("installation preserves unrelated registrations and disjoint overloads") {
    cel::op_lookup lookup;
    lookup.register_op<double, double>("twice", [](double x) { return x * 2.0; });
    lookup.register_op<bool, bool, bool>("min", [](bool a, bool b) { return a && b; });
    cel::stdlib::install(lookup);
    cel::parser parser(std::move(lookup));
    cel::raw_stack stack;
    CHECK(parser.parse_str("3.0.twice()").prepare<double>().call(stack) == 6.0);
    CHECK_FALSE(parser.parse_str("true.min(false)").prepare<bool>().call(stack));
    CHECK(parser.parse_str("min(2.0, 3.0)").prepare<double>().call(stack) == 2.0);
}

TEST_CASE("the subset rejects powi unsupported operand types and wrong arities") {
    cel::op_lookup lookup;
    cel::stdlib::install(lookup);
    cel::parser parser(std::move(lookup));
    for (auto text : {"powi(2.0, 3.0)", "2.0.powi(3.0)", "sqrt(true)", "min(true, false)",
                      "sqrt(1.0, 2.0)", "clamp(1.0, 2.0)", "sin_cos()", "sqrt(1.0f32)"}) {
        CAPTURE(text);
        CHECK_THROWS_AS(parser.parse_str(text), cel::parse_error);
    }
    CHECK(evaluate("sqrt(4.0)") == 2.0);
}

TEST_CASE("clamp validates bounds and otherwise preserves Rust floating behavior") {
    for (auto text :
         {"clamp(1.0, 3.0, 2.0)", "clamp(1.0, 0.0 / 0.0, 2.0)", "clamp(1.0, 0.0, 0.0 / 0.0)"}) {
        CAPTURE(text);
        CHECK_THROWS_WITH_AS(evaluate(text), "invalid clamp bounds", std::invalid_argument);
    }
    CHECK(std::isnan(evaluate("clamp(0.0 / 0.0, 0.0, 1.0)")));
    CHECK(evaluate("clamp(-1e9999, -2.0, 2.0)") == -2.0);
    CHECK(evaluate("clamp(1e9999, -2.0, 2.0)") == 2.0);
    CHECK(evaluate("clamp(4.0, 2.0, 2.0)") == 2.0);
    CHECK(std::signbit(evaluate("clamp(-0.0, 0.0, 1.0)")));
}

TEST_CASE("extrema ignore a lone NaN and preserve infinities") {
    for (auto name : {"min", "max"}) {
        const std::string prefix = std::string(name) + "(";
        CHECK(evaluate(prefix + "0.0 / 0.0, 4.0)") == 4.0);
        CHECK(evaluate(prefix + "4.0, 0.0 / 0.0)") == 4.0);
        CHECK(std::isnan(evaluate(prefix + "0.0 / 0.0, 0.0 / 0.0)")));
    }
    CHECK(evaluate("min(-1e9999, 2.0)") == -std::numeric_limits<double>::infinity());
    CHECK(evaluate("max(1e9999, 2.0)") == std::numeric_limits<double>::infinity());
}

TEST_CASE("signum distinguishes signed zeros and propagates NaN") {
    CHECK(evaluate("signum(-0.0)") == -1.0);
    CHECK(evaluate("signum(0.0)") == 1.0);
    CHECK(evaluate("signum(-1e9999)") == -1.0);
    CHECK(evaluate("signum(1e9999)") == 1.0);
    CHECK(std::isnan(evaluate("signum(0.0 / 0.0)")));
    CHECK_FALSE(std::signbit(evaluate("abs(-0.0)")));
}

TEST_CASE("floating domain failures produce NaN and infinity rather than exceptions") {
    for (auto text :
         {"sqrt(-1.0)", "asin(2.0)", "acos(-2.0)", "sin(1e9999)", "cos(1e9999)", "tan(1e9999)",
          "ln(-1.0)", "log2(-1.0)", "log10(-1.0)", "log(-1.0, 2.0)", "powf(-1.0, 0.5)",
          "fract(1e9999)", "rem_euclid(1.0, 0.0)", "rem_euclid(1e9999, 2.0)"}) {
        CAPTURE(text);
        CHECK(std::isnan(evaluate(text)));
    }
    CHECK(evaluate("ln(0.0)") == -std::numeric_limits<double>::infinity());
    CHECK(evaluate("recip(-0.0)") == -std::numeric_limits<double>::infinity());
    CHECK(evaluate("recip(0.0)") == std::numeric_limits<double>::infinity());
    CHECK(evaluate("exp(1e9999)") == std::numeric_limits<double>::infinity());
    CHECK(evaluate("powf(0.0 / 0.0, 0.0)") == 1.0);
}

TEST_CASE("rounding preserves signed zeros boundaries and ties without changing the environment") {
    const int original = std::fegetround();
    REQUIRE(original != -1);
    // Each mode must yield the same nearest-even results, with no environment mutation.
    for (int mode : {FE_TONEAREST, FE_DOWNWARD, FE_UPWARD, FE_TOWARDZERO}) {
        REQUIRE(std::fesetround(mode) == 0);
        for (const auto& [text, expected] : std::array{
                 std::pair{"round_ties_even(0.5)", 0.0}, std::pair{"round_ties_even(-0.5)", -0.0},
                 std::pair{"round_ties_even(1.5)", 2.0}, std::pair{"round_ties_even(2.5)", 2.0},
                 std::pair{"round_ties_even(3.5)", 4.0}, std::pair{"round_ties_even(-2.5)", -2.0},
                 std::pair{"round_ties_even(-3.5)", -4.0},
                 std::pair{"round_ties_even(0.49999999999999994)", 0.0},
                 std::pair{"round_ties_even(0.5000000000000001)", 1.0},
                 std::pair{"round_ties_even(4503599627370495.5)", 4503599627370496.0},
                 std::pair{"round_ties_even(4503599627370496.0)", 4503599627370496.0}}) {
            CAPTURE(mode);
            CAPTURE(text);
            const auto actual = evaluate(text);
            CHECK(actual == expected);
            CHECK(std::signbit(actual) == std::signbit(expected));
        }
        CHECK(std::isinf(evaluate("round_ties_even(1e9999)")));
        CHECK(std::isnan(evaluate("round_ties_even(0.0 / 0.0)")));
        CHECK(std::fegetround() == mode);
    }
    REQUIRE(std::fesetround(original) == 0);
    CHECK(std::signbit(evaluate("round(-0.25)")));
    CHECK(std::signbit(evaluate("trunc(-0.25)")));
    CHECK(std::signbit(evaluate("sqrt(-0.0)")));
    CHECK(std::signbit(evaluate("sin(-0.0)")));
}

TEST_CASE("Euclidean remainders use divisor magnitude and permit rounded upper bounds") {
    CHECK(evaluate("rem_euclid(-7.0, -4.0)") == 1.0);
    CHECK(evaluate("rem_euclid(7.0, -4.0)") == 3.0);
    CHECK(evaluate("rem_euclid(-1e-100, 1.0)") == 1.0);
    CHECK(std::signbit(evaluate("rem_euclid(-0.0, 2.0)")));
    CHECK(evaluate("rem_euclid(3.0, 1e9999)") == 3.0);
    CHECK(std::isinf(evaluate("rem_euclid(-3.0, 1e9999)")));
}

TEST_CASE("mul_add has one rounding step rather than a rounded intermediate product") {
    CHECK(evaluate("mul_add(1.0000000000000002, 0.9999999999999998, -1.0)") ==
          -4.930380657631324e-32);
    CHECK(std::signbit(evaluate("copysign(0.0, -1.0)")));
}
