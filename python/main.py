import sys

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
    # check if it is a string
    board_len = len(args[1])
    if board_len == 20 or board_len == 32 or board_len == 42:
        board_str = args[1]
    else:
        print("board is not valid")
        exit(1)
if arg_len > 2:
    # so far, we have 3, 4 and 5 as min erase
    min_erase = min(args[2], 5)
if arg_len > 3:
    # max step is 100
    depth = min(args[3], 150)
if arg_len > 4:
    # for now, max width is 5000
    width = min(args[4], 5000)

# read the board
