/**
 * board.h
 * by Yiheng Quan
 */

#ifndef BOARD_H
#define BOARD_H

#include <vector>
#include <array>
#include <set>
#include "pad.h"

// Another name for orb enum from pad.h
typedef pad::orbs Orb;
// Board is an array of Orb, for now max 7x6 so 42
typedef std::array<Orb, 42> Board;

// Convert location to index
#define index(x, y) (x * row + y)

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

    inline bool hasSameOrb(Orb orb, int x, int y)
    {
        if (validLocation(x, y))
        {
            return board[index(x, y)] == orb;
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
        traverse([&](int i, int j) {
            auto orb = board[index(i, j)];
            counter[orb]++;
        });
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

    /// Traverse through the board
    inline void traverse(void action(int, int))
    {
        for (int i = 0; i < row; i++)
        {
            for (int j = 0; j < column; j++)
            {
                action(i, j);
            }
        }
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
};

#endif
