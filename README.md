# cel-std

`cel-std` installs numeric operations into the C++ [CEL parser](
https://github.com/stlab/cel-parser) for execution by [CEL runtime](
https://github.com/stlab/cel-runtime). Use it when your C++ application needs
CEL expressions such as `clamp(sqrt(9.0), 0.0, 4.0)`.

This port retains 35 Rust `cel-std` operations with `f64` (`double`) operands.
It excludes `powi`, all integer overloads, and all `f32` overloads.
`sin_cos` returns a CEL `(f64, f64)` tuple. Ordinary and receiver calls use
the same registrations.

## Add the library

Use C++20 and CMake 3.24 or later. After loading [CPM](
https://github.com/cpm-cmake/CPM.cmake), add the library to your project:

```cmake
CPMAddPackage(
    NAME cel-std
    GITHUB_REPOSITORY stlab/cel-std
    GIT_TAG feat/f64-port
)
target_link_libraries(your_app PRIVATE cel::std)
```

Replace `your_app` with your executable target. The feature branch contains
the initial port; pin a reviewed commit for reproducible builds.
The library brings in compatible, pinned parser and runtime dependencies.

Include `<cel/std.hpp>` and `<cel/parser/compiler.hpp>`, create a
`cel::op_lookup`, and call `cel::stdlib::install(lookup)` before transferring
the lookup into a parser. See the [tutorial](docs/tutorial.md) for a complete,
compiled example and installed-package instructions.

## Build and test

Configure, build, and run all unit, example, and consumer tests:

```sh
cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug -DBUILD_DOCS=OFF
cmake --build build --parallel
ctest --test-dir build --output-on-failure
```

Generate the tutorial and API reference with Doxygen 1.13.1 or later:

```sh
cmake -S . -B build/docs -DBUILD_DOCS=ON -DBUILD_TESTING=OFF
cmake --build build/docs --target docs
```

Open `build/docs/html/index.html`.

## Documentation

The [tutorial](docs/tutorial.md) teaches installation, parsing, execution,
and tuple results. The [reference](docs/reference.md) lists the C++ interface
and retained signatures. For operation behavior and examples, see the
[CEL language book's standard library](
https://stlab.github.io/cel-rs/cel-book/standard-library.html).
The book also covers Rust types and `powi` that this port does not provide.

The implementation uses the C++ standard library and small adapters for
Rust semantics. Build without fast-math modes that discard NaNs, signed zeros,
or IEEE arithmetic. Approximate transcendental results can differ across
standard libraries; NaN payloads are not part of the contract.

## License

Boost Software License 1.0. The CPM bootstrap is MIT-licensed; see
`licenses/CPM-MIT.txt`.
