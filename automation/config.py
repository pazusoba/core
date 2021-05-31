#
# Constants
# Modify them before running
#

# The cycle of automation
CYCLE_DURATION = 500

# Game is the entire game without status bar
# Board is only the board area
GAME_LOCATION  = [149, 122, 1291, 2086]
BOARD_LOCATION = [0, 0, 0, 0]

# When in DEBUG mode, more texts will be printed
DEBUG_MODE = True

# This shouldn't be changed. Used to resize ORB to a uniform size
ORB_TEMPLATE_SIZE = (140, 140)

# Resize for game ratio 2:1, 16:9 and 3:2
GAME_SCREEN_SIZE_2_1  = (1000, 1950)
GAME_SCREEN_SIZE_16_9 = (1000, 1720)
GAME_SCREEN_SIZE_3_2  = (1000, 1440)

# On Mac OS, the scale might be 2 instead of 1 because of the retina display
SCREEN_SCALE = 1

# Added paddings to images
BORDER_LENGTH = 1

# This is used to ignore similar matches by how close they are using this offset
SORT_OFFSET = 100

#
# Variables
# They will get updated in run time
#

BOARD_COLUMN = 0
BOARD_ROW = 0
