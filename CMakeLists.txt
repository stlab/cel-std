cmake_minimum_required(VERSION 3.24)

include(cmake/CPM.cmake)
CPMAddPackage("gh:stlab/cpp-library@6.2.0")
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
CPMAddPackage("gh:stlab/cel-parser@1.1.2")
target_link_libraries(std PUBLIC cel::parser)

# Standalone tests use public headers, not the toolkit's private source include root.
get_property(directory_targets DIRECTORY PROPERTY BUILDSYSTEM_TARGETS)
foreach(directory_target IN LISTS directory_targets)
    get_target_property(include_directories "${directory_target}" INCLUDE_DIRECTORIES)
    if(include_directories)
        list(REMOVE_ITEM include_directories "${CMAKE_CURRENT_SOURCE_DIR}/src")
        set_property(TARGET "${directory_target}" PROPERTY INCLUDE_DIRECTORIES "${include_directories}")
    endif()
endforeach()
