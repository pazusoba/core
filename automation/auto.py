# To add a new cell, type '# %%'
# To add a new markdown cell, type '# %% [markdown]'
# %%
import subprocess, os
import time

# OpenCV
import cv2 as cv
import numpy as np

# Automation
import pyautogui as gui

from utils import *
from location import *
from screenshot import *
import os

current_dir = os.path.abspath(os.getcwd())
if not current_dir.endswith("automation"):
    exit("Run auto.py inside automation folder. Current path is {}".format(current_dir))

# %%
# Print everything in debug mode
debug_mode = True

# start and end loc
# board_loc = get_location_manually()
board_loc = [461, 1264, 1449, 2087]

# what's the size of the board
# TODO: detect this automatically
board_config = [6, 5]
board_size = (830, 690)
orb_size = (140, 140)
border_len = 1
board_column = board_row = 0
screen_scale = 1

# This has Red, Blue, Green, Light, Dark and Heal
colour_range = {
    "R": (np.array([0, 100, 100]), np.array([10, 255, 255])),
    "B": (np.array([100, 100, 100]), np.array([125, 255, 255])),
    "G": (np.array([40, 100, 100]), np.array([70, 255, 255])),
    "L": (np.array([20, 100, 100]), np.array([35, 255, 255])),
    "D": (np.array([125, 100, 100]), np.array([150, 255, 255])),
    "H": (np.array([150, 100, 100]), np.array([165, 255, 255])),
}

# read in some template orbs
jammer_template = cv.imread("template/jammer.png", 0)
poison_template = cv.imread("template/poison.png", 0)
bomb_template = cv.imread("template/bomb.png", 0)
heal_template = cv.imread("template/heal.png", 0)

orb_templates = {
    "J": (jammer_template, 0.6),
    "P": (poison_template, 0.6),
    "H": (heal_template, 0.6),
    "E": (bomb_template, 0.7) # E for explosive
}

match_offset = 25

# %%
# detect the colour of all orbs and return a list
def detectColour(orbs: list) -> str:
    output = ""

    # first do colour detection
    for i in range(len(orbs)):
        curr = "?"
        src = orbs[i]
        # resize to be larger than the template
        src = cv.resize(src, orb_size)

        # cv.imshow("orb", src)
        # cv.waitKey()

        # match template first because jammer can be recognised as water
        gray = cv.cvtColor(src, cv.COLOR_BGR2GRAY)
        for c in orb_templates:
            curr_template = orb_templates[c][0]
            m = cv.matchTemplate(gray, curr_template, cv.TM_CCOEFF_NORMED)
            threshold = orb_templates[c][1]
            match = False
            for _ in zip(*np.where(m >= threshold)[::-1]):
                match = True
                break
            if match:
                curr = c
                break
        
        scale = 0.6
        x, _ = orb_size
        # Calculate new size and offset for both x and y
        size = int(x * scale)
        start = int((x - size) / 2)
        end = size + start
        # get the zoomed orb
        zoomed = src[start:end, start:end]
        # cv.imshow("zoomed", zoomed)
        # cv.waitKey()

        # match colour if no template matches
        if curr == "?":
            # convert to hsv
            hsv = cv.cvtColor(zoomed, cv.COLOR_BGR2HSV)
            # check which colour matches
            for c in colour_range:
                curr_range = colour_range[c]
                mask = cv.inRange(hsv, curr_range[0], curr_range[1])
                contours, _ = cv.findContours(mask, cv.RETR_CCOMP, cv.CHAIN_APPROX_SIMPLE)
                # get the sum of all mask areas
                area = sum(map(lambda c: cv.contourArea(c), contours))
                min_area = 800
                if c == "H":
                    min_area = 300
                # Usually it needs to be at least about 1000 because of the plus (for recovery about 400), usually it is around 9000
                if area > min_area:
                    curr = c
                    break
        output += curr
    return output

