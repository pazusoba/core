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
#define LOCATION(x, y) (std::make_pair(x, y))
// shotern the code to get/set an orb from the board
#define ORB(board, location) (board[location.first][location.second])

// Another name for orb enum from pad.h
typedef pad::orbs Orb;
// Row is a list of Orbs
typedef std::vector<Orb> Row;
// Board is just a 2D vector
typedef std::vector<Row> Board;
// This indicates current orb's location
typedef std::pair<int, int> OrbLocation;
// This is used to get all connected orbs that can be erased
typedef std::set<OrbLocation> OrbSet;
// Combo needs to save orb locations

// A special structure for ComboInfo
struct ComboInfo
{
    int first;
    int second;
    Orb orb;
    ComboInfo(int f, int s, Orb o) : first(f), second(s), orb(o) {}
};
typedef std::vector<ComboInfo> Combo;
typedef std::vector<Combo> ComboList;

class PBoard
{
    int row;
    int column;
    // This tells the soler how to erase orb (by default, erase orbs if 3 or more of them are connected)
    int minEraseCondition = 3;
    // This saves all orbs in a 2D array, support all orb types
    Board board;

    /**
     * Move orbs down if there is an empty orb below, return whether board has been changed
     */
    bool moveOrbsDown();
    void floodfill(Combo *list, int x, int y, Orb orb, int direction);

    // Erase all combos, move orbs down and track the move count
    ComboList eraseComboAndMoveOrbs(int *moveCount);

    /**
     * Check whether there are at least 3 (4, 5 or more) same orbs around (up, down, left, right)
     * return - a set of xy that can be erased
     */
    OrbSet findSameOrbsAround(int x, int y);
    inline OrbSet findSameOrbsAround(OrbLocation loc)
    {
        return findSameOrbsAround(loc.first, loc.second);
    }

    /**
     * Check whether there is at least 1 same orb around (up, down, left, right) that is not in vhOrbs
     * return - a pair pointer that should be checked next
     */
    OrbLocation nextSameOrbAround(OrbSet *vhOrbs, int x, int y);
    inline OrbLocation nextSameOrbAround(OrbSet *vhOrbs, OrbLocation loc)
    {
        return nextSameOrbAround(vhOrbs, loc.first, loc.second);
    }

    /**
     * Check if orb at (x, y) has the same orb
     */
    inline bool hasSameOrb(Orb orb, OrbLocation loc)
    {
        return hasSameOrb(orb, loc.first, loc.second);
    }

    inline bool hasSameOrb(Orb orb, int x, int y)
    {
        if (validLocation(x, y))
        {
            return board[x][y] == orb;
        }

        return false;
    }

    /**
     * Calculate max combo from a list of orbs.
     * NOTE that this is not the true MAX COMBO possible,
     * but it represents the max combo an averge player can do.
     */
    int getMaxCombo(int *counter);

    /**
     * Max combo is simply row x column / 3
     */
    inline int getBoardMaxCombo()
    {
        return row * column / minEraseCondition;
    }

    /**
     * Check if the file is empty or doesn't exists
     */
    inline bool isEmptyFile()
    {
        return column == 0 && row == 0;
    }

    /**
     * Loop through the vector and count the number of each orbs
     */
    inline int *collectOrbCount()
    {
        int *counter = new int[pad::ORB_COUNT]{0};
        for (auto const &row : board)
        {
            for (auto const &orb : row)
            {
                counter[orb]++;
            }
        }
        return counter;
    }

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
    int rateBoard();

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

    // This is for displaying the board in QT
    inline std::vector<int> getBoardOrbs()
    {
        std::vector<int> orbs;
        orbs.reserve(row * column);
        for (auto const &row : board)
        {
            for (auto const &orb : row)
            {
                // , is important because you have 10 which can be 1 0 or just 10
                orbs.push_back(orb);
            }
        }
        return orbs;
    }

    inline bool hasSameBoard(const PBoard *b) const
    {
        return board == b->board;
    }

    /**
     * Swap the value of two orbs
     */
    inline void swapLocation(OrbLocation one, OrbLocation two)
    {
        // TODO: all points should be valid why?
        if (!validLocation(one) || !validLocation(two))
            return;

        auto temp = ORB(board, one);
        ORB(board, one) = ORB(board, two);
        ORB(board, two) = temp;
    }

    // Check if this loc is valid (not out of index)
    inline bool validLocation(OrbLocation loc)
    {
        return validLocation(loc.first, loc.second);
    }

    inline bool validLocation(int x, int y)
    {
        if (x >= 0 && x < column && y >= 0 && y < row)
        {
            // You cannot move a sealed orb
            // return board[x][y] != pad::seal;
            return true;
        }

        return false;
    }

    inline int orbLocationKey(const OrbLocation &loc)
    {
        return loc.first * 10000 + loc.second;
    }
};

#endif
