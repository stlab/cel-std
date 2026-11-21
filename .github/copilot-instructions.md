# Repository guidance

`cel-std` is a C++20 library that installs the Rust CEL standard library's
35 retained `f64` operations into the C++ CEL parser. Its public CMake target
is `cel::std`; the installed package name is `cel-std`.

## Build, test, and lint

Run commands from the repository root. Use CMake 3.24 or later, a C++20
compiler, and Ninja for the presets. CPM downloads pinned dependencies into
`.cache/cpm`; dependency declarations live in `CMakeLists.txt`.

Configure, build, and run the unit-test executable and compiled example:

```sh
cmake --preset=test
cmake --build --preset=test --parallel 2
ctest --preset=test --output-on-failure --no-tests=error
```

CTest registers `std_test`, `std_in_source_tests`, and `example`, not individual
doctest cases. The generated in-source runner has no cases until private tests
are added to production translation units.
Run a single doctest case directly after building:

```sh
build/test/std_test --list-test-cases
build/test/std_test \
  --test-case="sin_cos returns a structural sine then cosine tuple"
```

Run only the example through CTest:

```sh
ctest --preset=test -R '^example$' --output-on-failure
```

Run static analysis with the repository's `.clang-tidy` configuration and
`clang-tidy` on `PATH`:

```sh
cmake --preset=clang-tidy
cmake --build --preset=clang-tidy --parallel 2
ctest --preset=clang-tidy
```

Use `.clang-format` for C++ formatting.

Build the Release library without tests:

```sh
cmake --preset=default
cmake --build --preset=default --parallel 2
```

Build documentation with Doxygen 1.13.1 or later:

```sh
cmake --preset=docs
cmake --build --preset=docs
```

The generated site is `build/docs/html/index.html`. CI tests Ubuntu GCC,
Ubuntu Clang, macOS, and Windows and runs clang-tidy using source builds only.
Do not add or run installation, installed-consumer, or package-artifact tests.
The docs CI job runs on published releases.

## Architecture

- `include/cel/std.hpp` exposes `cel::stdlib::install(cel::op_lookup&)`.
  `src/std.cpp` contains the registrations and numeric adapters; parsing
  and execution belong to the external `cel-parser` and `cel-runtime`
  libraries. `cel::parser` is a public dependency and supplies the runtime
  transitively.
- The internal `add<Function, A...>` helper registers both the typed
  operation with `register_op` and a stateless `function_marker` with
  `register_function`. Preserve both registrations: ordinary calls,
  receiver calls, and calls inside closure bodies use this setup.
- Install into a lookup before moving it into `cel::parser`. Compile with
  `parse_str`, prepare the C++ result type, and execute with a caller-owned
  `cel::raw_stack`. Emitted programs retain their callbacks and can outlive
  the parser.
- `cpp_library_setup` from `stlab/cpp-library` defines the library, tests,
  example, documentation, and packaging. Prefer its supported parameters
  over duplicating its defaults. The `init` preset forces template
  regeneration, including presets and CI; do not use it as a normal build.
- Installation is non-recursive: install runtime, parser, and this library
  separately, in that order. See `docs/tutorial.md` for the
  `CPM_USE_LOCAL_PACKAGES` and `CMAKE_PREFIX_PATH` commands.

## Numeric and registration conventions

- Keep the port limited to `double` operands and results, except for
  `sin_cos`, which returns a structural CEL tuple adapted as
  `std::tuple<double, double>`. Integer and `f32` overloads and `powi` are
  outside this port. `docs/reference.md` lists the retained signatures.
- Receiver syntax supplies the first operand and must match ordinary-call
  argument order. Install each exact signature once per lookup; unrelated
  registrations and disjoint overloads must remain usable. Registration
  failures propagate, and installation is not transactional.
- Preserve Rust operation semantics rather than blindly substituting
  similarly named C++ functions. Existing adapters handle nearest-even
  rounding, Euclidean remainder, and clamp validation. `mul_add` uses
  `std::fma` for a single rounding step.
- Require IEEE 754 binary64 and avoid fast-math modes that discard NaNs,
  infinities, or signed zeros. `round_ties_even` must remain independent of
  the ambient rounding mode without changing it. Compare transcendental
  results with tolerances; NaN payloads and signs are unspecified.
- Invalid clamp bounds throw
  `std::invalid_argument("invalid clamp bounds")`, including NaN bounds.
  Other numeric domain failures produce NaN or infinity as specified by
  the linked CEL language book. Unsupported signatures fail during parsing
  with `cel::parse_error`.

## Tests and documentation

`tests/std_test.cpp` owns the public-contract doctest main. Its `evaluate<R>` helper destroys
the parser before executing the emitted program, so expression tests also
exercise callback lifetime. The scalar case table covers both ordinary and
receiver syntax; separate cases cover closures, overload coexistence,
rejected signatures, floating-point edge cases, and tuple structure.
Keep these public-interface tests external. Private tests, if added, belong in
their owning `.cpp` under `#if CPP_LIBRARY_TESTING()` after including
`<cpp-library/testing.hpp>`; guard doctest and test-only helpers there too.
Use the toolkit's thread, static-test-support, generated-runner, and 120-second
timeout defaults. Standalone tests must not receive the private `src` include root.

`examples/example.cpp` is compiled and run by CTest and included directly
in the Doxygen tutorial. Keep that example consistent with the tutorial.
`README.md`, `docs/tutorial.md`, `docs/reference.md`, and the public header
feed the generated documentation. Use the CEL language-book links in
`docs/reference.md` for operation contracts, accounting for this port's
smaller type and operation set.
