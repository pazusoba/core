#!/usr/local/bin/python3

import os
import sys

name = sys.argv[1]
if os.path.exists("a.out"):
    os.system("rm a.out")
os.system("g++ --std=c++11 {}".format(name))
os.system("./a.out")
