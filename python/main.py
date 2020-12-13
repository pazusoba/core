from sys import argv
from time import time
from multiprocessing import cpu_count
from queue import PriorityQueue

from constant import Orb, ORB_SIMULATION_NAMES
from board import Board

"""
python3 main.py GLHLGGLBDHDDDHGHHRDRLDDLGLDDRG 3 25 1000
- board
- min erase condition
- depth
- width
"""

arg_len = len(argv)
args = argv

board_str = "RHGHDRGLBLHGDBLLHBBBHRLHGHDGLB"
min_erase = 3
depth = 30
width = 5000

if arg_len > 1:
    board_str = args[1]
if arg_len > 2:
    # so far, we have 3, 4 and 5 as min erase
    min_erase = min(args[2], 5)
if arg_len > 3:
    # max step is 100
    depth = min(args[3], 150)
if arg_len > 4:
    # for now, max width is 5000
    width = min(args[4], 5000)


board = Board(board_str, min_erase, depth, width)
print("{} cores".format(cpu_count()))
print(board_str)

q = PriorityQueue()
children_boards = []
best_board = None
best_score = 0

start_time = time()
for column in range(board.column):
    for row in range(board.row):
        pos = board.duplicate()
        pos.loc = (column, row)
        pos.get_children()

# step 1
for c in board.children:
    q.put(c)

# from step 2
for step in range(0, depth):
    step_time = time()
    print("Step {} - ".format(step + 1), end="")
    for i in range(width):
        if q.empty():
            break

        curr_board = q.get()
        if curr_board.score > best_score:
            best_score = curr_board.score
            best_board = curr_board
        curr_board.get_children()
        for c in curr_board.children:
            children_boards.append(c)

    for b in children_boards:
        q.put(b)
    children_boards = []
    print("{}s".format(time() - step_time))

print("Total time - {}s".format(time() - start_time))
best_board.info()
