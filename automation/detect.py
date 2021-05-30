# OpenCV
import cv2 as cv
import numpy as np

import os
import time

from screenshot import *
from functools import cmp_to_key
from typing import Tuple, List

import pyautogui as gui
import config

game_loc = [454, 120, 1459, 2086]
screen_scale = 1
INPUT_SIZE = (1000, 1950)
SORT_OFFSET = 100
TAP_DELAY = 500

def find(template: str, img) -> Tuple[bool, Tuple[int, int]]:
    """
    Find template in img and tap on it if needed.
    img doesn't need to be in gray scale because it will be converted in this function.
    """
    result = (False, ())
    if os.path.exists(template):
        template_img = cv.imread(template, 0)
        print("👀 Looing for '{}'".format(template))
        # __show(template_img, "template")

        # NOTE: resize is very important for the detection to be consistent
        img = cv.resize(img, INPUT_SIZE)
        # __show(img, "Resize")

        gray = cv.cvtColor(img, cv.COLOR_BGR2GRAY)
        matches = cv.matchTemplate(gray, template_img, cv.TM_CCOEFF_NORMED)
        # __show(matches, "Raw")

        # Check if anything matches with the template
        w, h = template_img.shape[::-1]
        locations = __sorted_matches(matches, 0.8, offset=SORT_OFFSET)
        for pt in locations:
            # draw rectangles around all locations
            result = (True, ())
            cv.rectangle(img, pt, (pt[0] + w, pt[1] + h), (255, 0, 255), 3)

        if result[0]:
            # only consider the first one
            x, y = locations[0]

            # TODO: consider the scale change when we resize the image

            # need to scale x, y to original and get center point
            x += game_loc[0] +  w / 2
            y += game_loc[1] + h / 2

            result = (True, (x, y))
            print("=> ({}, {})".format(x, y))
            # __show(img, "Matches")
    else:
        exit("Cannot find template at {}".format(template))
    return result

def tap(template: str, img, delay=500) -> bool:
    """
    Find template from img and tap on it
    """

    # location can be () here so only get x, y if found
    found, location = find(template, img)
    if found:
        x, y = location
        gui.moveTo(x, y)
        gui.mouseDown()
        gui.mouseUp()

        time.sleep(delay / 500)
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
    print("=> Game Size is ({}, {})".format(width, height))

    monitor = {"top": top * screen_scale, "left": left * screen_scale, "width": width, "height": height}
    game_img = np.array(take_screenshot(monitor))

    # print(find(u"game/dungeons/tamadora.png", game_img))
    # print(tap(u"game/dungeons/mugen-kairou.png", game_img))
    # print(find(u"game/sell.png", game_img))
    # print(tap(u"game/buttons/special.png", game_img))
    # print(tap(u"game/home.png", game_img))

    # go to the required dungeon
    tap(u"game/dungeons/kairou/main.png", game_img)

def __testInstructions(instructions: List[str]) -> bool:
    left, top, end_left, end_top = game_loc
    width = (end_left - left) * screen_scale
    height = (end_top - top) * screen_scale
    print("=> Game Size is ({}, {})".format(width, height))

    monitor = {"top": top * screen_scale, "left": left * screen_scale, "width": width, "height": height}
    for template in instructions:
        game_img = np.array(take_screenshot(monitor))
        success = tap(template, game_img)
        if not success:
            print("❌ Couldn't find '{}'".format(template))
            return False

    print("✔ All instructions have been performed\n")
    return True

# NOTE: comment this line when using as a module
# __testFind()

# Go to mugen kairou
__testInstructions([
    "game/dungeons/kairou/main.png",
    "game/dungeons/kairou/sub1.png",
    "game/buttons/you.png",
    "game/buttons/challenge.png",
])

time.sleep(8)

# Quit any battles
__testInstructions([
    "game/buttons/menu.png",
    "game/buttons/quit_battle.png",
    "game/buttons/yes.png",
])
