# OpenCV
import cv2 as cv
import numpy as np

from screenshot import *
import os
from functools import cmp_to_key

import pyautogui as gui
import config

game_loc = [454, 120, 1459, 2086]
screen_scale = 1
INPUT_SIZE = (1000, 1950)
SORT_OFFSET = 100

def find(template: str, img, tap=False) -> bool:
    """
    Find template in img and tap on it if needed.
    img doesn't need to be in gray scale because it will be converted in this function.
    """
    found = False
    if os.path.exists(template):
        template_img = cv.imread(template, 0)
        print("Read template from '{}'".format(template))
        # show(template_img, "template")

        # NOTE: resize is very important for the detection to be consistent
        img = cv.resize(img, INPUT_SIZE)
        # show(img, "Resize")

        gray = cv.cvtColor(img, cv.COLOR_BGR2GRAY)
        matches = cv.matchTemplate(gray, template_img, cv.TM_CCOEFF_NORMED)
        # show(matches, "Raw")

        # Check if anything matches with the template
        w, h = template_img.shape[::-1]
        locations = __sorted_matches(matches, 0.8, offset=SORT_OFFSET)
        for pt in locations:
            # draw rectangles around all locations
            found = True
            cv.rectangle(img, pt, (pt[0] + w, pt[1] + h), (255, 0, 255), 3)

        if found:
            # only consider the first one
            x, y = locations[0]
            # need to scale x, y to original
            x += game_loc[0]
            y += game_loc[1]

            end_x, end_y = x + w, x + h
            # calculate the center point
            x += (end_x - x) / 2
            y += (end_y - y) / 2

            gui.leftClick(x, y)
            print("Found at ({}, {})".format(x, y))
            show(img, "Matches")
    else:
        exit("Cannot find file at {}".format(template))
    return found

def __sorted_matches(matches, threshold: int, offset=25, allow_duplicates=False) -> list:
    """
    Filter out matches base on the threshold.
    Return a list of sorted and filtered matches
    """
    output = []
    curr_pt = None

    filtered_matches = np.where(matches >= threshold)
    for pt in sorted(zip(*filtered_matches[::-1]), key=cmp_to_key(lambda a, b : __sort_element(a, b, offset))):
        if not allow_duplicates:
            # ignore points which are close to each other
            if curr_pt is None:
                curr_pt = pt
            elif abs(curr_pt[0] - pt[0]) < offset and abs(curr_pt[1] - pt[1]) < offset:
                continue
            else:
                curr_pt = pt

        output.append(pt)
    return output

def __sort_element(a, b, offset: int) -> bool:
    """
    Sort elements based on the offset
    """
    # 140, 141, 142 are treated as one point here
    if abs(a[0] - b[0]) < offset:
        return a[1] - b[1]
    return a[0] - b[0]

def __show(img, name="image"):
    """
    Show an image using opencv with title
    """
    cv.imshow(name, img)
    cv.waitKey()
    cv.destroyWindow(name)

def __testFind():
    """
    Debug only
    """
    left, top, end_left, end_top = game_loc
    width = (end_left - left) * screen_scale
    height = (end_top - top) * screen_scale
    print("Game Size is ({}, {})".format(width, height))

    monitor = {"top": top * screen_scale, "left": left * screen_scale, "width": width, "height": height}
    game_img = np.array(take_screenshot(monitor))
    # show(game_img, "game")

    # print(find(u"game/dungeons/tamadora.png", game_img))
    # print(find(u"game/dungeons/mugen-kairou.png", game_img))
    # print(find(u"game/sell.png", game_img))
    print(find(u"game/home.png", game_img))

# NOTE: comment this line when using as a module
__testFind()
