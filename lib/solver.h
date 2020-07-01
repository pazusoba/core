/**
 * solver.h
 * by Yiheng Quan
 */

#ifndef PAD_SOLVER_H
#define PAD_SOLVER_H

#include <string>
#include <vector>

namespace pad {
    /**
     * All orbs in the game.
     * 
     * seal is a tape over a row and orbs cannot move,
     * disabled means it will not get erased even if connected.
     * 
     * Also, consider weight because it can be quite important
     */
    enum orbs { empty, fire, water, wood, light, dark, recovery, jammer, bomb, poison, poison_plus, seal, disabled };
    const int ORB_COUNT = 13;
    // For displaying the orb name
    const std::string ORB_NAMES[ORB_COUNT] = { "Empty", "Fire", "Water", "Wood", "Light", "Dark", "Heal", "Jammer", "Bomb", "Poison", "Poison+", "---", "-X-"};
}

/**
 * This solve the board 
 */
class PadSolver {
    public:
        PadSolver();

        /**
         * Print out a board nicely formatted
         */
        void printBoard();

        /**
         * Calculate max combo from a list of orbs
         */
        int getMaxCombo();
    private:
        int row = 0;
        int column = 0;
        // This saves all orbs, it supports numbers more than 10
        std::vector<pad::orbs> board;

        /**
         * Read orbs from board.txt
         */
        void readBoard();
};

#endif
