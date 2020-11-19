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

PBoard::~PBoard()
{
    for (auto &row : board)
    {
        row.clear();
    }
    board.clear();
}

// MARK: - Board related
ComboList PBoard::eraseComboAndMoveOrbs(int *moveCount)
{
    ComboList combo;
    
    bool moreCombo;
    do
    {
        // Remember to reset
        moreCombo = false;
        for (int i = 0; i < column; i++)
        {
            for (int j = 0; j < row; j++)
            {
                // Ignore empty orbs
                if (board[i][j] == pad::empty)
                    continue;
                
                // Start finding combos
                auto erased = eraseCombo(&combo, i, j);
                if (erased)
                {
                    // Just to prevent setting it to false again
                    moreCombo = true;
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
    
    return combo;
}

void PBoard::floodfill(Combo *list, int x, int y, Orb orb)
{
    if (!validLocation(x, y))
        return;
    
    auto currOrb = board[x][y];
    // must be the same orb
    if (currOrb != orb)
        return;
    
    int count = 0;
    // all 4 directions
    for (int d = 0; d < 4; d++)
    {
        // 1 -> left
        // 2 -> right
        // 3 -> up
        // 4 -> down
        count = 0;
        
        int loop = row;
        if (d > 1)
            loop = column;
        
        for (int i = 0; i < loop; i++) {
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
            
            if (!validLocation(cx, cy))
                break;
            
            // stop if it doesn't match
            if (board[cx][cy] != orb)
                break;
            count++;
        }
        
        if (count >= minEraseCondition)
        {
            for (int i = 0; i < count; i++) {
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
                
                board[cx][cy] = pad::empty;
                list->emplace_back(cx, cy, board[cx][cy]);
            }
            
            for (int i = 0; i < count; i++) {
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
                floodfill(list, cx + 1, cy, orb);
                floodfill(list, cx - 1, cy, orb);
                floodfill(list, cx, cy + 1, orb);
                floodfill(list, cx, cy - 1, orb);
            }
        }
    }
}

bool PBoard::eraseCombo(ComboList *list, int ox, int oy)
{
    Combo combo;
    
    auto orb = board[ox][oy];
    floodfill(&combo, ox, oy, orb);
    
    if (combo.size() >= minEraseCondition)
    {
        list -> push_back(combo);
        return true;
    }
    
    return false;
}

int PBoard::rateBoard()
{
    int moveCount = 0;
    auto list = eraseComboAndMoveOrbs(&moveCount);
    int score = ProfileManager::shared().getScore(list, board, moveCount, minEraseCondition);
    
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
            auto orb = board[i][j];
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
                if (k >= s)
                {
                    board[i][j] = pad::empty;
                }
                else
                {
                    // If column is 5, i starts from 4 so the index of orb is 5 - 1 - 4 = 0
                    auto orb = orbs[k];
                    auto currOrb = board[i][j];
                    board[i][j] = orb;
                    
                    // Board is changed if currOrb is not actually orb
                    if (currOrb != orb)
                        changed = true;
                }
            }
        }
    }
    return changed;
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
            // If it only has one, this loop is being ignored
            do
            {
                // nextOrb is a pointer to the next pair
                auto nextOrb = nextSameOrbAround(&vhOrbs, *it);
                // Only search if there are new orbs, if nothing has been found (-1, -1) is returned
                if (nextOrb.first > -1)
                {
                    auto newOrbs = findSameOrbsAround(nextOrb);
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
                it++;
            } while (it != vhOrbs.end());
            
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

OrbLocation PBoard::nextSameOrbAround(OrbSet *vhOrbs, int x, int y)
{
    auto orb = board[x][y];
    
    // Find up, down, left and right
    if (hasSameOrb(orb, x - 1, y))
    {
        OrbLocation pair = LOCATION(x - 1, y);
        if (vhOrbs->count(pair) == 0)
            return pair;
    }
    if (hasSameOrb(orb, x + 1, y))
    {
        OrbLocation pair = LOCATION(x + 1, y);
        if (vhOrbs->count(pair) == 0)
            return pair;
    }
    if (hasSameOrb(orb, x, y - 1))
    {
        OrbLocation pair = LOCATION(x, y - 1);
        if (vhOrbs->count(pair) == 0)
            return pair;
    }
    if (hasSameOrb(orb, x, y + 1))
    {
        OrbLocation pair = LOCATION(x, y + 1);
        if (vhOrbs->count(pair) == 0)
            return pair;
    }
    
    // Return a default value
    return LOCATION(-1, -1);
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
