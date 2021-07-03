cmake_minimum_required (VERSION 3.10)

message("-- Start building pazusoba")

project(
    pazusoba
    VERSION 0.8.0
    LANGUAGES CXX
    HOMEPAGE_URL https://github.com/HenryQuan/pazusoba
    DESCRIPTION "Solve Puzzle & Dragons with C++"
)

message("-- Getting headers and sources")

file(GLOB PAZUSOBA_HEADERS "include/*.h")
file(GLOB PAZUSOBA_SOURCES "src/*.cpp")
add_library(
    pazusoba_lib
    ${PAZUSOBA_HEADERS}
    ${PAZUSOBA_SOURCES}
)
set_target_properties(
    pazusoba_lib 
    PROPERTIES PUBLIC_HEADER include/core.h
)
