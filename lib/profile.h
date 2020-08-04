/**
 * profile.h
 * P&D has many teams and profiles are for different teams and play style
 * 
 * by Yiheng Quan
 */

#ifndef PROFILE_H
#define PROFILE_H

#include <vector>
#include <set>
#include <map>
#include <string>
#include <iostream>
#include "board.h"
#include "pad.h"

// The base of all profiles, it only requires a getScore function
class Profile
{
public:
    virtual ~Profile() {}
    virtual std::string getProfileName() const = 0;
    virtual int getScore(const ComboList &list, const Board &board, int moveCount) const = 0;
};

// This is a singleton class to update profiles at run time
class ProfileManager
{
    std::vector<Profile *> profiles;
    ProfileManager() {}

public:
    // Return a shared instance of profile manager
    static ProfileManager &shared()
    {
        static ProfileManager p;
        return p;
    }

    // Loop through all profiles and add scores based on every profile
    // You need a list of combos, the current board and moveCount
    int getScore(ComboList &list, Board &board, int moveCount)
    {
        int score = 0;

        for (auto &p : profiles)
        {
            score += p->getScore(list, board, moveCount);
        }

        return score;
    }

    void clear()
    {
        // Delete all profiles
        for (const auto &p : profiles)
        {
            delete p;
        }
        profiles.clear();
    }

    // Add a profile to current list
    void addProfile(Profile *p)
    {
        profiles.push_back(p);
    }

    // Clear all profiles and set it to a new one
    void updateProfile(std::vector<Profile *> &p)
    {
        clear();
        for (const auto &p : profiles)
        {
            delete p;
        }
        profiles.clear();
        profiles = p;
    }
};

// More points for more combos
class ComboProfile : public Profile
{
    // If it is-1, always aim for more combo
    int targetCombo = -1;

public:
    // Just a default profile
    ComboProfile() {}
    // With a target combo
    ComboProfile(int combo) : targetCombo(combo) {}

    std::string getProfileName() const override
    {
        return "combo";
    }

    int getScore(const ComboList &list, const Board &board, int moveCount) const override
    {
        int score = 0;
        int combo = list.size();

        if (targetCombo <= 0)
        {
            // Get column and row based on board size
            int column = board.size();
            int row = board[0].size();
            // Check if there are orbs next to each other
            int orbAround = 0;
            int orbNext2 = 0;
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

                            int x = i + a, y = j + b;
                            // check x & y are valid
                            if (x >= 0 && x < column && y >= 0 && y < row)
                            {
                                // Check orbs are the same
                                auto orb = board[x][y];
                                if (curr == orb)
                                {
                                    orbAround++;
                                    if ((a == 0 && ((b == 1) || (b == -1))) ||
                                        (b == 0 && ((a == 1) || (a == -1))))
                                    {
                                        // This means that it is a line
                                        orbNext2 += 1;
                                        orbAround -= 1;
                                    }
                                }
                            }
                        }
                    }
                }
            }
            if (targetCombo < 0)
            {
                // Always aim for max combo
                score += pad::TIER_TWO_SCORE * orbAround;
                score += pad::TIER_THREE_SCORE * orbNext2;
                score += pad::TIER_FIVE_SCORE * moveCount;
                score += pad::TIER_EIGHT_SCORE * combo;
            }
            else
            {
                // Aim for zero combo and make sure orbs are not close to each other
                score -= pad::TIER_TWO_SCORE * orbAround;
                score -= pad::TIER_THREE_SCORE * orbNext2;
                score -= pad::TIER_FIVE_SCORE * moveCount;
                score -= pad::TIER_EIGHT_SCORE * combo;
            }
        }
        else
        {
            if (combo > targetCombo)
            {
                // Punish heavily for doing more combo that needed
                combo = (targetCombo - combo) * 5;
            }
            else if (combo == targetCombo)
            {
                // More points for doing exactly target combo
                score += pad::TIER_EIGHT_PLUS_SCORE;
            }

            // Usually, you don't have skyfall so don't need to bother with all thoses
            score += pad::TIER_FIVE_SCORE * moveCount;
            score += pad::TIER_EIGHT_SCORE * combo;
        }

        return score;
    }
};

