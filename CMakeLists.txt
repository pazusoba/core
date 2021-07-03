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

# This is needed to find headers
include_directories(include)
file(GLOB PAZUSOBA_SOURCES "src/*.cpp")
add_library(
    pazusoba SHARED
    ${PAZUSOBA_SOURCES}
)
