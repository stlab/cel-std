cmake_minimum_required(VERSION 3.24)

include(cmake/CPM.cmake)
CPMAddPackage("gh:stlab/cpp-library@5.8.1")
include("${cpp-library_SOURCE_DIR}/cpp-library.cmake")
cpp_library_enable_dependency_tracking()

project(std LANGUAGES CXX)
set(CMAKE_CXX_EXTENSIONS OFF)
include(GNUInstallDirs)

include(CTest)

option(CEL_INSTALL "Enable installation for CEL libraries" ${PROJECT_IS_TOP_LEVEL})
CPMAddPackage("gh:stlab/cel-parser@1.0.0")

cpp_library_setup(
    DESCRIPTION "The f64 CEL standard library"
    NAMESPACE cel
    REQUIRES_CPP_VERSION 20
    HEADERS std.hpp
    SOURCES std.cpp
    TESTS std_test.cpp
    EXAMPLES example.cpp
    DOCS_INPUTS README.md docs/tutorial.md docs/reference.md
    DOCS_OPTIONS
        "USE_MDFILE_AS_MAINPAGE = ${CMAKE_CURRENT_SOURCE_DIR}/README.md"
        "EXCLUDE_SYMBOLS = cel::op_lookup"
        "WARN_AS_ERROR = YES"
        "FULL_PATH_NAMES = NO"
        "STRIP_FROM_INC_PATH = ${CMAKE_CURRENT_SOURCE_DIR}/include"
)
cpp_library_map_dependency("cel::parser" "cel-parser CONFIG")
target_link_libraries(std PUBLIC cel::parser)
if(CEL_INSTALL)
    install(FILES LICENSE licenses/CPM-MIT.txt
        DESTINATION "${CMAKE_INSTALL_DATADIR}/licenses/cel-std")
endif()

if(PROJECT_IS_TOP_LEVEL AND BUILD_TESTING)
    configure_file(tests/consumer.cmake.in consumer.cmake @ONLY)
    foreach(mode source installed)
        if(mode STREQUAL "installed" AND NOT CEL_INSTALL)
            continue()
        endif()
        add_test(NAME consumer_${mode}
            COMMAND "${CMAKE_COMMAND}" "-DMODE=${mode}" "-DCONFIG=$<CONFIG>"
                -P "${CMAKE_CURRENT_BINARY_DIR}/consumer.cmake")
        set_tests_properties(consumer_${mode} PROPERTIES TIMEOUT 300)
    endforeach()
endif()
