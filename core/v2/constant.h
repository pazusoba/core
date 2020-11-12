//
// constant.h
// Store flags, constants and global variables
//
// Created by Yiheng Quan on 12/11/2020
//

#ifndef _CONSTANT_H_
#define _CONSTANT_H_

#include <string>

using namespace std;
namespace pazusoba
{
    namespace pad
    {
        // Flags
        bool debug = true;

        enum ORB
        {
            empty,
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
        enum DIRECTION
        {
            up_left,
            up,
            up_right,
            left,
            right,
            down_left,
            down,
            down_right
        };

        /// This is for all profiles and based on how important it is
        enum SCORE
        {
            TIER_ONE_SCORE = 1,
            TIER_TWO_SCORE = 5,
            TIER_THREE_SCORE = 10,
            TIER_FOUR_SCORE = 20,
            TIER_FIVE_SCORE = 50,
            TIER_SIX_SCORE = 100,
            TIER_SEVEN_SCORE = 500,
            TIER_EIGHT_SCORE = 1000,
            TIER_EIGHT_PLUS_SCORE = 2500,
            TIER_NINE_SCORE = 5000,
            TIER_TEN_SCORE = 10000,
        };

        /// The number of all available orbs
        const int ORB_COUNT = 14;

        /// Names of every orb
        const string ORB_NAMES[ORB_COUNT] = {"", "Fire", "Water", "Wood", "Light", "Dark", "Heal", "Jammer", "Bomb", "Poison", "Poison+", "Tape", "-X-", "???"};

        /// Match names some websites use (not all orbs are supported)
        const string ORB_NAMES_WEB[ORB_COUNT] = {"", "R", "B", "G", "L", "D", "H", "J", "", "P", "", "", "", ""};

        // TODO: consider weight to adjust the heuristic

        /// Names of every direction
        const string DIRECTION_NAMES[8] = {"UL", "U", "UR", "L", "R", "DL", "D", "DR"};
    } // namespace pad
} // namespace pazusoba

#endif
