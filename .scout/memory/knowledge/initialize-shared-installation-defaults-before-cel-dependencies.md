---
title: "Initialize shared installation defaults before CEL dependencies"
entity_type: pattern
confidence: 0.80
created: 2026-11-19T22:20:26Z
last_accessed: 2026-11-19T22:20:26Z
last_reinforced: 2026-11-19T22:20:26Z
access_count: 1
sources:
  - "CMakeLists.txt:12-27"
  - "https://github.com/stlab/cpp-library/blob/v6.1.0/cmake/cpp-library-install.cmake#L486-L497"
tags: ["cel-install", "cpp_library_setup", "cpmaddpackage", "installation-defaults", "fresh-cmake-cache"]
tier: knowledge
source: inferred
---

# Initialize shared installation defaults before CEL dependencies

Keep cel-std's cpp_library_setup call before the cel-parser CPMAddPackage call; link cel::parser after both targets exist (CMakeLists.txt:12-27).

CEL packages share the CEL_INSTALL cache option. cpp-library initializes it from PROJECT_IS_TOP_LEVEL only on its first declaration. If a nested parser/runtime package declares it first, a fresh build can cache OFF and cel-std installs nothing. Initializing the root through cpp_library_setup first gives the intended top-level default while preserving an explicit CEL_INSTALL=OFF. Do not restore a library-specific predeclaration of the shared option.

When changing dependency/setup order, inspect a fresh source-build CMake cache to confirm CEL_INSTALL's default and explicit OFF handling. Do not exercise installation or package-artifact tests; the user requires source-only validation.

Aliases / also known as: CEL_INSTALL, cpp_library_setup, CPMAddPackage, installation defaults, fresh CMake cache
