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
        // This tells the soler how to erase orb (by default, erase orbs if 3 of them are connected)
        int minEraseCondition = 3;
        // This saves all orbs in a 2D array, support all orb types
        std::vector<std::vector<pad::orbs>> board;

        /**
         * Read orbs from board.txt
         */
        void readBoard(std::string filePath);

        /**
         * Move orbs down if there is an empty orb below
         */
        void moveOrbsDown();

        /**
         * Erase orbs that are connected in a line.
         * return - the number of combos
         */
        int eraseOrbs();

        /**
         * Check whether there are at least 3 (4, 5 or more) same orbs around (up, down, left, right)
         * return - true as soon as it meets the condition
         */
        bool shouldEraseOrb(int x, int y);

        /**
         * Check whether the orb at (x, y) is the same as orb.
         * This will check if (x, y) is a valid orb.
         * x and y starts from 0, top left
         */
        bool hasSameOrb(int x, int y, pad::orbs orb);

        /**
         * Swap the value of two orbs
         */
        void swapOrbs(pad::orbs* first, pad::orbs* second);

        /**
         * Calculate max combo from a list of orbs.
         * NOTE that this is not the true MAX COMBO possible,
         * but it represents the max combo an averge player can do.
         */
        int getMaxCombo(int* counter);

        /**
         * Max combo is simply row x column / 3
         */
        int getBoardMaxCombo();

        /**
         * Check if the file is empty or doesn't exists
         */
        bool isEmptyFile();

        /**
         * Loop through the vector and count the number of each orbs
         */
        int* collectOrbCount();

    public:
        PadSolver(std::string filePath);
        PadSolver(std::string filePath, int minEraseCondition);
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
         * Solve current board
         */
        void solveBoard();
};

#endif
