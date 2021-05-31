"""
pazusoba automation
"""

import os
import time

import opencv
import gui
from config import DEBUG_MODE
from utils import waitForCycles

current_dir = os.path.abspath(os.getcwd())
if not current_dir.endswith("automation"):
    exit("Run main.py inside automation folder. Current path is {}".format(current_dir))

# 20, 30 or 42
FULL_AUTO_MODE = False
battle_count = 0

while True:
    battle_count += 1
    if DEBUG_MODE:
        print("\n=== BATTLE {} ===".format((battle_count)))

    start = time.time()
    board = opencv.run()
    if DEBUG_MODE:
        print("=> Read the board in %.3fs\n" % (time.time() - start))
        print(board)

    # board = "RHHBDRDRGHDLLBGRRBRHBGGBHBDDHH"
    if "?" in board:
        battle_count -= 1
        print("Waiting...")
        waitForCycles(4)
        # print it out for me to see and manually adjust it above
        # for i in range(board_row):
        #     start = i * board_column
        #     print(board[start:(start + board_column)])
    else:
        solution = opencv.getSolution(board)
        print("=> Solved in %.3fs\n" % (time.time() - start))
        gui.perform(solution)

        print('=> Cooling down...')
        waitForCycles(8)

    if FULL_AUTO_MODE:
        continue
    else:
        # Ask for user input
        decision = input("=> press enter to continue, enter anything to quit: ")
        if not decision == "":
            break
