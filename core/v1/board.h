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
// OrbIndex is now just an index
typedef int OrbIndex;

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
    /// By default, if 3 orbs are connected, it is considered a combo. Up to
    int minEraseCondition = 3;
    /// This saves all orbs in an array, support all orb types
    Board board;

    /// Move orbs down if there is an empty orb below, return whether board has been changed
    bool moveOrbsDown();
    
    /// Search for a combo and erase orbs
    void floodfill(Combo *list, int x, int y, Orb orb, int direction);

    // Erase all combos, move orbs down and track the move count
    ComboList eraseComboAndMoveOrbs(int *moveCount);

    inline bool hasSameOrb(Orb orb, OrbIndex index)
    {
        if (validLocation(index))
        {
            return board[index] == orb;
        }

        return false;
    }

    /**
     * Calculate max combo from a list of orbs.
     * NOTE that this is not the true MAX COMBO possible,
     * but it represents the max combo an averge player can do.
     */
    int getMaxCombo(int *counter);

    /// A quick and easy way of getting max combo
    inline int getBoardMaxCombo()
    {
        return row * column / minEraseCondition;
    }

    /// Check if the file is empty or doesn't exists
    inline bool isEmptyFile()
    {
        return column == 0 && row == 0;
    }

    /// Loop through the vector and count the number of each orbs
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

    /// Rate current board with profiles
    int rateBoard();

    /// Print out a board nicely formatted
    void printBoard();

    /// Print out all orbs in a line and it can be used for simulation
    void printBoardForSimulation();

    /// Print out some info about the board we have
    void printBoardInfo();

    /// Traverse through the board
    inline void traverse(void func(int, int))
    {
        for (int i = 0; i < row; i++)
        {
            for (int j = 0; j < column; j++)
            {
                func(i, j);
            }
        }
    }

    inline bool hasSameBoard(const PBoard *b) const
    {
        return board == b->board;
    }

    /// Swap the value of two orbs
    inline void swapLocation(OrbIndex one, OrbIndex two)
    {
        if (!validLocation(one) || !validLocation(two))
            return;

        auto temp = board[one];
        board[one] = board[two];
        board[two] = temp;
    }

    inline bool validLocation(OrbIndex index)
    {
        int x = index / column;
        int y = index % column;
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
