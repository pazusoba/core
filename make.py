#!/usr/local/bin/python3
"""
A script for generating makefiles from CMakeLists.txt
"""

import os, sys

extra_flags = ""
if os.name == "nt":
    # for now, MinGW should be used, MSVC may be supported in the future
    extra_flags = '-G "MinGW Makefiles"'
os.system("cmake -B release {}".format(extra_flags))
os.system("cmake -B debug -D CMAKE_BUILD_TYPE=Debug {}".format(extra_flags))

print("See CMakeLists.txt for all targets")
