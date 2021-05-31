"""
A collection of pyautogui method
"""

from config import BOARD_LOCATION, SCREEN_SCALE, DEBUG_MODE
from random import randint
from screenshot import take_screenshot

class GUI:
    def __init__(self, board_row):
        self.board_row = board_row

    def perform(self, route: list, snapshot=True):
        """
        Perform the best route step by step
        """

        if DEBUG_MODE:
            print("=== PERFORMING ===")
        # setup everything
        left, top, end_left, end_top = BOARD_LOCATION
        orb_height = (end_top - top) / self.board_row
        x_start = left + orb_height / 2
        y_start = top + orb_height / 2

        # save current position
        (px, py) = gui.position()
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
        gui.mouseUp()
        print("=> Done")
        if DEBUG_MODE:
            print("=> It took %.3fs." % (time.time() - start))

        # move back to current position after everything
        gui.moveTo(px, py)
        gui.leftClick()

        if snapshot:
            # save final solution
            width = (end_left - left) * SCREEN_SCALE
            height = (end_top - top) * SCREEN_SCALE
            monitor = {"top": top * SCREEN_SCALE, "left": left * SCREEN_SCALE, "width": width, "height": height}
            take_screenshot(monitor, write2disk=True)

    def __holdLeftKey(x, y, repeat=True):
        """
        Hold the left mouse key down twice if requires to prevent `window focus` issues
        """
        gui.mouseDown(x, y, button='left')
        if repeat:
            gui.mouseDown(x, y, button='left')

    def __moveTo(self, x, y, ultra_fast=False, random=False):
        """
        Move to (x, y) using default settings (duration=0, _pause=True) or ultra fast with random delays
        """
        if ultra_fast:
            gui.moveTo(target_x, target_y, duration=0, _pause=False)
            # add an random offset if required
            offset = 0 if random else randint(0, 100)

            # NOTE: 50ms is about the minimum time for the game to recognise correctly, less than it will cause some issues
            time.sleep((50 + offset) / 1000)
        else:
            gui.moveTo(x, y)