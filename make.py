#!/usr/local/bin/python3
"""
A script for generating makefiles from CMakeLists.txt
"""

import os
import sys
import shutil


def make():
    # simple make
    extra_flags = ""
    if os.name == "nt":
        # for now, MinGW should be used, MSVC may be supported in the future
        extra_flags = '-G "MinGW Makefiles"'
    os.system("cmake -B release {}".format(extra_flags))
    os.system("cmake -B debug -D CMAKE_BUILD_TYPE=Debug {}".format(extra_flags))

    print("See CMakeLists.txt for all targets")


def clean():
    shutil.rmtree("debug")
    shutil.rmtree("release")


def test():
    if not os.path.exists("debug/"):
        make()

    if os.name == "nt":
        # TODO: fix it for Windows
        os.system("cd debug && mingw32.exe test_pazusoba && ./test_pazusoba.exe")
    else:
        os.system("cd debug && make test_pazusoba && ./test_pazusoba")


argv = sys.argv
argc = len(argv)
if argc <= 1:
    make()
elif argc == 2:
    option = argv[1]
    if option == "test":
        test()
    elif option == "clean":
        clean()
    else:
        exit("Unknown command - (test, clean) avilable")
