import mss
import mss.tools

def take_screenshot(monitor, write2disk=False):
    with mss.mss() as screenshot:
        screen_raw = screenshot.grab(monitor)
        # Save to disk TODO: is this still necessary
        if write2disk:
            mss.tools.to_png(screen_raw.rgb, screen_raw.size, output="output.png")
        return screen_raw
