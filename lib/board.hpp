/**
 * board.hpp
 * by Yiheng Quan
 */

#ifndef BOARD_H
#define BOARD_H

#include <vector>
#include <set>
#include "pad.hpp"

// shorten the code to make a pair
#define LOCATION(x, y) (std::make_pair(x, y))
// shotern the code to get/set an orb from the board
#define ORB(board, location) (board[location.first][location.second])

// Another name for orb enum from pad.hpp
typedef pad::orbs Orb;
// Row is a list of Orb
typedef std::vector<Orb> Row;
// Board is just a 2D vector
typedef std::vector<Row> Board;
// This indicates current orb's location
typedef std::pair<int, int> OrbLocation;
// This is used to get all connected orbs that can be erased
typedef std::set<OrbLocation> OrbSet;

class PBoard
{
     int row;
     int column;
     // This tells the soler how to erase orb (by default, erase orbs if 3 or more of them are connected)
     int minEraseCondition = 3;
     // This saves all orbs in a 2D array, support all orb types
     Board board;
     // When it is true, more messages will be printed to the console
     bool printMoreMessages = false;

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
         * return - a set of xy that can be erased
         */
     OrbSet findSameOrbsAround(int x, int y);
     OrbSet findSameOrbsAround(OrbLocation loc);

     /**
         * Check whether there is at least 1 same orb around (up, down, left, right) that is not in vhOrbs
         * return - a pair pointer that should be checked next
         */
     OrbLocation *nextSameOrbAround(OrbSet *vhOrbs, int x, int y);
     OrbLocation *nextSameOrbAround(OrbSet *vhOrbs, OrbLocation loc);

     /**
         * Check if orb at (x, y) has the same orb
         */
     bool hasSameOrb(Orb orb, int x, int y);
     bool hasSameOrb(Orb orb, OrbLocation loc);

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
     int *collectOrbCount();

public:
     PBoard();
     PBoard(const Board &board, int row, int column, int minEraseCondition = 3);
     ~PBoard();

     /**
         * Rate current board. This is the heuristic
         * - three in a line (1000pt), based on 10^orb
         * - two in a line (100pt)
         * - more coming soon
         */
     int rateBoard(int step);

     /**
         * Print out a board nicely formatted
         */
     void printBoard();

     /**
     * Print out all orbs in a line and it can be used for simulation
     */
     void printBoardForSimulation();

     /**
         * Print out some info about the board we have
         */
     void printBoardInfo();

     /** 
      * Update the unique ID after update
      */
     std::string getBoardID();

     /**
         * Swap the value of two orbs
         */
     void swapLocation(OrbLocation one, OrbLocation two);

     // Check if this loc is valid (not out of index)
     bool validLocation(OrbLocation loc);
     bool validLocation(int x, int y);
};

#endif
