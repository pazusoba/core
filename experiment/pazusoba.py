"""
Call methods exported in the shared library
"""
from ctypes import *
from typing import List
import os

libpazusoba = CDLL("libpazusoba.so", winmode=0)
# NOTE: the restype here must be correct to call it properly
libpazusoba.explore.restype = None
libpazusoba.explore.argtypes = (c_int, POINTER(c_char_p))


def explore(arguments: List[str]) -> None:
    # additional step is required here because Mac is stricter than Windows
    argv = []
    for s in arguments:
        argv.append(c_char_p(s.encode("ascii")))
        # argv.append(create_string_buffer(s.encode('ascii')))
    c_argc = len(arguments)
    c_argv = (c_char_p * c_argc)()
    c_argv[:] = argv

    libpazusoba.explore(c_argc, c_argv)


if __name__ == "__main__":
    explore(["pazusoba", "RHLBDGPRHDRJPJRHHJGRDRHLGLPHBB", "3", "100", "10000"])
