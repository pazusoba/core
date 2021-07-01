import mss
import mss.tools
import pyautogui

def take_screenshot(monitor, write2disk=False, name="output.png"):
    # # NOTE: move the cursor outside the game screen so that it doesn't cover up the screen
    # x = monitor["height"] / 2 + monitor["top"]
    # y = monitor["width"] + monitor["left"] + 10
    # pyautogui.moveTo(y, y)

    with mss.mss() as screenshot:
        screen_raw = screenshot.grab(monitor)
        # Save to disk TODO: is this still necessary
        if write2disk:
            mss.tools.to_png(screen_raw.rgb, screen_raw.size, output=name)
        return screen_raw
