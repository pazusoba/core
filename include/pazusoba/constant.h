//
// constant.h
// Store flags, constants and global variables
//
// Created by Yiheng Quan on 12/11/2020
//

#ifndef _CONSTANT_H_
#define _CONSTANT_H_

#include <fmt/color.h>

namespace pazusoba {
/// when true, more logs will be printed
#define DEBUG_MODE true
// 7x6
#define MAX_BOARD_SIZE 42
// NOTE: if memory is not a concern, use int instead
typedef char orb;

namespace constant {
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

// see https://stackoverflow.com/a/3088477 for why double const is needed
// this makes sure that orbNames is not used multiple times
/// Name of orbs
const char* const orbNames[orbCount] = {
    "",       "Fire", "Water",  "Wood",    "Light", "Dark", "Heal",
    "Jammer", "Bomb", "Poison", "Poison+", "Tape",  "-X-",  "???"};

/// Match names https://pad.dawnglare.com/ use (not all orbs are supported)
const char* const orbWebNames[orbCount] = {"",  "R", "B", "G", "L", "D", "H",
                                           "J", "",  "P", "",  "",  "",  ""};

/// Match names http://serizawa.web5.jp/puzzdra_theory_maker/ use
const char* const orbWebMakerNames[orbCount] = {
    "", "0", "2", "1", "3", "4", "5", "6", "", "7", "", "", "", ""};

/// Weight of orbs (some are more important than others)
const int orbWeights[orbCount] = {0,  10, 10, 10, 10, 10, 30,
                                  10, 50, 5,  2,  0,  0,  0};

/// Name of directions
const char* const directionNames[8] = {"UL", "U",  "UR", "L",
                                       "R",  "DL", "D",  "DR"};

// Console colours for each orb
const fmt::color orbColors[orbCount] = {
    fmt::color::black,      fmt::color::red,         fmt::color::light_blue,
    fmt::color::green,      fmt::color::yellow,      fmt::color::purple,
    fmt::color::light_pink, fmt::color::brown,       fmt::color::dark_red,
    fmt::color::white,      fmt::color::ghost_white, fmt::color::black,
    fmt::color::black,      fmt::color::black};
}  // namespace constant
}  // namespace pazusoba

#endif
