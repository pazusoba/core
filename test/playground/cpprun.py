#!/usr/local/bin/python3

import os
import sys
import platform

name = sys.argv[1]
if os.path.exists("a.out"):
    os.system("rm a.out")
if os.path.exists("a.exe"):
    os.system("del a.exe")
os.system("g++ --std=c++11 {}".format(name))
if platform.system() == "Windows":
    os.system("a.exe")
else:
    os.system("./a.out")
