'''
Generate puzzle & dragons boards
Only support normal board (1 - 6, fire to heal)
by Yiheng Quan

Usage: ./board_pen.py [size] [repeat]
size must be in the format of 6x5, 7x6, column x row
'''

import sys
import uuid
import random

# 1 to 6, orbs that are supported
ORBS = list(range(1, 7))

# board size (6x5, 7x6, 5x4)
board = None
# how many board should be created
repeat = 1

arg_length = len(sys.argv)
if arg_length > 1:
    board = sys.argv[1]
if arg_length > 2:
    repeat = int(sys.argv[2])

size = board.split('x')
# column first and then row
column = int(size[0])
row = int(size[1])

# start generating files
for i in range(0, repeat):
    # just make it short, the original is too long
    filename = 'board_{}{}_{}.txt'.format(column, row, uuid.uuid4().hex[:6])
    with open(filename, 'w') as f:
        f.write('// {} x {}\n'.format(column, row))
        for j in range(0, row):
            for k in range(0, column):
                # pick a random value from ORBS
                f.write(str(random.choice(ORBS)) + ' ')
            f.write('\n')
        f.close()
