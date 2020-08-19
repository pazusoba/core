"""
Get the board location
"""
import pyautogui as gui

input('Move to top left of the board and press enter')
one = gui.position()
input('Move to bottom right of the board and press enter')
two = gui.position()

print("[{}, {}, {}, {}]".format(one.x, one.y, two.x, two.y))
