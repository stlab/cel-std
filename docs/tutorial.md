# Evaluate numeric CEL expressions {#tutorial}

This tutorial shows C++ developers how to install the library, compile
expressions, and retrieve scalar and tuple results. For individual operation
semantics, use the [CEL language book's standard library](
https://stlab.github.io/cel-rs/cel-book/standard-library.html).

## Before you begin

Use a C++20 compiler with IEEE 754 binary64 `double`, CMake 3.24 or later,
and CPM. Configure and build the library as described in the README.
The library depends on `cel::parser`, which supplies `cel::runtime`.
You do not need a separate runtime dependency declaration.

## Compile and run an expression

1. Link your executable against `cel::std`.
2. Include `<cel/std.hpp>` and `<cel/parser/compiler.hpp>`.
3. Call `cel::stdlib::install(lookup)` before moving the lookup into a parser.
4. Compile an expression with `parse_str`.
5. Prepare its C++ result type and execute it with a caller-owned `cel::raw_stack`.

The following complete program is built and run by the test suite:

\include example.cpp

The program prints:

```text
3
0, 1
```

For a multi-configuration generator, select the build configuration when
building and running tests:

```sh
cmake --build build --config Debug
ctest --test-dir build -C Debug --output-on-failure
```

## Use receiver calls and tuples

`sqrt(9.0)` and `9.0.sqrt()` select the same operation. The receiver is
the first operand: `atan2(y, x)` is also `y.atan2(x)`.
The installed functions remain available inside CEL closure bodies.

`sin_cos` produces a structural CEL tuple, not an opaque C++ object.
Prepare it as `std::tuple<double, double>`, as the example shows.
In CEL, `sin_cos(0.0).0` selects sine and `.1` selects cosine.
The program owns its emitted operations and can outlive the parser.
The caller supplies exclusive access to each stack during execution.

## Handle errors

The parser rejects unsupported operand types, arities, and names during
compilation with `cel::parse_error`. Integer and `f32` overloads and `powi`
are not installed.

During evaluation, `clamp` throws
`std::invalid_argument("invalid clamp bounds")` for reversed or NaN bounds.
Other floating-point domain failures return NaN or infinity as specified by
the language book; they do not become new C++ evaluation exceptions.
Allocation and dependency failures propagate to the caller.

Install the library once per lookup. Existing registrations with disjoint
signatures remain usable; installation is not transactional if registration
fails.

## Consume an installed package

To install a release build, run:

```sh
cmake -S . -B build/release -DCMAKE_BUILD_TYPE=Release \
  -DBUILD_TESTING=OFF -DBUILD_DOCS=OFF -DCEL_INSTALL=ON
cmake --build build/release --config Release
cmake --install build/release --config Release --prefix /PATH/TO/PREFIX
```

Replace `/PATH/TO/PREFIX` with your installation prefix. This installs the
standard library and its parser/runtime dependencies.

In the consuming project, find the package and link its exported target:

```cmake
find_package(cel-std CONFIG REQUIRED)
target_link_libraries(your_app PRIVATE cel::std)
```

Replace `your_app` with your executable target. Pass the installation prefix
as `-DCMAKE_PREFIX_PATH=/PATH/TO/PREFIX` when configuring the consumer.
Verify that the example produces the output shown in this tutorial.

See the \ref reference for the complete retained signature table.
