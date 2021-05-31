"""
Utilities
"""

import time
from config import ONE_CYCLE

def waitForNextCycle():
    waitForCycles(1)

def waitForCycles(count: int):
    # at least, one cycle here
    time.sleep((ONE_CYCLE * max(count, 1)) / 1000)
