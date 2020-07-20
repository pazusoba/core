/**
 * board.cpp
 * by Yiheng Quan
 */

#include <iostream>
#include <sstream>
#include <cstdlib>
#include <ctime>
#include "board.hpp"

/// Constructors
PBoard::PBoard() {}
PBoard::PBoard(const Board &board, int row, int column, int minEraseCondition)
{
    this->board = board;
    this->row = row;
    this->column = column;
    this->minEraseCondition = minEraseCondition;
}

PBoard::~PBoard()
{
    for (auto &row : board)
    {
        row.clear();
    }
    board.clear();
}

/// Board related

int PBoard::rateBoard(int step)
{
    int score = 0;

    // total combo
    int combo = 0;
    // If you move more, you get more combo in real life but sometimes not
    int moveCount = 0;
    int newCombo = 0;
    do
    {
        newCombo = eraseOrbs();
        if (newCombo > 0)
        {
            combo += newCombo;
            // Even if it has at least one new combo
            moveOrbsDown();
            moveCount++;
        }
    } while (newCombo > 0);

    // See if the remaining orbs are close to each other
    int orbAround = 0;
    int orbNearby = 0;
    for (int i = 0; i < column; i++)
    {
        for (int j = 0; j < row; j++)
        {
            auto curr = board[i][j];
            if (curr == pad::empty)
                continue;
            // Check if there are same orbs around
            for (int a = -1; a <= 1; a++)
            {
                for (int b = -1; b <= 1; b++)
                {
                    // This is the current orb
                    if (a == 0 && b == 0)
                        continue;
                    if (hasSameOrb(curr, i + a, j + b))
                    {
                        orbAround++;
                        if ((a == 0 && ((b == 1) || (b == -1))) ||
                            (b == 0 && ((a == 1) || (a == -1))))
                        {
                            // This means that it is a line
                            orbNearby += 1;
                            orbAround -= 1;
                        }
                    }
                }
            }
        }
    }

    score += pad::ORB_AROUND_SCORE * orbAround;
    score += pad::ORB_NEARBY_SCORE * orbNearby;

    score += pad::ONE_COMBO_SCORE * combo;
    score += pad::CASCADE_SCORE * moveCount;

    // // Try out amen puzzle
    // int orbLeft = 0;
    // for (int i = 0; i < column; i++)
    // {
    //     for (int j = 0; j < row; j++)
    //     {
    //         auto curr = board[i][j];
    //         if (curr != pad::empty)
    //             orbLeft++;
    //     }
    // }

    // int comboMoreOrLess = abs(combo - 7);
    // int orbLeftMore = orbLeft - 3;
    // if (comboMoreOrLess == 0 && orbLeftMore <= 0)
    // {
    //     score += 7000;
    // }

    // score -= orbLeftMore * pad::ONE_COMBO_SCORE;
    // score -= comboMoreOrLess * pad::CASCADE_SCORE;

    if (printMoreMessages)
        std::cout << "That was " << combo << " combo\n";
    return score;
}

void PBoard::moveOrbsDown()
{
    for (int j = 0; j < row; j++)
    {
        std::vector<pad::orbs> orbs;
        int emptyCount = 0;
        // Start checking from the bottom most column
        for (int i = column - 1; i >= 0; i--)
        {
            auto orb = board[i][j];
            if (pad::empty != orb)
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
            for (int i = column - 1; i >= 0; --i, ++k)
            {
                if (k >= s)
                {
                    board[i][j] = pad::empty;
                }
                else 
                {
                    // If column is 5, i starts from 4 so the index of orb is 5 - 1 - 4 = 0
                    auto orb = orbs[k];
                    board[i][j] = orb;
                }
            }
        }
    }
}

int PBoard::eraseOrbs()
{
    int combo = 0;

    // Collect all orbs that can be erased
    OrbSet orbs;

    for (int i = column - 1; i >= 0; i--)
    {
        for (int j = 0; j < row; j++)
        {
            auto curr = board[i][j];
            // Ignore empty orbs
            if (curr == pad::empty)
                continue;

            // vertical and horizontal orbs
            auto vhOrbs = findSameOrbsAround(i, j);
            // Here we need to loop throufh vhOrbs and check all orbs to see if there are orbs that can be erased
            auto it = vhOrbs.begin();
            while (it != vhOrbs.end())
            {
                // nextOrb is a pointer to the next pair
                auto nextOrb = nextSameOrbAround(&vhOrbs, *it);
                // Only search if there are new orbs
                if (nextOrb != NULL)
                {
                    auto newOrbs = findSameOrbsAround(*nextOrb);
                    vhOrbs.insert(newOrbs.begin(), newOrbs.end());
                    // Must check if there are new orbs or it will be an infinite loop
                    if (newOrbs.size() > 0)
                    {
                        // There are at least some orbs around it reset and continue
                        // TODO: ideally you want to start from new orbs but how to achieve it?
                        it = vhOrbs.begin();
                        continue;
                    }
                }
                delete nextOrb;
                it++;
            }

            // There should be orbs inside, check if the size is more than minEraseCondition (it was causing some issues)
            if ((int)vhOrbs.size() >= minEraseCondition)
            {
                for (auto const &xy : vhOrbs)
                {
                    ORB(board, xy) = pad::empty;
                }
                combo++;
            }
        }
    }

    return combo;
}

OrbSet PBoard::findSameOrbsAround(OrbLocation loc)
{
    return findSameOrbsAround(loc.first, loc.second);
}

