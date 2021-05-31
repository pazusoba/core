"""
Choose board or game location
"""

import pyautogui as gui

import cv2 as cv
import numpy as np

import mss
import mss.tools

from typing import List

def get_location_manually() -> List[int]:
    """
    Get the board location on the screen
    """
    input('Move to top left of the board and press enter')
    one = gui.position()
    input('Move to bottom right of the board and press enter')
    two = gui.position()

    print("Board Location - [{}, {}, {}, {}]".format(one.x, one.y, two.x, two.y))
    return [one.x, one.y, two.x, two.y]

def get_location_automatically():
    """
    Get the board location automatically on the screen
    """
    border = 16
    with mss.mss() as screen:
        raw = screen.grab(screen.monitors[0])
        raw_cv = np.array(raw)
        # add some borders around
        raw_cv = cv.copyMakeBorder(raw_cv, border, border, border, border, cv.BORDER_CONSTANT)
        gray = cv.cvtColor(raw_cv, cv.COLOR_BGR2GRAY)
        _, binary = cv.threshold(gray, 40, 255, cv.THRESH_BINARY)

        contours, _ = cv.findContours(binary, cv.RETR_CCOMP, cv.CHAIN_APPROX_SIMPLE)
        rectContours = [c for c in contours if  cv.contourArea(c) > 3000000]
        result = cv.drawContours(raw_cv, rectContours, -1, (255, 0, 255), 3)
        for contour in rectContours:
            print(contour)

        cv.imshow("fullscreen", result)
        clearAllWindows()

def clearAllWindows():
    cv.waitKey()
    cv.destroyAllWindows()

# this check if this script is being executed instead of using as a module
if __name__ == "__main__":
    get_location_manually()
    # get_location_automatically()
