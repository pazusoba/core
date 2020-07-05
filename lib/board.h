/**
 * board.h
 * by Yiheng Quan
 */

#ifndef BOARD_H
#define BOARD_H

#include <vector>
#include <set>
#include "pad.h"

// shorten the code to make a pair
#define PAIR(x, y) (std::make_pair(x, y))

// Another name for orb enum from pad.h
typedef pad::orbs Orb;
// Board is just a 2D vector
typedef std::vector<std::vector<Orb>> Board;
// This indicates current orb's location
typedef std::pair<int, int> OrbLocation;
// This is used to get all connected orbs that can be erased
typedef std::set<OrbLocation> OrbSet;

class PadBoard
{
    int row = 0;
    int column = 0;
    // This tells the soler how to erase orb (by default, erase orbs if 3 or more of them are connected)
    int minEraseCondition = 3;
    // This saves all orbs in a 2D array, support all orb types
    Board board;
    // When it is true, more messages will be printed to the console
    bool printMoreMessages = false;

    /**
         * Read orbs from board.txt
         */
    void readBoard(std::string filePath);

    /**
         * Print out a board nicely formatted
         */
    void printBoard(Board *board);

    /**
         * Print out some info about the board we have
         */
    void printBoardInfo(Board *board);

    /**
         * Move orbs down if there is an empty orb below
         */
    void moveOrbsDown(Board *board);

    /**
         * Erase orbs that are connected in a line.
         * return - the number of combos
         */
    int eraseOrbs(Board *board);

    /**
         * Check whether there are at least 3 (4, 5 or more) same orbs around (up, down, left, right)
         * return - a set of xy that can be erased
         */
    OrbSet findSameOrbsAround(Board *board, int x, int y);

    /**
         * Check whether there is at least 1 same orb around (up, down, left, right) that is not in vhOrbs
         * return - a pair pointer that should be checked next
         */
    OrbLocation *nextSameOrbAround(Board *board, OrbSet *vhOrbs, int x, int y);

    /**
         * Check if orb at (x, y) has the same orb
         */
    bool hasSameOrb(Board *board, Orb orb, int x, int y);

    /**
         * Swap the value of two orbs
         */
    void swapOrbs(Orb *first, Orb *second);

    /**
         * Calculate max combo from a list of orbs.
         * NOTE that this is not the true MAX COMBO possible,
         * but it represents the max combo an averge player can do.
         */
    int getMaxCombo(int *counter);

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
    int *collectOrbCount(Board *board);

public:
    PadBoard();
    PadBoard(std::string filePath, int minEraseCondition = 3);
    ~PadBoard();

    /**
         * Rate current board. This is the heuristic
         * - three in a line (1000pt), based on 10^orb
         * - two in a line (100pt)
         * - more coming soon
         */
    int rateBoard(Board *board);

    void debug();
};

#endif
