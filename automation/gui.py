"""
A collection of pyautogui method
"""

import time
import pyautogui

from config import BOARD_LOCATION, DEBUG_MODE
from random import randint
from screenshot import take_screenshot
from utils import getColumnRow, getMonitorParamsFrom

def perform(route: list, snapshot=True):
    """
    Perform the best route step by step
    """

    if DEBUG_MODE:
        print("- PERFORMING -")
    # setup everything
    left, top, end_left, end_top = BOARD_LOCATION
    _, row = getColumnRow()
    orb_height = (end_top - top) / row
    x_start = left + orb_height / 2
    y_start = top + orb_height / 2

    # save current position
    (px, py) = pyautogui.position()
    if DEBUG_MODE:
        step = len(route)
        print("=> {} steps".format(step))
        start = time.time()
    
    for i in range(step):
        curr = route[i]
        x, y = curr
        target_x = x_start + y * orb_height
        target_y = y_start + x * orb_height
        if i == 0:
            __holdLeftKey(target_x, target_y)  
        else:
            __moveTo(target_x, target_y, ultra_fast=True, random=True)

    # only release it when everything are all done
    pyautogui.mouseUp()
    print("=> Done")
    if DEBUG_MODE:
        print("=> It took %.3fs." % (time.time() - start))

    # move back to current position after everything
    pyautogui.moveTo(px, py)
    pyautogui.leftClick()

    if snapshot:
        # save final solution
        take_screenshot(getMonitorParamsFrom(BOARD_LOCATION), write2disk=True, name="route.png")

def __holdLeftKey(x, y, repeat=True):
    """
    Hold the left mouse key down twice if requires to prevent `window focus` issues
    """
    pyautogui.mouseDown(x, y, button='left')
    if repeat:
        pyautogui.mouseDown(x, y, button='left')

def __moveTo(x, y, ultra_fast=False, random=False):
    """
    Move to (x, y) using default settings (duration=0, _pause=True) or ultra fast with random delays
    """
    if ultra_fast:
        pyautogui.moveTo(x, y, duration=0, _pause=False)
        # add an random offset if required
        offset = 0 if not random else randint(0, 100)

        # NOTE: 50ms is about the minimum time for the game to recognise correctly, less than it will cause some issues
        time.sleep((50 + offset) / 1000)
    else:
        pyautogui.moveTo(x, y)