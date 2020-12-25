/**
 * board.cpp
 * by Yiheng Quan
 */

#include <iostream>
#include <sstream>
#include <array>
#include <set>
#include "board.h"
#include "profile.h"

// MARK: - Constructors
PBoard::PBoard() {}
PBoard::PBoard(const Board &board, int row, int column, int minEraseCondition)
{
    this->board = board;
    this->row = row;
    this->column = column;
    this->minEraseCondition = minEraseCondition;
}

// MARK: - Board related
ComboList PBoard::eraseComboAndMoveOrbs(int *moveCount)
{
    // Reverse to prevent slow pushing back
    ComboList comboList;
    comboList.reserve(row * column / minEraseCondition);
    Combo combo;
    combo.reserve(row * column);

    bool moreCombo;
    do
    {
        // Remember to reset
        moreCombo = false;

        // from bottom to top
        for (int i = column - 1; i >= 0; i--)
        {
            for (int j = 0; j < row; j++)
            {
                auto loc = OrbLocation(i, j, column);
                auto orb = board[loc.index];
                // Ignore empty orbs
                if (orb == pad::empty)
                    continue;

                // Start finding combos
                floodfill(&combo, loc, orb, -1);
                if ((int)combo.size() >= minEraseCondition)
                {
                    moreCombo = true;
                    comboList.push_back(combo);
                    combo.clear();
                }
            }
        }

        // Only move orbs down if there is at least one combo
        if (moreCombo)
        {
            // If the board is not moved, there won't be new combos so break;
            moreCombo = moveOrbsDown();
            *moveCount += 1;
        }
    } while (moreCombo);

    return comboList;
}

void PBoard::floodfill(Combo *list, const OrbLocation &loc, const Orb &orb, int direction)
{
    if (!validLocation(loc))
        return;

    auto currOrb = board[loc.index];
    // must be the same orb
    if (currOrb != orb)
        return;

    int count = 0;
    // track all directions
    int dList[4] = {0, 0, 0, 0};
    int x = loc.first;
    int y = loc.second;

    // all 4 directions
    // 0 -> right
    // 1 -> left
    // 2 -> down
    // 3 -> up
    for (int d = 0; d < 4; d++)
    {
        int loop = row;
        if (d > 1)
            loop = column;

        for (int i = 0; i < loop; i++)
        {
            int cx = x;
            int cy = y;
            if (d == 0)
                cy += i;
            else if (d == 1)
                cy -= i;
            else if (d == 2)
                cx += i;
            else if (d == 3)
                cx -= i;

            auto loc = OrbLocation(cx, cy, column);
            if (!validLocation(loc))
                break;

            // stop if it doesn't match
            if (board[loc.index] != orb)
                break;

            dList[d]++;
            count++;
        }
    }

    // added the same orb 4 times so need to -3
    count -= 3;

    // more than erase condition
    if (count >= minEraseCondition)
    {
        // the min number of connected orbs to consider it as a combo
        int minConnection = minEraseCondition >= 3 ? 3 : minEraseCondition;

        // erase and do flood fill
        for (int d = 0; d < 4; d++)
        {
            int currCount = dList[d];
            // must be more than min erase
            if (currCount < minConnection)
                continue;

            for (int i = 0; i < currCount; i++)
            {
                int cx = x;
                int cy = y;
                if (d == 0)
                    cy += i;
                else if (d == 1)
                    cy -= i;
                else if (d == 2)
                    cx += i;
                else if (d == 3)
                    cx -= i;

                // only need to check two orbs because by default, it needs to have at least 3 connected orbs to be considered as a combo
                bool hasOrbAround = false;
                if (d < 2)
                {
                    // check up down
                    hasOrbAround = hasSameOrb(orb, OrbLocation(cx + 1, cy, column)) && hasSameOrb(orb, OrbLocation(cx - 1, cy, column));
                }
                else
                {
                    // check left right
                    hasOrbAround = hasSameOrb(orb, OrbLocation(cx, cy + 1, column)) && hasSameOrb(orb, OrbLocation(cx, cy - 1, column));
                }

                // only remove if there are no same orbs in a different direction
                if (!hasOrbAround)
                {
                    board[INDEX_OF(cx, cy)] = pad::empty;
                    list->emplace_back(cx, cy, orb);
                }
            }

            for (int i = 0; i < currCount; i++)
            {
                int cx = x;
                int cy = y;
                if (d == 0)
                    cy += i;
                else if (d == 1)
                    cy -= i;
                else if (d == 2)
                    cx += i;
                else if (d == 3)
                    cx -= i;

                // fill all directions here
                floodfill(list, OrbLocation(cx, cy + 1, column), orb, 0);
                floodfill(list, OrbLocation(cx, cy - 1, column), orb, 1);
                floodfill(list, OrbLocation(cx + 1, cy, column), orb, 2);
                floodfill(list, OrbLocation(cx - 1, cy, column), orb, 3);
            }
        }
    }
}

