cmake_minimum_required(VERSION 3.24)

include(cmake/CPM.cmake)
CPMAddPackage("gh:stlab/cpp-library@5.8.1")
include("${cpp-library_SOURCE_DIR}/cpp-library.cmake")
cpp_library_enable_dependency_tracking()

project(std LANGUAGES CXX)
set(CMAKE_CXX_EXTENSIONS OFF)
if(PROJECT_IS_TOP_LEVEL)
    include(CTest)
endif()

option(CEL_INSTALL "Enable installation for CEL libraries" ${PROJECT_IS_TOP_LEVEL})
CPMAddPackage(
    NAME cel-parser
    GITHUB_REPOSITORY stlab/cel-parser
    GIT_TAG fac94057fd8a26a06ababbd453b86050584a3c61
)

if(NOT CPP_LIBRARY_VERSION)
    set(CPP_LIBRARY_VERSION "0.1.0")
endif()
cpp_library_setup(
    DESCRIPTION "The f64 CEL standard library"
    NAMESPACE cel
    REQUIRES_CPP_VERSION 20
    HEADERS std.hpp
    SOURCES std.cpp
    TESTS std_test.cpp
)
cpp_library_map_dependency("cel::parser" "cel-parser CONFIG")
target_link_libraries(std PUBLIC cel::parser)
