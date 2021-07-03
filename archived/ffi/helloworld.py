from ctypes import *
import os
# https://stackoverflow.com/a/64472088
# winmode 0 is working here and it is required
hello_world = CDLL("./pazusoba.so", winmode=0)
hello_world.pazusoba.restype = c_int
hello_world.pazusoba.argtypes = (c_int, POINTER(POINTER(c_char)),)
# hello_world = CDLL("./helloworld.so")

def pazusoba(argc, argv):
    size = 0
    for s in argv:
        size += len(s)
    
    return hello_world.pazusoba(argc, (c_char * (size + 1))(*argv))

pazusoba(4, ["RHGHDRGLBLHGDBLLHBBBHRLHGHDGLB", "3", "10", "5000"])
