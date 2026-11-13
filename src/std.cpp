// SPDX-License-Identifier: BSL-1.0
#include <algorithm>
#include <cel/parser/lookup.hpp>
#include <cel/std.hpp>
#include <cmath>
#include <limits>
#include <numbers>
#include <stdexcept>
#include <string_view>
#include <tuple>
#include <type_traits>

namespace {
static_assert(std::numeric_limits<double>::is_iec559 && std::numeric_limits<double>::digits == 53 &&
                  std::numeric_limits<double>::max_exponent == 1024,
              "CEL f64 requires IEEE 754 binary64 double");

/// Identifies one ordinary function without mutable callback state.
template <auto Function>
struct function_marker {};

/// Registers one exact signature for direct, receiver, and ordinary calls.
///
/// \pre No registration duplicates the installed operand signatures.
/// Expected complexity: O(s + 1) time for s existing overloads of these names;
/// constant additional registration storage.
template <auto Function, class... A>
void add(cel::op_lookup& lookup, std::string_view name) {
    using result = std::invoke_result_t<decltype(Function), A...>;
    using marker = function_marker<Function>;
    lookup.register_op<result, A...>(name, Function);
    lookup.register_function(name, marker{});
    lookup.register_op<result, marker, A...>(
        "()", [](marker, A... arguments) { return Function(arguments...); });
}

/// Rounds to the nearest integer, selecting the even neighbor on a tie.
///
/// Preserves signed zeros and nonfinite values independently of the rounding mode.
double round_ties_even(double x) {
    if (!std::isfinite(x)) return x;
    return std::copysign(x - std::remainder(x, 1.0), x);
}

/// Returns Rust's Euclidean remainder, including a possible rounded divisor magnitude.
double rem_euclid(double x, double divisor) {
    const auto remainder = std::fmod(x, divisor);
    return remainder < 0.0 ? remainder + std::abs(divisor) : remainder;
}

/// Returns x bounded by ordered limits, preserving NaN x.
///
/// Throws std::invalid_argument("invalid clamp bounds") for reversed or NaN bounds.
double clamp(double x, double lower, double upper) {
    if (!(lower <= upper)) throw std::invalid_argument("invalid clamp bounds");
    return std::clamp(x, lower, upper);
}
} // namespace

namespace cel::stdlib {
void install(op_lookup& lookup) {
    add<[](double a, double b) { return std::fmin(a, b); }, double, double>(lookup, "min");
    add<[](double a, double b) { return std::fmax(a, b); }, double, double>(lookup, "max");
    add<clamp, double, double, double>(lookup, "clamp");
    add<[](double x) { return std::abs(x); }, double>(lookup, "abs");
    add<[](double x) { return std::isnan(x) ? x : std::copysign(1.0, x); }, double>(lookup,
                                                                                    "signum");
    add<[](double x) { return std::sqrt(x); }, double>(lookup, "sqrt");
    add<[](double x) { return std::floor(x); }, double>(lookup, "floor");
    add<[](double x) { return std::ceil(x); }, double>(lookup, "ceil");
    add<[](double x) { return std::trunc(x); }, double>(lookup, "trunc");
    add<[](double x) { return x - std::trunc(x); }, double>(lookup, "fract");
    add<round_ties_even, double>(lookup, "round_ties_even");
    add<[](double x) { return std::round(x); }, double>(lookup, "round");
    add<[](double x) { return std::sin(x); }, double>(lookup, "sin");
    add<[](double x) { return std::cos(x); }, double>(lookup, "cos");
    add<[](double x) { return std::tan(x); }, double>(lookup, "tan");
    add<[](double x) { return std::asin(x); }, double>(lookup, "asin");
    add<[](double x) { return std::acos(x); }, double>(lookup, "acos");
    add<[](double x) { return std::atan(x); }, double>(lookup, "atan");
    add<[](double y, double x) { return std::atan2(y, x); }, double, double>(lookup, "atan2");
    add<[](double x) { return std::tuple{std::sin(x), std::cos(x)}; }, double>(lookup, "sin_cos");
    add<[](double x) { return x * (std::numbers::pi / 180.0); }, double>(lookup, "to_radians");
    add<[](double x) { return x * (180.0 / std::numbers::pi); }, double>(lookup, "to_degrees");
    add<[](double x, double y) { return std::hypot(x, y); }, double, double>(lookup, "hypot");
    add<[](double x) { return std::cbrt(x); }, double>(lookup, "cbrt");
    add<[](double x, double exponent) { return std::pow(x, exponent); }, double, double>(lookup,
                                                                                         "powf");
    add<[](double x) { return std::exp(x); }, double>(lookup, "exp");
    add<[](double x) { return std::exp2(x); }, double>(lookup, "exp2");
    add<[](double x) { return std::log(x); }, double>(lookup, "ln");
    add<[](double x, double base) { return std::log(x) / std::log(base); }, double, double>(lookup,
                                                                                            "log");
    add<[](double x) { return std::log2(x); }, double>(lookup, "log2");
    add<[](double x) { return std::log10(x); }, double>(lookup, "log10");
    add<[](double x) { return 1.0 / x; }, double>(lookup, "recip");
    add<[](double x, double multiplier, double addend) { return std::fma(x, multiplier, addend); },
        double, double, double>(lookup, "mul_add");
    add<rem_euclid, double, double>(lookup, "rem_euclid");
    add<[](double magnitude, double sign) { return std::copysign(magnitude, sign); }, double,
        double>(lookup, "copysign");
}
} // namespace cel::stdlib