// More points for more colour erased
class ColourProfile : public Profile
{
    // By default, all main colours
    std::vector<Orb> orbs{pad::fire, pad::water, pad::wood, pad::light, pad::dark, pad::recovery};

public:
    ColourProfile() {}
    ColourProfile(std::vector<Orb> o) : orbs(o) {}

    std::string getProfileName() const override
    {
        return "colour";
    }

    int getScore(const ComboList &list, const Board &board, int moveCount) const override
    {
        int score = 0;
        std::set<Orb> colours;
        for (const auto &c : list)
        {
            int x = c[0].first, y = c[0].second;
            colours.insert(board[x][y]);
        }

        // Check if colours matches
        score += colours.size() * pad::TIER_SIX_SCORE;
        return score;
    }
};

// More points if a combo has a certain shape
class ShapeProfile : public Profile
{
    // This determine which orb we are targetting
    // virtual std::vector<Orb> targetOrbs();
};

class TwoWayProfile : public ShapeProfile
{
public:
    std::string getProfileName() const override
    {
        return "2U";
    }

    int getScore(const ComboList &list, const Board &board, int moveCount) const override
    {
        int score = 0;
        for (const auto &c : list)
        {
            // 2U needs 4 orbs connected
            if (c.size() == 4)
                score += pad::TIER_SEVEN_SCORE;
        }
        return score;
    }
};

class LProfile : public ShapeProfile
{
public:
    std::string getProfileName() const override
    {
        return "L";
    }

    int getScore(const ComboList &list, const Board &board, int moveCount) const override
    {
        int score = 0;
        for (const auto &c : list)
        {
            // L shape must erase 5 orbs
            if (c.size() == 5)
            {
                std::map<int, int> vertical;
                std::map<int, int> horizontal;
                int bigFirst = -1;
                int bigSecond = -1;

                // Collect info
                for (const auto &loc : c)
                {
                    int x = loc.first;
                    int y = loc.second;
                    vertical[x]++;
                    horizontal[y]++;

                    // Track the largest number
                    if (vertical[x] >= 3)
                        bigFirst = x;
                    if (horizontal[y] >= 3)
                        bigSecond = y;
                }

                // This is the center point
                if (bigFirst > -1 && bigSecond > -1)
                {
                    int counter = 0;
                    // Check if bigFirst -2 or +2 exists
                    if (vertical[bigFirst - 2] > 0 || vertical[bigFirst + 2] > 0)
                        counter++;
                    // Same for bigSecond
                    if (horizontal[bigSecond - 2] > 0 || horizontal[bigSecond + 2] > 0)
                        counter++;

                    if (counter == 2)
                        score += pad::TIER_EIGHT_PLUS_SCORE;
                }
            }
        }
        return score;
    }
};

class PlusProfile : public ShapeProfile
{
public:
    std::string getProfileName() const override
    {
        return "+";
    }

    int getScore(const ComboList &list, const Board &board, int moveCount) const override
    {
        int score = 0;
        for (const auto &c : list)
        {
            // L shape must erase 5 orbs
            if (c.size() == 5)
            {
                std::map<int, int> vertical;
                std::map<int, int> horizontal;
                int bigFirst = -1;
                int bigSecond = -1;

                // Collect info
                for (const auto &loc : c)
                {
                    int x = loc.first;
                    int y = loc.second;
                    vertical[x]++;
                    horizontal[y]++;

                    // Track the largest number
                    if (vertical[x] >= 3)
                        bigFirst = x;
                    if (horizontal[y] >= 3)
                        bigSecond = y;
                }

                // This is the center point
                if (bigFirst > -1 && bigSecond > -1)
                {
                    int counter = 0;
                    // Check up down left right there is an orb around center orb
                    if (vertical[bigFirst - 1] > 0 && vertical[bigFirst + 1] > 0)
                        counter++;
                    if (horizontal[bigSecond - 1] > 0 && horizontal[bigSecond + 1] > 0)
                        counter++;

                    if (counter == 2)
                        score += pad::TIER_NINE_SCORE;
                }
            }
        }
        return score;
    }
};

// Void damage penetration 無効貫通
class VoidPenProfile : public ShapeProfile
{
public:
    std::string getProfileName() const override
    {
        return "void damage penetration";
    }

