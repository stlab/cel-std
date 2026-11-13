# C++ f64 CEL standard library handoff

## Completed

The port installs all 35 Rust f64 operations except `powi`, with structural
`sin_cos` tuples, ordinary and receiver calls, and persistent closure visibility.
The C++20 library uses cpp-library and CPM and exports `cel::std`.

The Doxygen tutorial includes the compiled example. The reference links each
retained signature to its published CEL language-book contract. Markdown
reference input fixes navigation in both GitHub and generated Doxygen pages.

The independent branch review identified the reference-navigation issue, now
fixed and verified against generated HTML. A separate standard-contract check
also kept NaN values outside `std::clamp`'s ordering preconditions.

Local validation passes: 12 unit cases with 248 assertions; the complete
four-test suite, including the example and independent source/installed consumers;
Debug, Release, unspecified-build-type, and multi-config builds; clang-tidy;
AddressSanitizer and UndefinedBehaviorSanitizer; warning-clean compilation;
formatting; Doxygen warnings-as-errors; and 35 published operation anchors.

## Deliberately excluded

Integer overloads, f32 overloads, and `powi` are outside the approved subset.
No runtime or parser changes are needed. No review finding is deferred.
The library does not promise identical transcendental bits, NaN payloads,
preserved floating-point exception flags, or fast-math compatibility.

## Remaining

Publish the reviewed branch and pull request. Confirm the configured Linux,
macOS, Windows, analyzer, and documentation CI jobs at the pushed head.
Integration into `main` requires a separate user decision; do not merge
automatically.
