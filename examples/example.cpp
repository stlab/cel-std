// SPDX-License-Identifier: BSL-1.0
#include <cel/parser/compiler.hpp>
#include <cel/std.hpp>
#include <iostream>
#include <tuple>
#include <utility>

/// Installs the standard library and prints scalar and tuple expression results.
///
/// Complexity: O(expression compilation and execution) time and program storage.
int main() {
    cel::op_lookup lookup;
    cel::stdlib::install(lookup);
    cel::parser parser(std::move(lookup));
    cel::raw_stack stack;

    auto scalar = parser.parse_str("clamp(sqrt(9.0), 0.0, 4.0)");
    std::cout << scalar.prepare<double>().call(stack) << '\n';

    auto pair = parser.parse_str("0.0.sin_cos()");
    const auto [sine, cosine] = pair.prepare<std::tuple<double, double>>().call(stack);
    std::cout << sine << ", " << cosine << '\n';
    return 0;
}