# %%
def run():
    # take a screenshot at board_loc, need to subtract because it is width and height
    left, top, end_left, end_top = board_loc
    width = (end_left - left) * screen_scale
    height = (end_top - top) * screen_scale

    monitor = {"top": top * screen_scale, "left": left * screen_scale, "width": width, "height": height}
    screen_img = np.array(take_screenshot(monitor))
   
    # NOTE: Testing only
    # screen_img = cv.imread("sample/dark.PNG")
    # resize it to about 830, 690 which is the size I use
    src = cv.resize(screen_img, board_size)

    # # get every single orb here
    # orb_count = len(orbContours) + jammer_len + bomb_len
    # print("There are {} orbs in total".format(orb_count))
    orbs, info = getEachOrb(src, board_size, orb_count, border_len)
    global board_column, board_row
    board_column, board_row = info

    # detect the colour of every orb and output a string, the list is in order so simple join the list will do
    return detectColour(orbs)

    # board_img = gui.screenshot(region=(left * screen_scale, top * screen_scale, width, height))
    # save the img for open cv
    # board_img.save("./board.png")

    # scan through the entire image
    # src = cv.imread("./board.png")
    # src = cv.imread("sample/battle.png")

    # # add some padding in case orbs are too close to the border
    # src = cv.copyMakeBorder(src, border_len, border_len, border_len, border_len, cv.BORDER_CONSTANT)

    # # convert to grayscale
    # gray = cv.cvtColor(src, cv.COLOR_BGR2GRAY)
    # # create a binary thresholded image
    # _, binary = cv.threshold(gray, 80, 255, cv.THRESH_BINARY_INV)
    # # find the contours from the thresholded image
    # contours, _ = cv.findContours(binary, cv.RETR_CCOMP, cv.CHAIN_APPROX_SIMPLE)
    # # for c in contours: print(cv.contourArea(c))
    # # Only orbs and poison
    # orbContours = [c for c in contours if 10000 < cv.contourArea(c) < 20000]

    # # match jammer and bomb
    # jammer_len = bomb_len = 0
    # """
    # Jammer detection
    # """
    # curr_pt = None
    # w, h = jammer_template.shape[::-1]
    # jm = cv.matchTemplate(gray, jammer_template, cv.TM_CCORR_NORMED)
    # threshold = 0.9
    # jammer_loc = sorted(zip(*np.where(jm >= threshold)[::-1]), key=cmp_to_key(sortLocation))
    # for pt in jammer_loc:
    #     # remove duplicates
    #     if curr_pt is None:
    #         curr_pt = pt
    #     elif abs(curr_pt[0] - pt[0]) < match_offset and abs(curr_pt[1] - pt[1]) < match_offset:
    #             continue
    #     else:
    #         curr_pt = pt
    #     # print(pt)
    #     # count and draw a box
    #     jammer_len += 1
    #     # cv.rectangle(src, pt, (pt[0] + w, pt[1] + h), (0, 0, 255), 1)
    # print("There are {} jammer(s)".format(jammer_len))
    # """
    # Bomb detection
    # """
    # curr_pt = None
    # w, h = bomb_template.shape[::-1]
    # bm = cv.matchTemplate(gray, bomb_template, cv.TM_CCOEFF_NORMED)
    # threshold = 0.515
    # bomb_loc = sorted(zip(*np.where(bm >= threshold)[::-1]), key=cmp_to_key(sortLocation))
    # for pt in bomb_loc:
    #     # remove duplicates
    #     if curr_pt is None:
    #         curr_pt = pt
    #     elif abs(curr_pt[0] - pt[0]) < match_offset and abs(curr_pt[1] - pt[1]) < match_offset:
    #             continue
    #     else:
    #         curr_pt = pt
    #     # print(pt)
    #     # count and draw a box        
    #     bomb_len += 1
    #     # cv.rectangle(src, pt, (pt[0] + w, pt[1] + h), (0, 255, 0), 1)
    # print("There are {} bomb(s)".format(bomb_len))

    # draw all contours
    # image = cv.drawContours(src, orbContours, -1, (255, 255, 255), 3)
    # cv.imshow("board", src)
    # cv.imshow("bw", binary)
    # cv.waitKey()


