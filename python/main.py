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

to_visit = PriorityQueue()
best_score = {}

start_time = time()
for column in range(board.column):
    for row in range(board.row):
        pos = board.duplicate()
        pos.loc = (column, row)
        pos.get_children()

# step 1
for c in board.children:
    to_visit.put(c)

# from step 2
for step in range(0, depth):
    step_time = time()
    print("Step {} - ".format(step + 1), end="")
    for i in range(width):
        if to_visit.empty():
            break

        curr_board = to_visit.get()
        curr_board.get_children()
        for c in curr_board.children:
            best_score[c.score] = c.board_hash
            to_visit.put(c)
            # c.info()

    print("{}s".format(time() - step_time))

print("Total time - {}s".format(time() - start_time))

for key in best_score:
    print("{} - {}".format(key, best_score[key]))
