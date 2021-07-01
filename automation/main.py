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
from automation import tapInOrder, find, tap, touch
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
    # prevent excessive computation
    puzzle_after_cycles = 0
    getting_rewards = False

    MONITOR = getMonitorParamsFrom(GAME_LOCATION)

    while True:
        # Get current game screen
        game_img = np.array(take_screenshot(MONITOR, write2disk=True, name="eye.png"))

        if in_dungeon == None:
            # setup in_dungeon
            in_dungeon = not find(u"game/home.png", game_img)[0]
            if DEBUG_MODE:
                print("=> In a dungeon - {}".format(in_dungeon))

        if in_dungeon:
            # never continue
            if tap(u"game/battle/continue.png", game_img):
                tapInOrder([
                    "game/buttons/no.png",
                    "game/buttons/yes.png",
                ])
                print("=> Lose. Give Up")
            
            # Rank Fever mode always show something in the begining
            feverMode = find(u"game/rank/fever.png", game_img)[0]
            # Clear or Game Over
            if tap(u"game/buttons/ok.png", game_img):
                if not feverMode:
                    print("=> Exiting the dungeon")
                    in_dungeon = False
                    waitForNextCycle()
                continue
            
            # Orbs are still falling, wait
            if find(u"game/battle/empty1.png", game_img)[0] or find(u"game/battle/empty2.png", game_img)[0]:
                if DEBUG_MODE:
                    print("=> Waiting for combo")
                puzzle_after_cycles += 0.25
                continue

            # Before boss batlle, track current battle
            if not boss_battle:
                if find(u"game/battle/battle_number.png", game_img)[0]:
                    battle_count += 1
                    puzzle_after_cycles += 1
                    print("=> Battle {}".format(battle_count))
                    # wait for one extra cycle to prevent counting twice here
                    waitForCycles(2)
                    continue
                elif find(u"game/battle/boss_alert.png", game_img)[0]:
                    battle_count += 1
                    puzzle_after_cycles += 1
                    boss_battle = True
                    print("=> Boss ({} / {})".format(battle_count, battle_count))
                    continue

            # Do puzzle if allowed
            if puzzle_after_cycles <= 0:
                if __doPuzzle():
                    # reset the counter
                    puzzle_after_cycles = 0
                puzzle_after_cycles = 2
            else:
                puzzle_after_cycles -= 1
        else:
            # check if getting rewards
            if getting_rewards or find(u"game/rewards.png", game_img)[0]:
                # Already back to home
                if find(u"game/dungeon.png", game_img)[0]:
                    getting_rewards = False
                    continue

                # update the flag because it may go to another screen
                if not getting_rewards:
                    getting_rewards = True
                    print("=> Getting rewards")

                # keep tapping the screen until sell button is visible 
                if not find(u"game/buttons/sell.png", game_img)[0]:
                    print("=> Received all rewards")
                    touch()
                else:
                    # clear the flag and keep tapping
                    if getting_rewards:
                        getting_rewards = False
                        print("=> Collected all rewards")
                    touch()
            # always click ok button to dismiss alerts
            elif tap(u"game/buttons/ok.png", game_img):
                print("=> OK")
                waitForNextCycle()
                continue
            # tap on new if found any
            # elif tap(u"game/dungeons/new.png", game_img):
            #     print("=> New Dungeon")
            #     continue
            elif tap(u"game/rank/gacha.png", game_img):
                print("=> Rank Fever Mode")
                continue
            elif tap(u"game/buttons/challenge.png", game_img):
                print("=> Challenge")
                in_dungeon = True
                puzzle_after_cycles = 6
                continue
            # elif tap(u"game/dungeons/new_loss.png", game_img):
            #     print("=> Challenge again")
            #     continue
            # choose a helper and enter the dungeon
            elif find(u"game/friends/helper.png", game_img)[0]:
                # NOTE: consider the case when it is out of stamina
                success = tapInOrder([
                    u"game/friends/you.png",
                    u"game/friends/select.png",
                    u"game/buttons/challenge.png",
                ])

                if success:
                    in_dungeon = True
                    puzzle_after_cycles = 6  
            elif find(u"game/rank/title.png", game_img)[0]:
                print("=> Checking current rank")
                touch()      
            # try going up one level to normal
            elif not find(u"game/dungeons/normal.png", game_img)[0]:
                print("=> One level back")
                tap(u"game/buttons/up.png", game_img)

        # NOTE: Never block the loop because it very important to see what's happen
        waitForNextCycle()

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
        return False
    else:
        solution = opencv.getSolution(board)
        if DEBUG_MODE:
            print("=> Solved in %.3fs\n" % (time.time() - start))
        gui.perform(solution)
        return True

if __name__ == "__main__":
    game_loop()
    # manual_loop()