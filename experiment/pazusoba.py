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


class Location:
    def __init__(self, raw):
        self.row = raw.row
        self.column = raw.column

    def __str__(self):
        return "({}, {})".format(self.row, self.column)

    def __repr__(self):
        return "({}, {})".format(self.row, self.column)


class State:
    def __init__(self, raw):
        self.combo = raw.combo
        self.max_combo = raw.max_combo
        self.step = raw.step
        self.row = raw.row
        self.column = raw.column
        self.routes = [Location(raw.routes[i])
                       for i in range(151) if raw.routes[i].row != -1]

        # shorten routes
        # insert the first
        self.simplified_routes = [self.routes[0]]
        length = len(self.routes)

        # loop from the second step
        for i in range(1, length - 1):
            x0 = self.routes[i].row - self.routes[i - 1].row
            x1 = self.routes[i + 1].row - self.routes[i].row
            if x0 == x1:
                y0 = self.routes[i].column - self.routes[i - 1].column
                y1 = self.routes[i + 1].column - self.routes[i].column
                if y0 == y1:
                    continue
            self.simplified_routes.append(self.routes[i])

        # insert the last
        self.simplified_routes.append(self.routes[-1])
        self.simplified_step = len(self.simplified_routes)

    def __str__(self):
        return "Combo: {}\nMax Combo: {}\nStep: {} ({})\nRow: {}\nColumn: {}\nRoutes: {}\nSimplified Routes: {}".format(
            self.combo, self.max_combo, self.step, self.simplified_step, self.row, self.column, self.routes, self.simplified_routes)

    def __repr__(self):
        return "Combo: {}\nMax Combo: {}\nStep: {} ({})\nRow: {}\nColumn: {}\nRoutes: {}\nSimplified Routes: {}".format(
            self.combo, self.max_combo, self.step, self.simplified_step, self.row, self.column, self.routes, self.simplified_routes)


libpazusoba = CDLL("libpazusoba.so", winmode=0)
# NOTE: the restype here must be correct to call it properly
libpazusoba.explore.restype = c_state
libpazusoba.explore.argtypes = (c_int, POINTER(c_char_p))


def explore(arguments: List[str]) -> State:
    # additional step is required here because Mac is stricter than Windows
    argv = []
    for s in arguments:
        argv.append(c_char_p(s.encode("ascii")))
    c_argc = len(arguments)
    c_argv = (c_char_p * c_argc)()
    c_argv[:] = argv

    state = libpazusoba.explore(c_argc, c_argv)
    return State(state)


if __name__ == "__main__":
    state = explore(
        ["pazusoba", "RLRRDBHBLDBLDHRGLGBRGLBDBHDGRL", "3", "100", "5000"])
    print(state)
