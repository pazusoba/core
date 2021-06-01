"""
pazusoba automation
"""

import os
import time

import numpy as np

import opencv
import gui

from config import DEBUG_MODE, BOARD_LOCATION, GAME_LOCATION
from utils import waitForCycles, waitForNextCycle, getMonitorParamsFrom
from automation import tapInOrder, find, tap
from screenshot import take_screenshot

current_dir = os.path.abspath(os.getcwd())
if not current_dir.endswith("automation"):
    exit("Run main.py inside automation folder. Current path is {}".format(current_dir))

def game_loop():
    """
    The main loop of automation
    """

    battle_count = 0
    boss_battle = False
    in_dungeon = None

    MONITOR = getMonitorParamsFrom(GAME_LOCATION)

    while True:
        waitForNextCycle()
        # Get current game screen
        game_img = np.array(take_screenshot(MONITOR, write2disk=True, name="eye.png"))

        if in_dungeon == None:
            # setup in_dungeon
            in_dungeon = not find(u"game/home.png", game_img)[0]
            if DEBUG_MODE:
                print("=> In a dungeon - {}".format(in_dungeon))

        if in_dungeon:
            # Orbs are still falling, wait
            if find(u"game/battle/empty1.png", game_img)[0] or find(u"game/battle/empty2.png", game_img)[0]:
                if DEBUG_MODE:
                    print("=> Waiting for combo")
                waitForNextCycle()
                continue

            # Before boss batlle, track current battle
            if not boss_battle:
                if find(u"game/battle/battle_number.png", game_img)[0]:
                    battle_count += 1
                    print("=> Battle {}".format(battle_count))
                    # wait for one extra cycle to prevent counting twice here
                    waitForCycles(2)
                    continue

                if find(u"game/battle/boss_alert.png", game_img)[0]:
                    battle_count += 1
                    boss_battle = True
                    print("=> Boss ({} / {})".format(battle_count, battle_count))
                    continue

            # Clear or Game Over
            if tap(u"game/buttons/ok.png", game_img):
                print("=> Exiting the dungeon")
                in_dungeon = False
                waitForCycles(2)
                continue

            # Do puzzle here
            __doPuzzle()
        else:
            # either before entering a dungeon or getting rewards
            pass
            

def manual_loop():
    """
    Semi automation
    """

    while True:
        __doPuzzle()
                
        decision = input("=> press enter to continue, enter anything to quit: ")
        if not decision == "":
            break

def __doPuzzle():
    # Do puzzle here
    start = time.time()
    board = opencv.run()
    if DEBUG_MODE:
        print("=> Read the board in %.3fs\n" % (time.time() - start))
        print(board)

    # board = "RHHBDRDRGHDLLBGRRBRHBGGBHBDDHH"
    if "?" in board:
        print("=> Failed to read the board")
        # print it out for me to see and manually adjust it above
        # for i in range(board_row):
        #     start = i * board_column
        #     print(board[start:(start + board_column)])
    else:
        solution = opencv.getSolution(board)
        if DEBUG_MODE:
            print("=> Solved in %.3fs\n" % (time.time() - start))
        gui.perform(solution)

if __name__ == "__main__":
    game_loop()
    # manual_loop()