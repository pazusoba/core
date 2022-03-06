"""
Call methods exported in the shared library
"""
from ctypes import *
from typing import List
import os
import enum


class c_location(Structure):
    _fields_ = [("row", c_int),
                ("column", c_int)]


class Location:
    def __init__(self, raw):
        self.row = raw.row
        self.column = raw.column

    def __str__(self):
        return "({}, {})".format(self.row, self.column)

    def __repr__(self):
        return "({}, {})".format(self.row, self.column)


class c_state(Structure):
    _fields_ = [("combo", c_int),
                ("max_combo", c_int),
                ("step", c_int),
                ("row", c_int),
                ("column", c_int),
                ("goal", c_bool),
                ("routes", c_location*151),
                ("board", c_char*42)]


class State:
    def __init__(self, raw):
        self.combo = raw.combo
        self.max_combo = raw.max_combo
        self.step = raw.step
        self.row = raw.row
        self.column = raw.column
        self.goal = raw.goal
        self.routes = [Location(raw.routes[i])
                       for i in range(151) if raw.routes[i].row != -1]
        ORB_LIST = ['', 'R', 'B', 'G', 'L', 'D', 'H']
        self.board = "".join([ORB_LIST[b] for b in raw.board])

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
        return "Combo: {}/{}\nStep: {} ({})\nSize: {} x {}\nGoal: {}\nBoard: {}".format(
            self.combo, self.max_combo, self.step, self.simplified_step, self.row, self.column, self.goal, self.board)

    def __repr__(self):
        return "Combo: {}/{}\nStep: {} ({})\nSize: {} x {}\nGoal: {}\nBoard: {}".format(
            self.combo, self.max_combo, self.step, self.simplified_step, self.row, self.column, self.goal, self.board)


orb_list = (c_bool*11)


class Orb(enum.Enum):
    EMPTY = 0
    FIRE = 1
    WATER = 2
    WOOD = 3
    LIGHT = 4
    DARK = 5
    HEAL = 6
    JAMMER = 7
    BOMB = 8
    POISON = 9
    POISON_PLUS = 10


class c_profile(Structure):
    _fields_ = [("name", c_int),
                ("stop_threshold", c_int),
                ("target", c_int),
                ("orbs", orb_list)]


class ProfileName(enum.Enum):
    COMBO = 0             # target certain combo, -1 means max combo
    COLOUR = 1            # how many colours should be included, 5, 6
    COLOUR_COMBO = 2      # how many combo for one colour, 2, 3
    CONNECTED_ORB = 3     # how many orbs connected, 4, 5, 6
    ORB_REMAINING = 4     # how many orbs remaining, usually less than 5
    SHAPE_L = 5           # L shape
    SHAPE_PLUS = 6        # + plus shape, 十字
    SHAPE_SQUARE = 7      # square shape, 無効貫通
    SHAPE_ROW = 8         # row shape, one line
    SHAPE_COLUMN = 9      # column shape, 追撃


class Profile:
    def __init__(self, name: ProfileName, threshold: int = 20, target: int = -1, orbs: List[bool] = None):
        if orbs is None:
            # default to 5 colours + heal
            c_orb_list = orb_list(
                *convert([Orb.FIRE, Orb.WATER, Orb.WOOD, Orb.LIGHT, Orb.DARK, Orb.HEAL]))
        else:
            c_orb_list = orb_list(*orbs)

        self.c_profile = c_profile(
            int(name.value), threshold, target, c_orb_list)


libpazusoba = CDLL("libpazusoba.so", winmode=0)
# NOTE: the restype here must be correct to call it properly
libpazusoba.adventure.restype = c_state
libpazusoba.adventure.argtypes = (c_int, POINTER(c_char_p))

libpazusoba.adventureEx.restype = c_state
libpazusoba.adventureEx.argtypes = (POINTER(c_char), c_int, c_int, c_int)


def adventureEx(board: str, min_erase: int, search_depth: int, beam_size: int, profiles: List[Profile]) -> State:
    # additional step is required here because Mac is stricter than Windows
    c_board = c_char_p(board.encode("ascii"))
    # convert python Profile list to c_profile array
    profile_count = len(profiles)
    c_profile_list = (c_profile * profile_count)()
    for i in range(profile_count):
        c_profile_list[i] = profiles[i].c_profile

    state = libpazusoba.adventureEx(
        c_board, min_erase, search_depth, beam_size, c_profile_list, profile_count)
    return State(state)


def convert(orbs: List[Orb]) -> List[bool]:
    """Convert orb list to bool list"""
    orb_list = [False] * 11
    for o in orbs:
        orb_list[int(o.value)] = True
    return orb_list


def adventure(arguments: List[str]) -> State:
    # additional step is required here because Mac is stricter than Windows
    argv = []
    for s in arguments:
        argv.append(c_char_p(s.encode("ascii")))
    c_argc = len(arguments)
    c_argv = (c_char_p * c_argc)()
    c_argv[:] = argv

    state = libpazusoba.adventure(c_argc, c_argv)
    return State(state)


if __name__ == "__main__":
    # state = adventure(
    #     ["pazusoba", "RLRRDBHBLDBLDHRGLGBRGLBDBHDGRL", "3", "100", "10000"])
    state = adventureEx(
        "LRGDLDHRRRRDHDDGBDHLLGBDBBBGLR", 3, 150, 10000, [
            # Profile(ProfileName.SHAPE_PLUS, threshold=100),
            Profile(name=ProfileName.CONNECTED_ORB, target=4),
            Profile(name=ProfileName.SHAPE_PLUS),
        ])
    print(state)