    int getScore(const ComboList &list, const Board &board, int moveCount) const override
    {
        // Try this board
        // DHLHHDHDDDHLDDHDLLLHHLHLLLLDHLHLDLHLLLHLHH
        int score = 0;
        for (const auto &c : list)
        {
            int size = c.size();
            // must be a 3x3 so 9 orbs
            if (size == 9)
            {
                // Do the same like + and L
                std::map<int, int> vertical;
                std::map<int, int> horizontal;

                // Collect info
                for (const auto &loc : c)
                {
                    int x = loc.first;
                    int y = loc.second;
                    vertical[x]++;
                    horizontal[y]++;
                }

                // All x and y are 3 because it is 3x3
                if (vertical.size() == 3 && horizontal.size() == 3)
                {
                    int count = 0;
                    for (auto curr = vertical.begin(); curr != vertical.end(); curr++)
                    {
                        if (curr->second == 3)
                            count++;
                    }
                    for (auto curr = horizontal.begin(); curr != horizontal.end(); curr++)
                    {
                        if (curr->second == 3)
                            count++;
                    }
                    if (count == 6)
                        score += pad::TIER_TEN_SCORE;
                }
            }
            else
            {
                // Punish if it is less than or more than 9 orbs
                score -= abs(9 - size) * pad::TIER_EIGHT_SCORE;
            }
        }
        return score;
    }
};


// Connect 10 - 12 orbs together and you get +1 combo and a green soybean
class SoybeanProfile : public ShapeProfile
{
public:
    std::string getProfileName() const override
    {
        return "soybean";
    }

    int getScore(const ComboList &list, const Board &board, int moveCount) const override
    {
        int score = 0;
        for (const auto &c : list)
        {
            int size = c.size();
            if (size >= 10 && size <= 12)
            {
                score += pad::TIER_TEN_SCORE;
            }
        }
        return score;
    }
};

class OneRowProfile : public ShapeProfile
{
public:
    std::string getProfileName() const override
    {
        return "row";
    }

    int getScore(const ComboList &list, const Board &board, int moveCount) const override
    {
        int score = 0;
        auto row = board[0].size();
        for (const auto &c : list)
        {
            // It has a row, xxxxxx or more
            if (c.size() >= row)
            {
                // Record all encountered x
                std::map<int, int> xs;
                for (const auto &loc : c)
                {
                    xs[loc.first]++;
                }

                // Sometimes, it is a long L so just check which x has more than row times
                for (auto curr = xs.begin(); curr != xs.end(); curr++)
                {
                    if (curr->second >= (int)row)
                    {
                        score += pad::TIER_NINE_SCORE;
                        break;
                    }
                }
            }
        }
        return score;
    }
};

// More points if there are less orbs left
class OrbProfile : public Profile
{
    int targetNumber = -1;

public:
    OrbProfile() {}
    OrbProfile(int count) : targetNumber(count) {}

    std::string getProfileName() const override
    {
        return "orb remains";
    }

    int getScore(const ComboList &list, const Board &board, int moveCount) const override
    {
        // Get column and row based on board size
        int column = board.size();
        int row = board[0].size();
        int boardSize = row * column;
        // -1 means no target so why use this?
        if (targetNumber < 0 || targetNumber > boardSize)
            return 0;

        int score = 0;
        int orbRemain = 0;
        // Score how many orbs are left for each type
        std::map<Orb, int> allOrbs;
        for (const auto &c : board)
        {
            for (const auto &r : c)
            {
                if (r != pad::empty)
                {
                    allOrbs[r]++;
                    orbRemain++;
                }
            }
        }

        // Get the distance from the goal
        auto distance = orbRemain - targetNumber;
        // Do this only if we haven't reached the goal
        if (distance > 0)
        {
            // Better to connect more orbs per type
            for (const auto &c : list)
            {
                // Since that orb was erased so we just need to punish for not erasing more of it
                auto orb = c[0].orb;
                score -= (allOrbs[orb] * pad::TIER_SEVEN_SCORE);
            }
            // As long it is less than target number, it is good
            score -= distance * pad::TIER_NINE_SCORE;
        }
        return score;
    }
};

#endif
