cmake_minimum_required (VERSION 3.10)

project(
    pazusoba
    VERSION 0.8.0
    LANGUAGES CXX
    HOMEPAGE_URL https://github.com/HenryQuan/pazusoba
    DESCRIPTION "Solve Puzzle & Dragons with C++"
)

if(MSVC)
    message(FATAL_ERROR "MSVC is not supported. Use `MinGW Makefiles` instead")
endif()

# TODO: is C++17 required excpept for the nested namespace?
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED true)

set(MAIN_CPP test/pazusoba.cpp)
set(TEST_CPP test/test.cpp)

# strict compiler warnings
set(GNU_COMPILER_FLAGS -Wall -Wextra -pedantic)
set(OPTIMISATION_FLAGS -O2)
set(DEBUG_COMPILER_FLAGS -g)

message(STATUS "Add {fmt} module")
# add {fmt} module
add_subdirectory(src/fmt EXCLUDE_FROM_ALL)
link_libraries(fmt::fmt-header-only)

# include headers and get all source files
include_directories(include)
# fiind all source files under src/
file(GLOB_RECURSE PAZUSOBA_SOURCES "src/*.cpp")

# use release by default
if (NOT CMAKE_BUILD_TYPE)
    message(NOTICE "CMAKE_BUILD_TYPE is not set, use release as default")
    set(CMAKE_BUILD_TYPE RELEASE)
endif()

string(TOLOWER ${CMAKE_BUILD_TYPE} build_type)
# check if current config is release
if(${build_type} STREQUAL "release")
    message(STATUS "Generating for RELEASE")
    add_compile_options(${GNU_COMPILER_FLAGS} ${OPTIMISATION_FLAGS})

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

    # pazusoba test in release mode in case anything is different
    add_executable(
        test_pazusoba
        ${TEST_CPP}
        ${PAZUSOBA_SOURCES}
    )
else()
    message(STATUS "Generating for DEBUG")
    add_compile_options(${GNU_COMPILER_FLAGS} ${DEBUG_COMPILER_FLAGS})
 
    # pazusoba debug binary
    add_executable(
        debug_pazusoba
        ${MAIN_CPP}
        ${PAZUSOBA_SOURCES}
    )

    # pazusoba test
    add_executable(
        test_pazusoba
        ${TEST_CPP}
        ${PAZUSOBA_SOURCES}
    )
endif()
