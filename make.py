#!/usr/local/bin/python3
"""
A script for generating makefiles from CMakeLists.txt
"""

import os
import platform
import sys
import shutil


def make():
    # simple make
    extra_flags = ""
    if platform.system() == "Windows":
        # for now, MinGW should be used, MSVC may be supported in the future
        extra_flags = '-G "MinGW Makefiles"'
    os.system("cmake -B release {}".format(extra_flags))
    os.system("cmake -B debug -D CMAKE_BUILD_TYPE=Debug {}".format(extra_flags))

    print("See CMakeLists.txt for all targets")


def clean():
    _clean("build")
    _clean("debug")
    _clean("release")

def _clean(folder: str):
    if os.path.exists(folder):
        shutil.rmtree(folder)

def test():
    if not os.path.exists("debug/"):
        make()

    if platform.system() == "Windows":
        # TODO: fix it for Windows
        os.system("cd debug && mingw32-make.exe test_pazusoba && test_pazusoba.exe")
    else:
        os.system("cd debug && make test_pazusoba && ./test_pazusoba")

def binary():
    if not os.path.exists("release/"):
        make()

    if platform.system() == "Windows":
        # TODO: fix it for Windows
        os.system("cd release && mingw32-make.exe pazusoba_binary")
    else:
        os.system("cd release && make pazusoba_binary")

def xcode():
    if platform.system() == "Darwin":
        os.system("cmake -B xcode -G Xcode -D CMAKE_BUILD_TYPE=Debug")
        os.system("cp -r support xcode")
        os.system("open xcode/pazusoba.xcodeproj")

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
    elif option == "xcode":
        xcode()
    elif option == "binary":
        binary()
    else:
        exit("Unknown command - (test, clean, xcode, binary) avilable")
