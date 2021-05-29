# OpenCV
import cv2 as cv
import numpy as np

from screenshot import *
import os
from functools import cmp_to_key

import pyautogui as gui

game_loc = [454, 120, 1459, 2086]
screen_scale = 1
match_offset = 100
uniform_size = (1000, 1950)

def find(template, img):
    found = False
    if os.path.exists(template):
        # cv color
        template_img = cv.imread(template, 0)
        print("Read template from '{}'".format(template))
        # show(template_img, "template")

        # NOTE: resize is very important for the detection to be consistent
        img = cv.resize(img, uniform_size)
        # show(img, "Resize")

        gray = cv.cvtColor(img, cv.COLOR_BGR2GRAY)
        matches = cv.matchTemplate(gray, template_img, cv.TM_CCOEFF_NORMED)
        # show(matches, "Raw")

        # Check if anything matches with the template
        w, h = template_img.shape[::-1]
        curr_pt = None
        locations = list(sorted_matches(matches, 0.8))
        for pt in locations:
             # remove duplicates
            if curr_pt is None:
                curr_pt = pt
            elif abs(curr_pt[0] - pt[0]) < match_offset and abs(curr_pt[1] - pt[1]) < match_offset:
                continue
            else:
                curr_pt = pt
            # print(pt)

            # draw rectangles around all locations
            found = True
            cv.rectangle(img, pt, (pt[0] + w, pt[1] + h), (255, 0, 255), 3)

        if found:
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
        return found
    else:
        exit("Cannot find {}".format(template))
    return found

def sorted_matches(matches, threshold):
    filtered_matches = np.where(matches >= threshold)
    return sorted(zip(*filtered_matches[::-1]), key=cmp_to_key(sortLocation))

def sortLocation(a, b):
    # 140, 141, 142 are treated as one point here
    if abs(a[0] - b[0]) < match_offset:
        return a[1] - b[1]
    return a[0] - b[0]

def show(img, name="Image"):
    cv.imshow(name, img)
    cv.waitKey()
    cv.destroyWindow(name)

def testFind():
    left, top, end_left, end_top = game_loc
    width = (end_left - left) * screen_scale
    height = (end_top - top) * screen_scale
    print("Game Size is ({}, {})".format(width, height))

    monitor = {"top": top * screen_scale, "left": left * screen_scale, "width": width, "height": height}
    game_img = np.array(take_screenshot(monitor))
    # show(game_img, "game")

    print(find(u"game/dungeons/tamadora.png", game_img))
    # print(find(u"game/dungeons/mugen-kairou.png", game_img))
    # print(find(u"game/sell.png", game_img))

# NOTE: comment this line when using as a module
testFind()
