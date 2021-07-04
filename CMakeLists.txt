cmake_minimum_required (VERSION 3.10)

project(
    pazusoba
    VERSION 0.8.0
    LANGUAGES CXX
    HOMEPAGE_URL https://github.com/HenryQuan/pazusoba
    DESCRIPTION "Solve Puzzle & Dragons with C++"
)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED true)

set(MAIN_CPP test/pazusoba.cpp)
set(TEST_CPP test/test.cpp)

set(GNU_COMPILER_FLAGS -Wall -Wextra -pedantic)
set(OTHER_COMPILER_FLAGS -Wall -Werror)
set(DEBUG_COMPILER_FLAGS -g)

# compiler settings
if(CMAKE_COMPILER_IS_GNUCXX OR MINGW)
    message(STATUS "GCC - adding compiler flags")
    add_compile_options(${GNU_COMPILER_FLAGS})
elseif(MSVC)
    # Assume that this is visual studio on Windows
    message(STATUS "MSVC - using different compiler flags")
    add_compile_options(${OTHER_COMPILER_FLAGS})
endif()

set(CMAKE_BUILD_TYPE RELEASE)

# Add O2 to all release build
if(CMAKE_BUILD_TYPE MATCHES Release)
    message(STATUS "Apply optimisation")
    add_compile_options(-O2)
endif()

# add {fmt} module
add_subdirectory(src/fmt EXCLUDE_FROM_ALL)
link_libraries(fmt::fmt-header-only)

# include headers and get all source files
include_directories(include)
file(GLOB PAZUSOBA_SOURCES "src/*.cpp")

# shared library
add_library(
    pazusoba 
    SHARED
    ${PAZUSOBA_SOURCES}
)

# pazuauto
add_executable(
    pazuauto
    ${MAIN_CPP}
    ${PAZUSOBA_SOURCES}
)

# pazusoba binary
add_executable(
    pazusoba_binary
    ${MAIN_CPP}
    ${PAZUSOBA_SOURCES}
)

# pazusoba debug binary
add_executable(
    pazusoba_debug
    ${MAIN_CPP}
    ${PAZUSOBA_SOURCES}
)
target_compile_options(
    pazusoba_debug 
    PRIVATE ${DEBUG_COMPILER_FLAGS}
)

# pazusoba test
add_executable(
    test_pazusoba
    ${TEST_CPP}
    ${PAZUSOBA_SOURCES}
)
target_compile_options(
    test_pazusoba 
    PRIVATE ${DEBUG_COMPILER_FLAGS}
)
