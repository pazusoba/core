# OpenCV
import cv2 as cv
import numpy as np

import os

from screenshot import *
from functools import cmp_to_key
from typing import Tuple, List

from utils import waitForCycles, waitForNextCycle
import pyautogui as gui
from config import GAME_LOCATION, GAME_SCREEN_SIZE_16_9, GAME_SCREEN_SIZE_2_1, GAME_SCREEN_SIZE_3_2, \
    SCREEN_SCALE, SORT_OFFSET, ONE_CYCLE

left, top, end_left, end_top = GAME_LOCATION
width = (end_left - left) * SCREEN_SCALE
height = (end_top - top) * SCREEN_SCALE
print("=> Game Size is ({}, {})".format(width, height))
monitor = {"top": top * SCREEN_SCALE, "left": left * SCREEN_SCALE, "width": width, "height": height}

# determine best input size
ratio = height / width
INPUT_SIZE = 0
ACCURACY = 0.8
if ratio < 1.6:
    INPUT_SIZE = GAME_SCREEN_SIZE_3_2
elif ratio < 1.85:
    INPUT_SIZE = GAME_SCREEN_SIZE_16_9
else:
    # NOTE: all templates are based on 2 : 1 game ratio
    # ACCURACY = 0.9
    INPUT_SIZE = GAME_SCREEN_SIZE_2_1
print("=> Ratio is {:.3f}. Resize to {}. Accuracy is set to {}".format(ratio, INPUT_SIZE, ACCURACY))

# track original size to go back to extract point
width_scale = width / INPUT_SIZE[0]
height_scale = height / INPUT_SIZE[1]
print("=> Scales: Width - {:.3f}, Height - {:.3f}".format(width_scale, height_scale))

def find(template: str, img) -> Tuple[bool, Tuple[int, int]]:
    """
    Find template in img and tap on it if needed.
    img doesn't need to be in gray scale because it will be converted in this function.
    """
    result = (False, ())
    if os.path.exists(template):
        template_img = cv.imread(template, 0)
        # print("👀 Looing for '{}'".format(template))
        # showImage(template_img, "template")

        # NOTE: resize is very important for the detection to be consistent
        img = cv.resize(img, INPUT_SIZE)
        # showImage(img, "Resize")

        gray = cv.cvtColor(img, cv.COLOR_BGR2GRAY)
        matches = cv.matchTemplate(gray, template_img, cv.TM_CCOEFF_NORMED)
        # showImage(matches, "Raw")

        # Check if anything matches with the template
        w, h = template_img.shape[::-1]
        locations = __sorted_matches(matches, ACCURACY, offset=SORT_OFFSET)
        for pt in locations:
            # draw rectangles around all locations
            result = (True, ())
            cv.rectangle(img, pt, (pt[0] + w, pt[1] + h), (255, 0, 255), 3)

        if result[0]:
            # only consider the first one
            x, y = locations[0]
            # print("=> Original ({}, {})".format(x, y))
            # NOTE: consider the scale change when we resize the image
            x *= width_scale
            y *= height_scale
            # print("=> Scaled ({}, {})".format(x, y))
            # print("=> w {}, h {}".format(w, h))

            # need to scale x, y to original and get center point
            # NOTE: consider template scale as well
            # TODO: make sure this still works on other machines
            x += GAME_LOCATION[0] +  w * width_scale / 2
            y += GAME_LOCATION[1] + h * height_scale / 2

            result = (True, (x, y))
            print("=> ({}, {})".format(x, y))
            # resized_img = cv.resize(img, (300, 570))
            # showImage(resized_img, "Matches")
    else:
        exit("Cannot find template at {}".format(template))
    return result

def tap(template: str, img, delay=1) -> bool:
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

        waitForCycles(delay)
    return found

def swipe_up(up=True):
    # find center point
    if up:
        offset = height / 3.3
    else:
        offset = - height / 3.3

    x, y = width / 2 + left, height / 2 + top
    gui.moveTo(x, y - offset)        
    gui.mouseDown()
    gui.moveTo(x, y + offset, duration=0.5)
    gui.mouseUp()

def swipe_down():
    swipe_up(False)

def touch():
    """
    Tap the center point of Game Location
    """
    x = width * 0.5 + left
    y = height * 0.75 + top
    gui.moveTo(x, y)
    gui.mouseDown()
    gui.mouseUp()

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

def showImage(img, name="image"):
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
    left, top, end_left, end_top = GAME_LOCATION
    width = (end_left - left) * SCREEN_SCALE
    height = (end_top - top) * SCREEN_SCALE
    print("=> Game Size is ({}, {})".format(width, height))

    monitor = {"top": top * SCREEN_SCALE, "left": left * SCREEN_SCALE, "width": width, "height": height}
    game_img = np.array(take_screenshot(monitor))

    # print(find(u"game/dungeons/tamadora.png", game_img))
    # print(tap(u"game/dungeons/mugen-kairou.png", game_img))
    # print(find(u"game/sell.png", game_img))
    # print(tap(u"game/buttons/special.png", game_img))
    # print(tap(u"game/home.png", game_img))

    # go to the required dungeon
    tap(u"game/dungeons/kairou/sub1.png", game_img)

def tapInOrder(instructions: List[str]) -> bool:
    """
    Find templates in order with delay
    """

    result = False

    # at least, try all of them and see if one if successful
    for template in instructions:
        # NOTE: move the cursor outside the game screen so that it doesn't cover up the screen
        gui.moveTo(end_left + 10, height / 2)

        game_img = np.array(take_screenshot(monitor))
        success = tap(template, game_img)
        if success:
            result = True

    # for template in instructions:
    #     while not success:
            
        # if not success:
        #     print("❌ Couldn't find '{}'".format(template))
        #     return False
        
    return True

def __mugen_loop():
    """
    Go to mugen kairou, quit and repeat
    """
    while True:
        # test join and quit
        tapInOrder([
            # "game/dungeons/kairou/main.png",
            "game/dungeons/kairou/sub1.png",
            "game/friends/you.png",
            "game/buttons/challenge.png",
        ])

        waitForCycles(16)

        # Quit any battles
        tapInOrder([
            "game/battle/menu.png",
            "game/buttons/quit_battle.png",
            "game/buttons/yes.png",
        ])

        waitForCycles(6)

def __battle_counter():
    """
    Count battles, boss battle and also when exiting the dungeon
    """
    battle = 0
    boss = False
    while True:
        game_img = np.array(take_screenshot(monitor))
        # if find(u"game/battle/empty1.png", game_img)[0] or find(u"game/battle/empty2.png", game_img)[0]:
        #     print("=> Falling")
        if not boss:
            if find(u"game/battle/battle_number.png", game_img)[0]:
                battle += 1
                print("=> Battle {}".format(battle))
                # wait for one extra cycle to prevent counting twice
                waitForCycles(2)
            elif find(u"game/battle/boss_alert.png", game_img)[0]:
                battle += 1
                boss = True
                print("=> Boss ({} / {})".format(battle, battle))

        if tap(u"game/buttons/ok.png", game_img):
            print("=> End")
        waitForNextCycle()

def __get_resized_screenshot():
    screenshot_img = np.array(take_screenshot(monitor))
    showImage(cv.resize(screenshot_img, INPUT_SIZE), "Screenshot")

if __name__ == "__main__":
    # __get_resized_screenshot()
    # swipe_up()
    # swipe_down()
    # __testFind()

    # __mugen_loop()
    __get_resized_screenshot()
    # __battle_counter()
