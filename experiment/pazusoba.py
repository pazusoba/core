"""
Call methods exported in the shared library
"""
from ctypes import *
from typing import List
import os


class c_location(Structure):
    _fields_ = [("row", c_int),
                ("column", c_int)]


class c_state(Structure):
    _fields_ = [("combo", c_int),
                ("max_combo", c_int),
                ("step", c_int),
                ("row", c_int),
                ("column", c_int),
                ("routes", c_location*151)]


libpazusoba = CDLL("libpazusoba.so", winmode=0)
# NOTE: the restype here must be correct to call it properly
libpazusoba.explore.restype = c_state
libpazusoba.explore.argtypes = (c_int, POINTER(c_char_p))


def explore(arguments: List[str]) -> c_state:
    # additional step is required here because Mac is stricter than Windows
    argv = []
    for s in arguments:
        argv.append(c_char_p(s.encode("ascii")))
    c_argc = len(arguments)
    c_argv = (c_char_p * c_argc)()
    c_argv[:] = argv

    state = libpazusoba.explore(c_argc, c_argv)
    return state


if __name__ == "__main__":
    s = explore(
        ["pazusoba", "RLRRDBHBLDBLDHRGLGBRGLBDBHDGRL", "3", "100", "500"])
    print(s.combo, s.max_combo, s.step, s.row, s.column)
    for i in range(150):
        if i > s.step:
            break
        print(s.routes[i].row, s.routes[i].column)
