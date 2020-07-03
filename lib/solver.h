/**
 * solver.h
 * by Yiheng Quan
 */

#ifndef PAD_SOLVER_H
#define PAD_SOLVER_H

#include <string>
#include <vector>
#include "pad.h"

/**
 * This solve the board 
 */
class PadSolver {
    private:
        int row = 0;
        int column = 0;
        // This saves all orbs in a 2D array, support all orb types
        std::vector<std::vector<pad::orbs>> board;

        /**
         * Read orbs from board.txt
         */
        void readBoard(std::string filePath);

        /**
         * Check if the file is empty or doesn't exists
         */
        bool isEmptyFile();
    public:
        PadSolver(std::string filePath);
        ~PadSolver();

        /**
         * Print out a board nicely formatted
         */
        void printBoard();

        /**
         * Print out some info about the board we have
         */
        void printBoardInfo();

        /**
         * Calculate max combo from a list of orbs
         */
        int getMaxCombo();
};

#endif
