"""
board.py
- data class for board to store the board
"""

from constant import ORB_SIMULATION_NAMES, Orb
from copy import copy, deepcopy
from random import randint


class Combo:
    loc = None
    orb = None

    def __init__(self, loc: tuple, orb: Orb):
        self.loc = loc
        self.orb = orb


class Board:
    row = 0
    column = 0
    board = []
    board_hash = ""

    min_erase = 3
    depth = 30
    width = 1000

    step = 0
    score = 0
    children = []
    loc = None

    parent = None
    prev_score = 0
    prev_loc = None

    def __init__(self, board_str, min_erase, depth, width):
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
        self.min_erase = min_erase
        self.depth = depth
        self.width = width

    def __lt__(self, other):
        if self.score == other.score:
            return self.step > other.step
        return self.score < other.score

    def swap(self, from_loc, to_loc):
        """
        copy current board and swap 2 orb locations
        """

        board_copy = self.duplicate()
        new_board = board_copy.board
        board_copy.children = []

        # swap two values, so simple in python
        x, y = from_loc
        nx, ny = to_loc
        new_board[x][y], new_board[nx][ny] = new_board[nx][ny], new_board[x][y]

        board_copy.step += 1
        board_copy.loc = to_loc

        board_copy.parent = self
        board_copy.prev_score = self.score
        board_copy.prev_loc = self.loc

        board_copy.calc_board_hash(force=True)
        board_copy.calc_score()

        return board_copy

    def get_children(self, diagonal=False):
        if self.step == self.depth:
            return

        for x in range(-1, 2):
            for y in range(-1, 2):
                if x == y == 0:
                    continue

                is_diagonal = abs(x) * abs(y) > 0
                # continue if it is diagonal but not in diagonal mode
                if not diagonal and is_diagonal:
                    continue

                nx, ny = self.loc
                nx += x
                ny += y
                if (nx, ny) == self.prev_loc:
                    continue
                if self.is_valid_loc((nx, ny)):
                    new_board = self.swap(self.loc, (nx, ny))
                    self.children.append(new_board)

    def is_valid_loc(self, loc):
        # check x and y are in bound
        x, y = loc
        return 0 <= x <= self.column - 1 and 0 <= y <= self.row - 1

    def calc_board_hash(self, force=False):
        # only calculate if current hash is empty
        if len(self.board_hash) == 0 or force:
            self.board_hash = ""
            for x in self.board:
                for y in x:
                    self.board_hash += ORB_SIMULATION_NAMES[y.value]

    def calc_score(self):
        # copy the board to erase combo
        temp = deepcopy(self.board)
        combo_list = []
        combo = []
        move_count = 0

        more_combo = True
        while more_combo:
            more_combo = False

            for x in range(self.column - 1, -1, -1):
                for y in range(self.row):
                    curr_orb = temp[x][y]
                    if curr_orb == Orb.EMPTY:
                        continue

                    self._flood_fill(temp, combo, x, y, curr_orb, -1)
                    if len(combo) >= self.min_erase:
                        more_combo = True
                        combo_list.append(combo)
                        combo = []

            if more_combo:
                move_count += 1
                more_combo = self._move_orbs_down(temp)
        self.score = len(combo_list) * 1000 + move_count * 50

    def _flood_fill(self, board, combo, x, y, orb, direction):
        if not self.is_valid_loc((x, y)):
            return

        curr_orb = board[x][y]
        if curr_orb != orb:
            return

        count = 0
        direction_list = [0, 0, 0, 0]

        for d in range(4):
            loop = self.row
            if d > 1:
                loop = self.column

            for i in range(loop):
                cx, cy = x, y
                if d == 0:
                    cy += i
                elif d == 1:
                    cy -= i
                elif d == 2:
                    cx += i
                elif d == 3:
                    cx -= i

                if not self.is_valid_loc((cx, cy)):
                    break
                if board[cx][cy] != orb:
                    break

                direction_list[d] += 1
                count += 1

        # added the same orb 4 times so need to -3
        count -= 3
        if count >= self.min_erase:
            min_connection = 3 if self.min_erase >= 3 else self.min_erase

            for d in range(4):
                curr_count = direction_list[d]
                if curr_count < min_connection:
                    continue

                for i in range(curr_count):
                    cx, cy = x, y
                    if d == 0:
                        cy += i
                    elif d == 1:
                        cy -= i
                    elif d == 2:
                        cx += i
                    elif d == 3:
                        cx -= i

                    has_orb_around = False
                    if d < 2:
                        has_orb_around = self.has_same_orb(
                            board, orb, cx + 1, cy) and self.has_same_orb(board, orb, cx - 1, cy)
                    else:
                        has_orb_around = self.has_same_orb(
                            board, orb, cx, cy + 1) and self.has_same_orb(board, orb, cx, cy - 1)

                    if not has_orb_around:
                        board[cx][cy] = Orb.EMPTY
                        combo.append(Combo((cx, cy), orb))

                for i in range(curr_count):
                    cx, cy = x, y
                    if d == 0:
                        cy += i
                    elif d == 1:
                        cy -= i
                    elif d == 2:
                        cx += i
                    elif d == 3:
                        cx -= i

                    self._flood_fill(board, combo, cx, cy + 1, orb, 0)
                    self._flood_fill(board, combo, cx, cy - 1, orb, 1)
                    self._flood_fill(board, combo, cx + 1, cy, orb, 2)
                    self._flood_fill(board, combo, cx - 1, cy, orb, 3)

    def has_same_orb(self, board, orb, x, y):
        if self.is_valid_loc((x, y)):
            return board[x][y] == orb
        return False

    def _move_orbs_down(self, board):
        changed = False
        for y in range(self.row):
            orbs = []
            empty = 0

            # from bottom up
            for x in range(self.column - 1, -1, -1):
                curr_orb = board[x][y]
                if curr_orb == Orb.EMPTY:
                    empty += 1
                else:
                    orbs.append(curr_orb)

            # at least one orb is removed but do nothing if all are removed
            if 0 < empty < self.column:
                changed = True
                s = len(orbs)
                k = 0
                for x in range(self.column - 1, -1, -1):
                    if k >= s:
                        board[x][y] = Orb.EMPTY
                    else:
                        board[x][y] = orbs[k]
                    k += 1
        return changed

    def info(self):
        self.print_board()
        print("{} x {} | Step - {} | {} -> {} | Score - {} ({}) | Children - {}"
              .format(self.row, self.column, self.step, self.prev_loc, self.loc, self.score, self.prev_score, len(self.children)))

    def duplicate(self):
        new_copy = copy(self)
        new_copy.board = deepcopy(self.board)
        return new_copy

    def get_target_score(self):
        # an estimation of the score we want to reach
        return 8000  # 8 combo

    def print_board(self, number=False, offset=1):
        if not number:
            print(self.board_hash)
        else:
            for x in self.board:
                for y in x:
                    print(y.value - offset, end="")
            print()
