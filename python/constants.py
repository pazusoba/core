"""
constant.py
- store all constants used by the program
"""

from enum import Enum

class Orb(Enum):
    EMPTY = 0
    FIRE = 1
    WATER = 2
    WOOD = 3
    LIGHT = 4
    DARK = 5
    HEAL = 6
    JAMMER = 7
    BOMB = 8
    POISON = 9
    POISON_PLUS = 10
    SEAL = 11
    DISABLED = 12
    UNKNOWN = 13

NUM_ORBS = 14
ORB_NAMES = ["Empty", "Fire", "Water", "Wood", "Light", "Dark", "Heal", "Jammer", "Bomb", "Poison", "Poison+", "Tape", "-X-", "???"]
ORB_SIMULATION_NAMES = ["", "R", "B", "G", "L", "D", "H", "J", "", "P", "", "", "", ""]

class Direction(Enum):
    UP_LEFT = 0
    UP = 1
    UP_RIGHT = 2
    LEFT = 3
    RIGHT = 4
    DOWN_LEFT = 5
    DOWN = 6
    DOWN_RIGHT = 7

# 8 directions in total
DIRECTION_NAMES = ["UL", "U", "UR", "L", "R", "DL", "D", "DR"]

# scores, max 1000 meaning one combo
TIER_ONE_SCORE = 1
TIER_TWO_SCORE = 3
TIER_THREE_SCORE = 6
TIER_FOUR_SCORE = 12
TIER_FIVE_SCORE = 25
TIER_SIX_SCORE = 50
TIER_SEVEN_SCORE = 100
TIER_EIGHT_SCORE = 200
TIER_EIGHT_PLUS_SCORE = 300
TIER_NINE_SCORE = 500
TIER_TEN_SCORE = 1000
