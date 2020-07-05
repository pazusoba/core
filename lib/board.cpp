/**
 * board.cpp
 * by Yiheng Quan
 */

#include <iostream>
#include "board.h"

/// Constructors

PadBoard::PadBoard(std::string filePath) {

}

PadBoard::PadBoard(std::string filePath, int minEraseCondition) {

}

PadBoard::~PadBoard() {

}

/// Board related

void PadBoard::readBoard(std::string filePath)
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
        std::vector<Orb> boardRow;
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
            boardRow.push_back(Orb(a));
        }

        // Add this row to the board
        board.push_back(boardRow);

        column++;
    }

    boardFile.close();
}

int PadBoard::rateBoard(Board *board)
{
    int score = 0;

    // Loop through each orbs and see if there similar orbs around it
    for (int i = 0; i < column; i++)
    {
        for (int j = 0; j < row; j++)
        {
            auto currOrb = (*board)[i][j];
            // Check for same colour around in a 3x3 grid, curr orb is in the middle
            int orbAround = 0;
            int twoInLine = 0;
            for (int x = -1; x <= 1; x++)
            {
                for (int y = -1; y <= 1; y++)
                {
                    if (hasSameOrb(board, currOrb, i + x, j + y))
                    {
                        orbAround++;
                        if ((x == 0 && ((y == 1) || (y == -1))) ||
                            (y == 0 && ((x == 1) || (x == -1))))
                        {
                            twoInLine += 1;
                        }
                    }
                }
            }
            // Having same colour orb nearby
            score += pad::ORB_NEARBY_SCORE * orbAround;
            // Two orbs in a line, easier to make combo
            score += pad::ORB_AROUND_SCORE * twoInLine;
        }
    }

    // total combo
    int combo = 0;
    // I encourage cascade even if it does less combo
    int moveCount = 0;
    int newCombo = eraseOrbs(board);
    while (newCombo > 0)
    {
        combo += newCombo;
        if (printMoreMessages)
            printBoard(board);
        moveOrbsDown(board);
        newCombo = eraseOrbs(board);
        moveCount++;
    }

    // Here is a simple calculation, moveCount should -1 because the first movement doesn't count
    score += pad::ONE_COMBO_SCORE * (combo + moveCount);

    std::cout << "That was " << combo << " combo\n";
    return score;
}


void PadBoard::moveOrbsDown(Board *board)
{
    // we start from the second last row -1 and also convert to index so -2
    // i can be 0 for the first row or the first row won't be updated
    for (int i = column - 2; i >= 0; i--)
    {
        for (int j = 0; j < row; j++)
        {
            // Keep checking if bottom orb is empty until it is not or out of bound
            int k = 1;
            while (i + k < column && pad::empty == (*board)[i + k][j])
            {
                // k - 1 because you want to compare with the orb right below
                // k is increasing so k - 1 is the orb below (if you don't do it, nothing will be updated)
                (*board)[i + k][j] = (*board)[i + k - 1][j];
                (*board)[i + k - 1][j] = pad::empty;
                k += 1;
            }
        }
    }
    if (printMoreMessages)
        std::cout << "Board has been updated\n";
}

