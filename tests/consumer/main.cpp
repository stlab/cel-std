// SPDX-License-Identifier: BSL-1.0
#include <cel/parser/compiler.hpp>
#include <cel/std.hpp>
#include <tuple>
#include <utility>

/// Returns success when a standalone consumer can evaluate scalar and tuple operations.
///
/// Complexity: O(expression compilation and execution) time and program storage.
int main() {
    cel::op_lookup lookup;
    cel::stdlib::install(lookup);
    cel::parser parser(std::move(lookup));
    cel::raw_stack stack;
    const auto scalar = parser.parse_str("sqrt(9.0)").prepare<double>().call(stack);
    const auto pair =
        parser.parse_str("0.0.sin_cos()").prepare<std::tuple<double, double>>().call(stack);
    return scalar == 3.0 && pair == std::tuple{0.0, 1.0} ? 0 : 1;
}
