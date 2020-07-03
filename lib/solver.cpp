/**
 * solver.cpp
 * by Yiheng Quan
 */

#include <iostream>
#include <fstream>
#include <sstream>
#include "solver.h"

/// Constrcutors

PadSolver::PadSolver(std::string filePath)
{
    readBoard(filePath);
}

PadSolver::PadSolver(std::string filePath, int minEraseCondition) : PadSolver(filePath)
{
    this->minEraseCondition = minEraseCondition;
}

PadSolver::~PadSolver()
{
    for (auto row : board)
    {
        // Clean all rows inside
        row.clear();
    }
    // Clean the board
    board.clear();
}

/// Board related

void PadSolver::readBoard(std::string filePath)
{
    std::string lines;

    std::ifstream boardFile(filePath);
    while (getline(boardFile, lines))
    {
        // Ignore lines that start with `//`
        if (lines.find("//") == 0)
            continue;

        // Remove trailing spaces by substr, +1 for substr (to include the char before space)
        int index = lines.find_last_not_of(" ") + 1;
        lines = lines.substr(0, index);

        // This is for storing this new row
        std::vector<pad::orbs> boardRow;
        // Keep reading until error, it will get rid of spaces automatically
        std::stringstream ss(lines);
        while (ss.good())
        {
            // Only add one to row if we are in the first column,
            // the size is fixed so there won't be a row with a different number of orbs
            if (column == 0)
                row++;
            // Read it out as a number
            int a = 0;
            ss >> a;

            // Convert int into orbs
            boardRow.push_back(pad::orbs(a));
        }

        // Add this row to the board
        board.push_back(boardRow);

        column++;
    }

    boardFile.close();
}

void PadSolver::solveBoard()
{
    // TODO: update solve board
    // Erase orbs and move the board down
    int combo = 0;
    int newCombo = eraseOrbs();
    while (newCombo > 0) {
        combo += newCombo;
        // printBoard();
        moveOrbsDown();
        newCombo = eraseOrbs();
    }

    std::cout << "That was " << combo << " combo\n";
}

void PadSolver::moveOrbsDown()
{
    // we start from the second last row -1 and also convert to index so -2
    // i can be 0 for the first row or the first row won't be updated
    for (int i = column - 2; i >= 0; i--)
    {
        for (int j = 0; j < row; j++)
        {
            // Keep checking if bottom orb is empty until it is not or out of bound
            int k = 1;
            while (i + k < column && pad::empty == board[i + k][j])
            {
                // k - 1 because you want to compare with the orb right below
                // k is increasing so k - 1 is the orb below (if you don't do it, nothing will be updated)
                board[i + k][j] = board[i + k - 1][j];
                board[i + k - 1][j] = pad::empty;
                k += 1;
            }
        }
    }
    std::cout << "Board has been updated\n";
}

int PadSolver::eraseOrbs()
{
    int combo = 0;

    // Collect all orbs that can be erased
    std::set<std::pair<int, int>> orbs;

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
                auto nextOrb = nextSameOrbAround(it->first, it->second, &vhOrbs);
                // Only search if there are new orbs
                if (nextOrb != NULL)
                {
                    auto newOrbs = findSameOrbsAround(nextOrb->first, nextOrb->second);
                    vhOrbs.insert(newOrbs.begin(), newOrbs.end());
                }
                delete nextOrb;
                it++;
            }

            // There should be orbs inside, check if the size is more than minEraseCondition (it was causing some issues)
            if ((int)vhOrbs.size() >= minEraseCondition)
            {
                for (auto xy : vhOrbs)
                {
                    board[xy.first][xy.second] = pad::empty;
                }
                combo++;
            }
        }
    }

    return combo;
}

std::set<std::pair<int, int>> PadSolver::findSameOrbsAround(int x, int y)
{
    auto curr = board[x][y];

    // Check vertically
    std::set<std::pair<int, int>> vOrbs;
    int up = x, down = x;
    int upOrb = 1, downOrb = 1;
    while (--up >= 0)
    {
        if (board[up][y] == curr)
        {
            vOrbs.insert(std::make_pair(up, y));
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
            vOrbs.insert(std::make_pair(down, y));
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
    std::set<std::pair<int, int>> hOrbs;
    int left = y, right = y;
    int leftOrb = 1, rightOrb = 1;
    while (--left >= 0)
    {
        if (board[x][left] == curr)
        {
            hOrbs.insert(std::make_pair(x, left));
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
            hOrbs.insert(std::make_pair(x, right));
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

std::pair<int, int> *PadSolver::nextSameOrbAround(int x, int y, std::set<std::pair<int, int>> *vhOrbs)
{
    auto orb = board[x][y];

    // Find up, down, left and right
    auto pair = new std::pair<int, int>;
    if (hasSameOrb(x - 1, y, orb))
    {
        *pair = std::make_pair(x - 1, y);
        if (vhOrbs->find(*pair) != vhOrbs->end())
            return pair;
    }
    if (hasSameOrb(x + 1, y, orb))
    {
        *pair = std::make_pair(x + 1, y);
        if (vhOrbs->find(*pair) != vhOrbs->end())
            return pair;
    }
    if (hasSameOrb(x, y - 1, orb))
    {
        *pair = std::make_pair(x, y - 1);
        if (vhOrbs->find(*pair) != vhOrbs->end())
            return pair;
    }
    if (hasSameOrb(x, y + 1, orb))
    {
        *pair = std::make_pair(x, y + 1);
        if (vhOrbs->find(*pair) != vhOrbs->end())
            return pair;
    }

    return NULL;
}

bool PadSolver::hasSameOrb(int x, int y, pad::orbs orb)
{
    if (x >= 0 && x < column && y >= 0 && y < row)
    {
        return board[x][y] == orb;
    }

    return false;
}

void PadSolver::swapOrbs(pad::orbs *first, pad::orbs *second)
{
    auto temp = *first;
    *first = *second;
    *second = temp;
}

void PadSolver::printBoard()
{
    if (isEmptyFile())
    {
        std::cout << "- empty -\n";
        return;
    }

    // Print everything out nicely
    std::cout << row << " x " << column << std::endl;
    for (auto row : board)
    {
        for (auto orb : row)
        {
            std::cout << pad::ORB_NAMES[orb] << "\t";
        }
        std::cout << std::endl;
    }
    std::cout << std::endl;
}

void PadSolver::printBoardInfo()
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

/// Utils

int PadSolver::getMaxCombo(int *counter)
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

int PadSolver::getBoardMaxCombo()
{
    return row * column / minEraseCondition;
}

bool PadSolver::isEmptyFile()
{
    return column == 0 && row == 0;
}

int *PadSolver::collectOrbCount()
{
    int *counter = new int[pad::ORB_COUNT]{0};
    for (auto row : board)
    {
        for (auto orb : row)
        {
            counter[orb]++;
        }
    }
    return counter;
}
