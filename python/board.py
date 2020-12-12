"""
board.py
- data class for board to store the board
"""

from constant import ORB_SIMULATION_NAMES, Orb
from copy import copy


class Board:
    row = 0
    column = 0
    board = []
    board_hash = ""

    step = 0
    score = 0

    def __init__(self, board_str):
        length = len(board_str)
        if length == 20:
            self.column, self.row = 4, 5
        elif length == 30:
            self.column, self.row = 5, 6
        elif length == 42:
            self.column, self.row = 6, 7
        else:
            raise AssertionError("Board length must be 20, 30 or 42")

        for x in range(self.column):
            new_row = []
            for y in range(self.row):
                orb_char = board_str[x * self.row + y]
                new_orb = Orb(ORB_SIMULATION_NAMES.index(orb_char))
                new_row.append(new_orb)
            self.board.append(new_row)

        self.board_hash = board_str

    def swap(self, from_loc, to_loc):
        """
        copy current board and swap 2 orb locations
        """

        board_copy = copy(self)
        new_board = board_copy.board

        x, y = from_loc
        nx, ny = to_loc
        new_board[x][y], new_board[nx][ny] = new_board[nx][ny], new_board[x][y]

        board_copy.step += 1
        board_copy.calc_board_hash(force=True)
        return board_copy

    def calc_board_hash(self, force=False):
        # only calculate if current hash is empty
        if len(self.board_hash) == 0 or force:
            self.board_hash = ""
            for x in self.board:
                for y in x:
                    self.board_hash += ORB_SIMULATION_NAMES[y.value]

    def info(self):
        self.print_board()
        print("{} x {} | Step - {} | Score - {}".format(self.row,
                                                        self.column, self.step, self.score))

    def print_board(self, number=False, offset=1):
        if not number:
            print(self.board_hash)
        else:
            for x in self.board:
                for y in x:
                    print(y.value - offset, end="")
            print()