# %%
# 0,0 0,1 0,2 0,3 -> 0,0 0,3
def shorten(path: list) -> list:
    # copy the original list
    temp = path[:]
    prev = after = None
    l = len(path)
    count = 0
    for index, item in enumerate(path):
        if index > 0 and index < (l - 1):
            prev = path[index - 1]
            after = path[index + 1]
            # remove item if it is in the middle
            if (prev[0] == item[0] == after[0] or prev[1] == item[1] == after[1]):
                temp.remove(item)
                # make prev longer to move
                prev.append(0.5)
                count += 1
    print("Shortened {} moves".format(count)) 
    return temp

# %%
def perform(solution: list):
    print("- PERFORMING -")
    # setup everything
    left, top, end_left, end_top = board_loc
    orb_height = (end_top - top) / board_row
    x_start = left + orb_height / 2
    y_start = top + orb_height / 2

    # save current position
    (px, py) = gui.position()
    step = len(solution)
    print("{} steps in total.".format(step))
    start = time.time()
    for i in range(step):
        curr = solution[i]
        x, y = curr
        target_x = x_start + y * orb_height
        target_y = y_start + x * orb_height
        if i == 0:
            # need to hold the mouse
            gui.mouseDown(target_x, target_y, button='left')
            gui.mouseDown(target_x, target_y, button='left')            
        else:
            # NOTE: 50ms is about the minimum time for the game to recognise movements, less than it will cause some issues
            duration = 0
            # if len(curr) == 3:
            #     duration = 0.2
            # simply move to there
            # NOTE: when _pause is False, it will go very fast
            gui.moveTo(target_x, target_y, duration=duration, _pause=False)
            time.sleep(50 / 1000)
    print("Performed!")
    print("It took %.3fs." % (time.time() - start))
    # only release it when everything are all done
    gui.mouseUp()
    # move back to current position after everything
    gui.moveTo(px, py)
    gui.leftClick()

    # save solution image
    width = (end_left - left) * screen_scale
    height = (end_top - top) * screen_scale
    board_img = gui.screenshot(region=(left * screen_scale, top * screen_scale, width, height))
    board_img.save("./solution.png")

# %%
def getSolution(input: str) -> list:
    print("- SOLVING -")
    if os.path.exists("path.pazusoba"):
        os.remove("path.pazusoba")
    start = time.time()
    solution = []
    completed = False
    failed_count = 0
    # make sure a solution is written to the disk
    while not completed:
        # Ignore output from the program
        pazusoba = subprocess.Popen(['./pazusoba.exe', input, '3', '40', '9000'], stdout=subprocess.DEVNULL)
        pazusoba.wait()

        output_file = "path.pazusoba"
        if os.path.exists(output_file):
            with open(output_file, 'r') as p:
                solution = p.read().split('|')[:-1]
            # convert to int list
            solution = list(map(lambda x: list(map(int, x.split(','))), solution))
            completed = True
        else:
            failed_count += 1
            print("Failed. Retry... {}".format(failed_count))
            if failed_count > 10:
                print("Failed. Try again later.")
                break
    print("It took %.3fs." % (time.time() - start))

    return shorten(solution)
    # return solution

# %%
def shorten(solution: list) -> list:
   print("- SIMPLIFYING -")
   # insert the first
   simplified_solution = [solution[0]]
   length = len(solution)

   # loop from the second step
   for i in range(1, length - 1):
       x0 = solution[i][0] - solution[i - 1][0]
       x1 = solution[i + 1][0] - solution[i][0]
       if x0 == x1:
           y0 = solution[i][1] - solution[i - 1][1]
           y1 = solution[i + 1][1] - solution[i][1]
           if y0 == y1:
               continue
       simplified_solution.append(solution[i])

   # insert the last
   simplified_solution.append(solution[length - 1])
   print("{} steps -> {} steps".format(length, len(simplified_solution)))
   return simplified_solution


# %%
# 20, 30 or 42
orb_count = 30
auto = False
battle_count = 0

while True:
    battle_count += 1
    print("\n=== BATTLE {} ===".format((battle_count)))

    start = time.time()
    board = run()
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
        solution = getSolution(board)
        print("Solved in %.3fs.\n" % (time.time() - start))
        perform(solution)

        print('Cooling down...')
        time.sleep(4)

    if auto:
        continue
    else:
        # Ask for user input
        decision = input("Please enter to continue, q to quit: ")
        if decision.startswith("q"):
            break
