cmake_minimum_required(VERSION 3.24)

include(cmake/CPM.cmake)
CPMAddPackage("gh:stlab/cpp-library@6.1.0")
include("${cpp-library_SOURCE_DIR}/cpp-library.cmake")
cpp_library_enable_dependency_tracking()

project(std LANGUAGES CXX)

include(CTest)

# Initialize the shared CEL_INSTALL default before same-namespace dependencies.
cpp_library_setup(
    DESCRIPTION "The f64 CEL standard library"
    NAMESPACE cel
    HEADERS std.hpp
    SOURCES std.cpp
    TESTS std_test.cpp
    EXAMPLES example.cpp
    LICENSE_FILES licenses/CPM-MIT.txt
    DOCS_INPUTS README.md docs/tutorial.md docs/reference.md
    DOCS_EXCLUDE_SYMBOLS cel::op_lookup
    DOCS_OPTIONS
        "USE_MDFILE_AS_MAINPAGE = ${CMAKE_CURRENT_SOURCE_DIR}/README.md"
)
CPMAddPackage("gh:stlab/cel-parser@1.1.1")
target_link_libraries(std PUBLIC cel::parser)
