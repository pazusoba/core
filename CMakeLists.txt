cmake_minimum_required (VERSION 3.10)

project(
    pazusoba
    VERSION 0.8.0
    LANGUAGES CXX
    HOMEPAGE_URL https://github.com/HenryQuan/pazusoba
    DESCRIPTION "Solve Puzzle & Dragons with C++"
)

# compiler settings
if(CMAKE_COMPILER_IS_GNUCXX)
    message(STATUS "GCC detected, adding compiler flags")
    add_compile_options(-Wall -Wextra -pedantic -O2)
elseif(CMAKE_COMPILER_IS_GNUCXX)
    # Assume that this is visual studio on Windows
    message(STATUS "Not GCC, using different compiler flags")
    add_compile_options(-Wall -Werror -O2)
endif(CMAKE_COMPILER_IS_GNUCXX)

set(CMAKE_CXX_STANDARD 20)

# include headers and get all source files
include_directories(include)
file(GLOB PAZUSOBA_SOURCES "src/*.cpp")

add_library(
    pazusoba SHARED
    ${PAZUSOBA_SOURCES}
)

add_executable(
    pazusoba_release
    ${PAZUSOBA_SOURCES}
)
