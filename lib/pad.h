/**
 * pad.h
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
        disabled
    };
    const int ORB_COUNT = 13;
    // For displaying the orb name
    const std::string ORB_NAMES[ORB_COUNT] = {"", "Fire", "Water", "Wood", "Light", "Dark", "Heal", "Jammer", "Bomb", "Poison", "Poison+", "Tape", "-X-"};
    // Some emulation websites use these names for orbs (not all orbs are supported)
    const std::string ORB_SIMULATION_NAMES[ORB_COUNT] = {"", "R", "B", "G", "L", "D", "H", "J", "", "P", "", "", ""};

    // TODO: consider weight to adjust the heuristic

    /**
     * This is the MAX STEPS for 5 seconds, the default time
     * 
     * New player -> 3 - 4 steps per second 
     * Intermediate player -> 7 - 9 steps per second 
     * Pro player -> 13 - 15 steps per second 
     * Bot player -> 20 steps per second
     */
    const int MAX_STEPS = 100;

    enum score
    {
        /**
         * nearby means vertically, horizontally and diagonally
         */
        ORB_NEARBY_SCORE = 5,
        /**
         * nearby means only vertically and horizontally
         */
        ORB_AROUND_SCORE = 50,
        /**
         * score for a single combo
         */
        ONE_COMBO_SCORE = 1000,
    };
} // namespace pad

#endif
