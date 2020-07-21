/**
 * pad.hpp
 * by Yiheng Quan
 */

#ifndef PAD_H
#define PAD_H

#include <string>

namespace pad
{
    /**
     * All orbs in the game.
     * 
     * seal is a tape over a row and orbs cannot move,
     * disabled means it will not get erased even if connected.
     */
    enum orbs
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
    const int ORB_COUNT = 14;
    // For displaying the orb name
    const std::string ORB_NAMES[ORB_COUNT] = {"", "Fire", "Water", "Wood", "Light", "Dark", "Heal", "Jammer", "Bomb", "Poison", "Poison+", "Tape", "-X-", "???"};
    // Some emulation websites use these names for orbs (not all orbs are supported)
    const std::string ORB_SIMULATION_NAMES[ORB_COUNT] = {"", "R", "B", "G", "L", "D", "H", "J", "", "P", "", "", "", ""};

    // TODO: consider weight to adjust the heuristic

    // All 8 possible directions to move to
    enum direction
    {
        upLeft,
        up,
        upRight,
        left,
        right,
        downLeft,
        down,
        downRight
    };
    // Direction names
    const std::string DIRECTION_NAMES[8] = {"UP LEFT", "UP", "UP RIGHT", "LEFT", "RIGHT", "DOWN LEFT", "DOWN", "DOWN RIGHT"};

    enum score
    {
        /**
         * nearby means vertically, horizontally and diagonally
         */
        ORB_NEARBY_SCORE = 1,
        /**
         * nearby means only vertically and horizontally
         */
        ORB_AROUND_SCORE = 5,
        /**
         * encourage to cascade
         */
        CASCADE_SCORE = 20,
        /**
         * score for a single combo
         */
        ONE_COMBO_SCORE = 1000,
    };
} // namespace pad

#endif
