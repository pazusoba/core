"""
pazusoba automation
"""

import time
import opencv
import os

current_dir = os.path.abspath(os.getcwd())
if not current_dir.endswith("automation"):
    exit("Run main.py inside automation folder. Current path is {}".format(current_dir))

# 20, 30 or 42
full_auto = False
battle_count = 0

while True:
    battle_count += 1
    print("\n=== BATTLE {} ===".format((battle_count)))

    start = time.time()
    board = opencv.run()
    print(board)
    print("Read the board in %.3fs.\n" % (time.time() - start))
    # board = "RHHBDRDRGHDLLBGRRBRHBGGBHBDDHH"
    if "?" in board:
        battle_count -= 1
        print("Waiting...")
        # wait for a second
        time.sleep(2)
        # print it out for me to see and manually adjust it above
        # for i in range(board_row):
        #     start = i * board_column
        #     print(board[start:(start + board_column)])
    else:
        solution = opencv.getSolution(board)
        print("Solved in %.3fs.\n" % (time.time() - start))
        opencv.perform(solution)

        print('Cooling down...')
        time.sleep(4)

    if full_auto:
        continue
    else:
        # Ask for user input
        decision = input("Please enter to continue, q to quit: ")
        if decision.startswith("q"):
            break
