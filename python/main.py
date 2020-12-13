import sys
from constant import Orb, ORB_SIMULATION_NAMES
from board import Board

"""
python3 main.py GLHLGGLBDHDDDHGHHRDRLDDLGLDDRG 3 25 1000
- board
- min erase condition
- depth
- width
"""

arg_len = len(sys.argv)
args = sys.argv

board_str = "RHGHDRGLBLHGDBLLHBBBHRLHGHDGLB"
min_erase = 3
depth = 30
width = 1000

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

for column in range(board.column):
    for row in range(board.row):
        pos = board.duplicate()
        pos.loc = (column, row)
        pos.get_children()

board.info()
board.children[0].get_children()
board.children[0].info()
