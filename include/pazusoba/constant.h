//
// constant.h
// Store flags, constants and global variables
//
// Created by Yiheng Quan on 12/11/2020
//

#ifndef _CONSTANT_H_
#define _CONSTANT_H_

#include <string>

/// when true, more logs will be printed
#define DEBUG_PRINT true
namespace pazusoba::constant {
enum orbs {
    empty = 0,
    fire,
    water,
    wood,
    light,
    dark,
    recovery,
    jammer,
    bomb,
    poison,
    poison_plus,
    seal,
    disabled,
    unknown
};

/// All 8 possible directions
enum directions {
    up_left = 0,
    up,
    up_right,
    left,
    right,
    down_left,
    down,
    down_right
};

/// This is for all profiles and based on how important it is
enum scores {
    tier_one = 10,
    tier_two = 20,
    tier_three = 30,
    tier_four = 40,
    tier_five = 50,
    tier_six = 60,
    tier_seven = 70,
    tier_eight = 80,
    tier_nine = 90,
    tier_ten = 100,
};

/// The number of all available orbs
const int orbCount = 14;

/// Name of orbs
const std::string orbNames[orbCount] = {
    "",       "Fire", "Water",  "Wood",    "Light", "Dark", "Heal",
    "Jammer", "Bomb", "Poison", "Poison+", "Tape",  "-X-",  "???"};

/// Match names https://pad.dawnglare.com/ use (not all orbs are supported)
const std::string orbNamesWeb[orbCount] = {"",  "R", "B", "G", "L", "D", "H",
                                           "J", "",  "P", "",  "",  "",  ""};

/// Weight of orbs (some are more important than others)
const int orbWeights[orbCount] = {0,  10, 10, 10, 10, 10, 30,
                                  10, 50, 5,  2,  0,  0,  0};

/// Name of directions
const std::string directionNames[8] = {"UL", "U",  "UR", "L",
                                       "R",  "DL", "D",  "DR"};
}  // namespace pazusoba::constant

#endif