OrbSet PBoard::findSameOrbsAround(int x, int y)
{
    auto curr = board[x][y];

    // Check vertically
    OrbSet vOrbs;
    // Add this orb first or another infinite loop
    vOrbs.insert(LOCATION(x, y));
    int up = x, down = x;
    int upOrb = 1, downOrb = 1;
    while (--up >= 0)
    {
        if (board[up][y] == curr)
        {
            vOrbs.insert(LOCATION(up, y));
            upOrb++;
        }
        else
        {
            // Break immediately if nothing matches
            break;
        }
    }
    while (++down < column)
    {
        if (board[down][y] == curr)
        {
            vOrbs.insert(LOCATION(down, y));
            downOrb++;
        }
        else
        {
            break;
        }
    }
    // Now, we only need to check if there are at least 3 (4 or 5) orbs vertically
    // Less than the condition, -1 to remove the duplicate (which is the current orb)
    if (upOrb + downOrb - 1 < minEraseCondition)
        vOrbs.clear();

    // Check horizontally
    OrbSet hOrbs;
    // Add it again just in case it was cleared
    hOrbs.insert(LOCATION(x, y));
    int left = y, right = y;
    int leftOrb = 1, rightOrb = 1;
    while (--left >= 0)
    {
        if (board[x][left] == curr)
        {
            hOrbs.insert(LOCATION(x, left));
            leftOrb++;
        }
        else
        {
            break;
        }
    }
    while (++right < row)
    {
        if (board[x][right] == curr)
        {
            hOrbs.insert(LOCATION(x, right));
            rightOrb++;
        }
        else
        {
            break;
        }
    }
    // Same as above
    if (leftOrb + rightOrb - 1 < minEraseCondition)
        hOrbs.clear();

    // Merge vertical and horizontal all to vertical
    vOrbs.insert(hOrbs.begin(), hOrbs.end());
    return vOrbs;
}

OrbLocation *PBoard::nextSameOrbAround(OrbSet *vhOrbs, OrbLocation loc)
{
    return nextSameOrbAround(vhOrbs, loc.first, loc.second);
}

OrbLocation *PBoard::nextSameOrbAround(OrbSet *vhOrbs, int x, int y)
{
    auto orb = board[x][y];

    // Find up, down, left and right
    auto pair = new OrbLocation;
    if (hasSameOrb(orb, x - 1, y))
    {
        *pair = LOCATION(x - 1, y);
        if (vhOrbs->count(*pair) == 0)
            return pair;
    }
    if (hasSameOrb(orb, x + 1, y))
    {
        *pair = LOCATION(x + 1, y);
        if (vhOrbs->count(*pair) == 0)
            return pair;
    }
    if (hasSameOrb(orb, x, y - 1))
    {
        *pair = LOCATION(x, y - 1);
        if (vhOrbs->count(*pair) == 0)
            return pair;
    }
    if (hasSameOrb(orb, x, y + 1))
    {
        *pair = LOCATION(x, y + 1);
        if (vhOrbs->count(*pair) == 0)
            return pair;
    }

    // Remember to release it
    delete pair;
    return NULL;
}

bool PBoard::hasSameOrb(Orb orb, OrbLocation loc)
{
    return hasSameOrb(orb, loc.first, loc.second);
}

bool PBoard::hasSameOrb(Orb orb, int x, int y)
{
    if (validLocation(x, y))
    {
        return board[x][y] == orb;
    }

    return false;
}

void PBoard::swapLocation(OrbLocation one, OrbLocation two)
{
    // TODO: all points should be valid why?
    if (!validLocation(one) || !validLocation(two))
        return;

    auto temp = ORB(board, one);
    ORB(board, one) = ORB(board, two);
    ORB(board, two) = temp;
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
    for (auto const &row : board)
    {
        for (auto const &orb : row)
        {
            std::cout << pad::ORB_NAMES[orb] << "\t";
        }
        std::cout << std::endl;
    }
    std::cout << std::endl;
}

void PBoard::printBoardForSimulation()
{
    std::stringstream ss;
    for (auto const &row : board)
    {
        for (auto const &orb : row)
        {
            std::cout << pad::ORB_SIMULATION_NAMES[orb];
            ss << (int)(orb - 1);
        }
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

int PBoard::estimatedBestScore()
{
    int score = 0;
    int maxCombo = getMaxCombo(collectOrbCount());
    // TODO: a naive implementation

    int maxBoardMovement = column - 2 + row - 3;
    score += maxCombo * pad::ONE_COMBO_SCORE;
    score += maxBoardMovement * pad::CASCADE_SCORE;
    // Take orb left into account
    score -= (row * column - maxCombo * minEraseCondition) * 50;
    return score;
}

/// Utils

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

std::string PBoard::getBoardID()
{
    // Get unique ID
    std::stringstream id;
    for (auto const &row : board)
    {
        for (auto const &orb : row)
        {
            // , is important because you have 10 which can be 1 0 or just 10
            id << (int)orb << ",";
        }
    }
    return id.str();
}

int PBoard::getBoardMaxCombo()
{
    return row * column / minEraseCondition;
}

bool PBoard::isEmptyFile()
{
    return column == 0 && row == 0;
}

int *PBoard::collectOrbCount()
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

bool PBoard::validLocation(OrbLocation loc)
{
    return validLocation(loc.first, loc.second);
}

bool PBoard::validLocation(int x, int y)
{
    if (x >= 0 && x < column && y >= 0 && y < row)
    {
        // You cannot move a sealed orb
        return board[x][y] != pad::seal;
    }

    return false;
}
