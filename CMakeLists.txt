cmake_minimum_required(VERSION 3.24)

include(cmake/CPM.cmake)
CPMAddPackage("gh:stlab/cpp-library@6.0.2")
include("${cpp-library_SOURCE_DIR}/cpp-library.cmake")
cpp_library_enable_dependency_tracking()

project(std LANGUAGES CXX)

include(CTest)

option(CEL_INSTALL "Enable installation for CEL libraries" ${PROJECT_IS_TOP_LEVEL})
CPMAddPackage("gh:stlab/cel-parser@1.1.0")

cpp_library_setup(
    DESCRIPTION "The f64 CEL standard library"
    NAMESPACE cel
    HEADERS std.hpp
    SOURCES std.cpp
    TESTS std_test.cpp
    EXAMPLES example.cpp
    DOCS_INPUTS README.md docs/tutorial.md docs/reference.md
    DOCS_EXCLUDE_SYMBOLS cel::op_lookup
    DOCS_OPTIONS
        "USE_MDFILE_AS_MAINPAGE = ${CMAKE_CURRENT_SOURCE_DIR}/README.md"
)
target_link_libraries(std PUBLIC cel::parser)
if(CEL_INSTALL)
    install(FILES LICENSE licenses/CPM-MIT.txt
        DESTINATION "${CMAKE_INSTALL_DATADIR}/licenses/cel-std")
endif()
