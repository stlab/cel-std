# C++ CEL standard library

Date: 2026-11-13
Status: Approved

## Intent

Create the public `stlab/cel-std` repository as a C++ port of the floating-point
subset of Rust `cel-std`. C++ applications install the library into the existing
`stlab/cel-parser` lookup and execute expressions through `stlab/cel-runtime`.

The user selected a public repository and explicitly excluded `powi` while
retaining `sin_cos`.

## Goals and boundaries

Preserve Rust CEL operation names, operand order, arities, result types, and
documented numeric behavior for f64 operations. Map f64 to C++ `double`.
Register no f32 or integer overloads. Do not modify the runtime or parser ports,
extend CEL syntax, add implicit conversions, or provide a parallel math API.

The supported operations are:

```text
min max clamp abs signum sqrt floor ceil trunc fract round_ties_even round
sin cos tan asin acos atan atan2 sin_cos to_radians to_degrees hypot cbrt
powf exp exp2 ln log log2 log10 recip mul_add rem_euclid copysign
```

All operands and scalar results are f64. `sin_cos(f64)` returns the structural
CEL tuple `(f64, f64)`, ordered sine then cosine. `powi` is not registered.
Ordinary calls and receiver calls resolve to the same signatures. Persistent
registrations remain available in closure bodies.

## Approaches

A compiled installer library matches the existing parser package, keeps
implementation details private, and gives consumers a small public interface.
This is the recommended approach.

A header-only implementation removes one compiled artifact but exposes numeric
adapters and registration machinery in downstream translation units. It offers
no integration benefit for this port.

A standalone math library with a separate parser adapter adds an API and
packaging boundary that the requested CEL port does not need.

## Public interface and ownership

Provide `<cel/std.hpp>`, the CMake target `cel::std`, and
`cel::stdlib::install(cel::op_lookup&)`. Use `stdlib`, rather than the earlier
suggested `cel::std`, to avoid shadowing the C++ standard namespace inside `cel`.
These spellings are proposed builder choices, not existing upstream APIs.

The installer adds complete named operation signatures through the parser's
public registration APIs. It retains no references to the caller's lookup.
Emitted numeric callbacks own no mutable shared state. Programs use the existing
runtime ownership and prepared-call interfaces.

As in Rust, the installer requires that none of its exact signatures is already
installed. Disjoint overloads and unrelated user registrations remain valid.
Do not add transactional copies or promise rollback after registration failure.
Document installation complexity against the actual parser registration costs.

## Numeric behavior and failures

Use C++20 standard-library facilities wherever they satisfy the Rust contract:
for example, `<cmath>` functions, `std::fma`, and angle-conversion constants.
Keep small private adapters for semantic differences.

In particular, preserve NaN handling in min/max and signum, signed-zero behavior
where specified, ties-to-even rounding independent of the ambient rounding mode,
the Rust definition of fract and rem_euclid, and argument order for atan2,
log, copysign, and mul_add. Do not replace rem_euclid with std::remainder.

Clamp rejects reversed or NaN bounds at evaluation time with the message
`invalid clamp bounds`; NaN input with valid bounds follows Rust behavior.
Other floating-point domain errors produce floating-point results, including
NaN and infinity, rather than new evaluation exceptions. NaN payloads and
bit-identical transcendental results are not promised by the Rust book.
Use tolerance comparisons for approximate results.

## Build, dependencies, and repository

Use C++20, `stlab/cpp-library`, and CPM in the pattern established by the parser
port. Pin dependencies to verified revisions. Depend publicly on `cel::parser`,
which supplies the runtime dependency; avoid fetching a second runtime version.
Follow the ports' shared `CEL_INSTALL` convention before adding dependencies.

Support both CPM source consumption and an installed CMake package, including
transitive dependency discovery. Use the existing ports' license conventions
after verifying their license files. Include build/test CI and Doxygen
documentation generation using cpp-library's supported mechanisms.

Create the new repository beside the existing ports and implement on a branch
in a worktree under its `.claude/worktrees/`. Do not modify cel-rs or commit
implementation directly to main.

## Documentation

The README orients C++ consumers and links to the tutorial and API reference.
The Doxygen tutorial shows dependency setup, installation, parsing, execution,
receiver calls, and tuple extraction with compiled examples.

The reference documents the installer, package, subset boundary, and signatures.
Each retained operation links to its corresponding section in the published
CEL language book instead of duplicating operation prose. Verify the published
base URL and anchors before using them. Clearly state that the linked Rust book
also describes types and powi that this C++ subset does not support.

## Validation and acceptance

Use the ports' existing test framework and cpp-library checks. Test every
operation through parsed CEL, including both call forms and operand order.
Cover closure visibility, tuple typing and extraction, unrelated registrations,
and absence of powi and unsupported overloads.

Add focused numeric edge cases for NaN, infinity, signed zeros, halfway values,
nondefault rounding modes, invalid clamp bounds, Euclidean remainders, and fused
multiply-add. Derive expected results from the Rust/public book contracts.
Exercise programs after their parser is destroyed.

Build and run examples and documentation snippets. Generate Doxygen without
unresolved internal references. Verify an independent source consumer and an
installed-package consumer. Run available compiler, formatter, and sanitizer
checks appropriate to the new C++ library, and report any unavailable checks.

## Builder decisions

Choose internal adapter organization and registration helpers after examining
the exact current parser API. Verify cpp-library's current supported setup,
formatting, CI, packaging, and documentation commands rather than inventing
targets. Resolve standard-library semantic differences with the smallest
contracted, unit-tested adapters.

Use the `sean-parent` GitHub account for the new repository, as explicitly
requested by the user. Switch the active GitHub CLI account with `gh auth switch`
and select that username in the new repository's local Git credential settings.
Verify permission to create `stlab/cel-std`. If creation is denied, retain
the local repository and report the remote-creation blocker.