int PadBoard::eraseOrbs(Board *board)
{
    int combo = 0;

    // Collect all orbs that can be erased
    OrbSet orbs;

    for (int i = column - 1; i >= 0; i--)
    {
        for (int j = 0; j < row; j++)
        {
            auto curr = (*board)[i][j];
            // Ignore empty orbs
            if (curr == pad::empty)
                continue;

            // vertical and horizontal orbs
            auto vhOrbs = findSameOrbsAround(board, i, j);
            // Here we need to loop throufh vhOrbs and check all orbs to see if there are orbs that can be erased
            auto it = vhOrbs.begin();
            while (it != vhOrbs.end())
            {
                // nextOrb is a pointer to the next pair
                auto nextOrb = nextSameOrbAround(board, it->first, it->second, &vhOrbs);
                // Only search if there are new orbs
                if (nextOrb != NULL)
                {
                    auto newOrbs = findSameOrbsAround(board, nextOrb->first, nextOrb->second);
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
                for (auto xy : vhOrbs)
                {
                    (*board)[xy.first][xy.second] = pad::empty;
                }
                combo++;
            }
        }
    }

    return combo;
}

PadBoard::OrbSet PadBoard::findSameOrbsAround(Board *board, int x, int y)
{
    auto curr = (*board)[x][y];

    // Check vertically
    OrbSet vOrbs;
    // Add this orb first or another infinite loop
    vOrbs.insert(PAIR(x, y));
    int up = x, down = x;
    int upOrb = 1, downOrb = 1;
    while (--up >= 0)
    {
        if ((*board)[up][y] == curr)
        {
            vOrbs.insert(PAIR(up, y));
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
        if ((*board)[down][y] == curr)
        {
            vOrbs.insert(PAIR(down, y));
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
    hOrbs.insert(PAIR(x, y));
    int left = y, right = y;
    int leftOrb = 1, rightOrb = 1;
    while (--left >= 0)
    {
        if ((*board)[x][left] == curr)
        {
            hOrbs.insert(PAIR(x, left));
            leftOrb++;
        }
        else
        {
            break;
        }
    }
    while (++right < row)
    {
        if ((*board)[x][right] == curr)
        {
            hOrbs.insert(PAIR(x, right));
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

OrbLocation *PadBoard::nextSameOrbAround(Board *board, OrbSet *vhOrbs, int x, int y)
{
    auto orb = (*board)[x][y];

    // Find up, down, left and right
    auto pair = new OrbLocation;
    if (hasSameOrb(board, x - 1, y, orb))
    {
        *pair = PAIR(x - 1, y);
        if (vhOrbs->count(*pair) == 0)
            return pair;
    }
    if (hasSameOrb(board, x + 1, y, orb))
    {
        *pair = PAIR(x + 1, y);
        if (vhOrbs->count(*pair) == 0)
            return pair;
    }
    if (hasSameOrb(board, x, y - 1, orb))
    {
        *pair = PAIR(x, y - 1);
        if (vhOrbs->count(*pair) == 0)
            return pair;
    }
    if (hasSameOrb(board, x, y + 1, orb))
    {
        *pair = PAIR(x, y + 1);
        if (vhOrbs->count(*pair) == 0)
            return pair;
    }

    // Remember to release it
    delete pair;
    return NULL;
}

bool PadBoard::hasSameOrb(Board *board, int x, int y, Orb orb)
{
    if (x >= 0 && x < column && y >= 0 && y < row)
    {
        return (*board)[x][y] == orb;
    }

    return false;
}

void PadBoard::swapOrbs(Orb *first, Orb *second)
{
    auto temp = *first;
    *first = *second;
    *second = temp;
}

void PadBoard::printBoard(Board *board)
{
    if (isEmptyFile())
    {
        std::cout << "- empty -\n";
        return;
    }

    // Print everything out nicely
    std::cout << std::endl;
    std::cout << row << " x " << column << std::endl;
    for (auto row : (*board))
    {
        for (auto orb : row)
        {
            std::cout << pad::ORB_NAMES[orb] << "\t";
        }
        std::cout << std::endl;
    }
    std::cout << std::endl;
}

void PadBoard::printBoardInfo(Board *board)
{
    if (isEmptyFile())
    {
        std::cout << "no info\n";
        return;
    }

    // Collect orb info
    int *counter = collectOrbCount(board);

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

int PadBoard::getMaxCombo(int *counter)
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

int PadBoard::getBoardMaxCombo()
{
    return row * column / minEraseCondition;
}

bool PadBoard::isEmptyFile()
{
    return column == 0 && row == 0;
}

int *PadBoard::collectOrbCount(Board *board)
{
    int *counter = new int[pad::ORB_COUNT]{0};
    for (auto row : (*board))
    {
        for (auto orb : row)
        {
            counter[orb]++;
        }
    }
    return counter;
}

/// Utils

