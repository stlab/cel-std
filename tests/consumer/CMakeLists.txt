cmake_minimum_required(VERSION 3.24)
project(cel_std_consumer LANGUAGES CXX)
enable_testing()

if(CEL_STD_SOURCE_DIR)
    include("${CEL_STD_SOURCE_DIR}/cmake/CPM.cmake")
    CPMAddPackage(NAME cel-std SOURCE_DIR "${CEL_STD_SOURCE_DIR}")
else()
    find_package(cel-std CONFIG REQUIRED)
endif()

add_executable(consumer main.cpp)
target_link_libraries(consumer PRIVATE cel::std)
add_test(NAME consumer COMMAND consumer)
