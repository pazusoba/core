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
#include "configuration.h"

/// Convert location to index
#define INDEX_OF(x, y) (x * column + y)

/// Another name for orb enum from pad.h
typedef pad::orbs Orb;
/// Board is an array of Orb, for now max 7x6 so 42
typedef std::array<Orb, 42> Board;

/// Convert index to include first (x) and second (y)
struct OrbLocation
{
    int index;
    int first;
    int second;
    int column = Configuration::shared().getColumn();

    bool operator==(const OrbLocation &loc) const
    {
        return first == loc.first && second == loc.second;
    }

    OrbLocation() {}
    OrbLocation(int index) : index(index)
    {
        first = index / column;
        second = index % column;
    }
    // column can be ignored sometimes
    OrbLocation(int first, int second) : first(first), second(second)
    {
        index = first * column + second;
    }
};

/// Struct for storing combo info
struct ComboInfo
{
    int first;
    int second;
    Orb orb;
    ComboInfo(int f, int s, const Orb &o) : first(f), second(s), orb(o) {}
};
typedef std::vector<ComboInfo> Combo;
typedef std::vector<Combo> ComboList;

class PBoard
{
    int row;
    int column;
    int minErase;

    /// This saves all orbs in an array, support all orb types
    Board board;

    /// Move orbs down if there is an empty orb below, return whether board has been changed
    bool moveOrbsDown();

    /// Search for a combo and erase orbs
    void floodfill(Combo *list, const OrbLocation &loc, const Orb &orb, int direction);

    // Erase all combos, move orbs down and track the move count
    ComboList eraseComboAndMoveOrbs(int *moveCount);

    inline bool hasSameOrb(const Orb &orb, const OrbLocation &loc)
    {
        if (validLocation(loc))
        {
            return board[loc.index] == orb;
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
        return row * column / minErase;
    }

    /// Check if the file is empty or doesn't exists
    inline bool isEmptyFile()
    {
        return column == 0 && row == 0;
    }

    inline int getIndex(int x, int y)
    {
        return y * column + x;
    }

    /// Loop through the vector and count the number of each orbs
    inline int *collectOrbCount()
    {
        int *counter = new int[pad::ORB_COUNT]{0};
        for (int i = 0; i < row; i++)
        {
            for (int j = 0; j < column; j++)
            {
                auto orb = board[INDEX_OF(i, j)];
                counter[orb]++;
            }
        }
        return counter;
    }

public:
    PBoard() {}
    PBoard(const Board &board);

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
    inline void swapLocation(const OrbLocation &one, const OrbLocation &two)
    {
        if (!validLocation(one) || !validLocation(two))
            return;

        auto temp = board[one.index];
        board[one.index] = board[two.index];
        board[two.index] = temp;
    }

    inline bool validLocation(const OrbLocation &loc)
    {
        int x = loc.first;
        int y = loc.second;
        if (x >= 0 && x < row && y >= 0 && y < column)
        {
            // You cannot move a sealed orb
            // return board[x][y] != pad::seal;
            return true;
        }

        return false;
    }
};

#endif