int PBoard::rateBoard()
{
    int moveCount = 0;
    auto list = eraseComboAndMoveOrbs(&moveCount);

    int score = ProfileManager::shared().getScore(list, board, moveCount, minEraseCondition, row, column);

    return score;
}

bool PBoard::moveOrbsDown()
{
    bool changed = false;
    for (int j = 0; j < row; j++)
    {
        std::vector<pad::orbs> orbs;
        orbs.reserve(column);
        int emptyCount = 0;
        // Start checking from the bottom most column
        for (int i = column - 1; i >= 0; i--)
        {
            auto orb = board[INDEX_OF(i, j)];
            if (orb != pad::empty)
            {
                orbs.push_back(orb);
            }
            else
            {
                emptyCount++;
            }
        }

        // Must be less than column (it means that this column is all empty)
        if (emptyCount > 0 && emptyCount < column)
        {
            // Fill the saved orbs
            int k = 0, s = (int)orbs.size();
            for (int i = column - 1; i >= 0; i--, k++)
            {
                int index = INDEX_OF(i, j);
                if (k >= s)
                {
                    board[index] = pad::empty;
                }
                else
                {
                    // If column is 5, i starts from 4 so the index of orb is 5 - 1 - 4 = 0
                    auto orb = orbs[k];
                    auto currOrb = board[index];
                    board[index] = orb;

                    // Board is changed if currOrb is not actually orb
                    if (currOrb != orb)
                        changed = true;
                }
            }
        }
    }
    return changed;
}

void PBoard::printBoard()
{
    if (isEmptyFile())
    {
        std::cout << "- empty -\n";
        return;
    }

    // Print everything out nicely
    std::cout << std::endl;
    std::cout << row << " x " << column << std::endl;
    for (int i = 0; i < row; i++)
    {
        for (int j = 0; j < column; j++)
        {
            auto orb = board[INDEX_OF(i, j)];
            std::cout << pad::ORB_NAMES[orb] << "\t";
        }
        std::cout << std::endl;
    }
    std::cout << std::endl;
}

void PBoard::printBoardForSimulation()
{
    std::stringstream ss;
    for (int i = 0; i < row * column; i++)
    {
        auto orb = board[i];
        std::cout << pad::ORB_SIMULATION_NAMES[orb];
        ss << (int)(orb - 1);
    }
    std::cout << std::endl;
    std::cout << ss.str() << std::endl;
}

void PBoard::printBoardInfo()
{
    if (isEmptyFile())
    {
        std::cout << "no info\n";
        return;
    }

    // Collect orb info
    int *counter = collectOrbCount();

    // Print out some board info
    for (int i = 1; i < pad::ORB_COUNT; i++)
    {
        int count = counter[i];
        if (count == 0)
            continue;
        // It is just like fire x 5 wood x 6
        std::cout << count << " x " << pad::ORB_NAMES[i];
        if (i != pad::ORB_COUNT - 1)
            std::cout << " | ";
    }
    std::cout << std::endl;

    std::cout << "Board max combo: " << getBoardMaxCombo() << std::endl;
    std::cout << "Current max combo: " << getMaxCombo(counter) << std::endl;

    delete[] counter;
}

// MARK: - Utils
int PBoard::getMaxCombo(int *counter)
{
    if (isEmptyFile())
        return 0;

    int comboCounter = 0;

    int moreComboCount;
    do
    {
        // Reset this flag everytime
        moreComboCount = 0;
        // This tracks how many orbs are left
        int orbLeft = 0;
        int maxOrbCounter = 0;

        for (int i = 1; i < pad::ORB_COUNT; i++)
        {
            // Keep -3 or other minEraseCondition (4, 5) until all orbs are less than 2
            int curr = counter[i];
            if (curr >= minEraseCondition)
            {
                moreComboCount += 1;
                comboCounter++;
                counter[i] -= minEraseCondition;
                if (curr > maxOrbCounter)
                    maxOrbCounter = curr;
            }
            else
            {
                orbLeft += curr;
            }
        }

        // Only one colour can do combo now
        if (moreComboCount == 1)
        {
            // Orbs left are in different colour but they can still seperate other colours
            int maxComboPossible = orbLeft / minEraseCondition;
            int maxCombo = maxOrbCounter / minEraseCondition;
            // Make sure there are enough orbs
            comboCounter += maxCombo > maxComboPossible ? maxComboPossible : maxCombo;

            // No orbs left means one colour, -1 here because we count one more but doesn't apply to a single colour board
            if (orbLeft > 0)
                comboCounter--;
            break;
        }
    } while (moreComboCount > 0);

    return comboCounter;
}
