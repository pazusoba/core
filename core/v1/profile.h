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
#include <cmath>
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
    virtual int getScore(const ComboList &list, const Board &board, int moveCount, int minEraseCondition) const = 0;
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
    int getScore(ComboList &list, Board &board, int moveCount, int minEraseCondition)
    {
        int score = 0;

        for (auto &p : profiles)
        {
            score += p->getScore(list, board, moveCount, minEraseCondition);
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

    int getScore(const ComboList &list, const Board &board, int moveCount, int minEraseCondition) const override
    {
        int score = 0;
        int combo = list.size();

        // Get column and row based on board size
        int column = board.size();
        int row = board[0].size();
        // Check if there are orbs next to each other
        int orbAround = 0;
        int orbNext2 = 0;

        // Collect all orbs with current location
        // std::map<Orb, std::vector<OrbLocation>> distanceInfo;

        for (int i = 0; i < column; i++)
        {
            for (int j = 0; j < row; j++)
            {
                auto curr = board[i][j];
                if (curr == pad::empty)
                    continue;

                // TODO: improve this??
                // save this location
                // distanceInfo[curr].push_back(LOCATION(i, j));

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

        // For every orbs, we need to get the distance of it from other orbs
        // for (auto curr = distanceInfo.begin(); curr != distanceInfo.end(); curr++)
        // {
        //     // track the total distance
        //     int distance = 0;
        //     auto orbs = curr->second;
        //     int size = orbs.size();
        //     for (int i = 0; i < size; i++)
        //     {
        //         auto loc = orbs[i];
        //         for (int j = i; j < size; j++)
        //         {
        //             auto other = orbs[j];
        //             distance += abs(loc.first - other.first) + abs(loc.second - other.second);
        //             // distance += (int)sqrt(pow(loc.first - other.first, 2) + pow(loc.second - other.second, 2));
        //         }
        //     }

        //     // Less points if far away
        //     score -= pad::TIER_THREE_SCORE * distance;
        // }

        if (targetCombo == 0)
        {
            // Aim for zero combo and make sure orbs are not close to each other
            score -= pad::TIER_TWO_SCORE * orbAround;
            score -= pad::TIER_THREE_SCORE * orbNext2;
            score -= pad::TIER_FIVE_SCORE * moveCount;
            score -= pad::TIER_EIGHT_SCORE * combo;
        }
        else
        {
            if (targetCombo > 0)
            {
                int distance = combo - targetCombo;
                if (distance > 0)
                {
                    // Sometimes, it is ok to do more combo temporarily
                    combo *= -1;
                }
            }
            else
            {
                // Encourage cascading
                score += pad::TIER_FIVE_SCORE * moveCount;
            }

            // Always aim for max combo by default
            score += pad::TIER_ONE_SCORE * orbAround;
            score += pad::TIER_ONE_SCORE * orbNext2;
            score += pad::TIER_EIGHT_SCORE * combo;
        }

        return score;
    }
};

// More points for more colour erased
class ColourProfile : public Profile
{
    // By default, all main colours
    std::vector<Orb> orbs;

public:
    ColourProfile() {}
    ColourProfile(std::vector<Orb> o) : orbs(o) {}

    std::string getProfileName() const override
    {
        return "colour";
    }

    int getScore(const ComboList &list, const Board &board, int moveCount, int minEraseCondition) const override
    {
        int score = 0;
        std::set<Orb> colours;
        for (const auto &c : list)
        {
            auto orb = c[0].orb;
            if (orbs.size() == 0)
            {
                colours.insert(orb);
            }
            else
            {
                for (const auto &o : orbs)
                {
                    if (orb == o)
                    {
                        // Only insert if matches
                        colours.insert(orb);
                        break;
                    }
                }
            }
        }

        // Check if colours matches
        score += colours.size() * pad::TIER_SEVEN_SCORE;
        return score;
    }
};

// More points if a combo has a certain shape
class ShapeProfile : public Profile
{
    std::vector<Orb> orbs;

public:
    ShapeProfile() {}
    ShapeProfile(std::vector<Orb> o) : orbs(o) {}

    // Check if orb list contains this orb
    virtual bool isTheOrb(Orb orb) const
    {
        if (orbs.size() == 0)
            return true;

        for (const auto &o : orbs)
        {
            if (orb == o)
            {
                return true;
            }
        }

        return false;
    }
};

class TwoWayProfile : public ShapeProfile
{
public:
    TwoWayProfile() : ShapeProfile() {}
    TwoWayProfile(std::vector<Orb> orbs) : ShapeProfile(orbs) {}

    std::string getProfileName() const override
    {
        return "2U";
    }

    int getScore(const ComboList &list, const Board &board, int moveCount, int minEraseCondition) const override
    {
        int score = 0;
        for (const auto &c : list)
        {
            // recovery is not two way
            if (c[0].orb == pad::recovery)
                continue;

            // 2U needs 4 orbs connected
            if (c.size() == 4 && isTheOrb(c[0].orb))
            {
                score += pad::TIER_SEVEN_SCORE;
            }
        }
        return score;
    }
};

class LProfile : public ShapeProfile
{
public:
    LProfile() : ShapeProfile() {}
    LProfile(std::vector<Orb> orbs) : ShapeProfile(orbs) {}

    std::string getProfileName() const override
    {
        return "L";
    }

    int getScore(const ComboList &list, const Board &board, int moveCount, int minEraseCondition) const override
    {
        int score = 0;
        for (const auto &c : list)
        {
            // L shape must erase 5 orbs and right colour
            if (c.size() == 5 && isTheOrb(c[0].orb))
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
    PlusProfile() : ShapeProfile() {}
    PlusProfile(std::vector<Orb> orbs) : ShapeProfile(orbs) {}

    std::string getProfileName() const override
    {
        return "+";
    }

    int getScore(const ComboList &list, const Board &board, int moveCount, int minEraseCondition) const override
    {
        int score = 0;
        for (const auto &c : list)
        {
            // L shape must erase 5 orbs
            if (c.size() == 5 && isTheOrb(c[0].orb))
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
    VoidPenProfile() : ShapeProfile() {}
    VoidPenProfile(std::vector<Orb> orbs) : ShapeProfile(orbs) {}

    std::string getProfileName() const override
    {
        return "void damage penetration";
    }

    int getScore(const ComboList &list, const Board &board, int moveCount, int minEraseCondition) const override
    {
        // Try this board
        // DHLHHDHDDDHLDDHDLLLHHLHLLLLDHLHLDLHLLLHLHH
        int score = 0;
        for (const auto &c : list)
        {
            auto orb = c[0].orb;
            // More points if it is a 3x3 shape
            if (isTheOrb(orb))
            {
                int size = c.size();
                int distance = size - 9;

                if (size == minEraseCondition)
                {
                    score -= pad::TIER_NINE_SCORE;
                }

                if (size <= 9)
                {
                    // Must connect more than min erase condition
                    score += size * pad::TIER_SEVEN_SCORE;

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

                    int v = vertical.size();
                    int h = horizontal.size();
                    if (v < 4 && h < 4)
                        score += (v + h) * pad::TIER_SEVEN_SCORE;

                    // All x and y are 3 because it is 3x3
                    if (v == 3 && h == 3)
                    {
                        int count = 0;
                        for (auto curr = vertical.begin(); curr != vertical.end(); curr++)
                        {
                            auto value = curr->second;
                            if (value == 3)
                                count++;
                        }
                        for (auto curr = horizontal.begin(); curr != horizontal.end(); curr++)
                        {
                            auto value = curr->second;
                            if (value == 3)
                                count++;
                        }

                        if (count < 6)
                            score += count * pad::TIER_EIGHT_PLUS_SCORE;
                        if (count == 6)
                            score += count * pad::TIER_TEN_SCORE;
                    }
                }
                else
                {
                    score -= distance * pad::TIER_NINE_SCORE;
                }
            }
        }
        return score;
    }
};

// Connect 10 - 12 orbs together and you get +1 combo and a green soybean
class SoybeanProfile : public ShapeProfile
{
public:
    SoybeanProfile() : ShapeProfile() {}
    SoybeanProfile(std::vector<Orb> orbs) : ShapeProfile(orbs) {}

    std::string getProfileName() const override
    {
        return "soybean";
    }

    int getScore(const ComboList &list, const Board &board, int moveCount, int minEraseCondition) const override
    {
        int score = 0;
        for (const auto &c : list)
        {
            int size = c.size();
            if (size >= 10 && size <= 12 && isTheOrb(c[0].orb))
            {
                score += pad::TIER_NINE_SCORE;
            }
        }
        return score;
    }
};

class OneRowProfile : public ShapeProfile
{
public:
    OneRowProfile() : ShapeProfile() {}
    OneRowProfile(std::vector<Orb> orbs) : ShapeProfile(orbs) {}

    std::string getProfileName() const override
    {
        return "row";
    }

    int getScore(const ComboList &list, const Board &board, int moveCount, int minEraseCondition) const override
    {
        int score = 0;
        int row = board[0].size();
        for (const auto &c : list)
        {
            int size = c.size();
            // It has a row, xxxxxx or more
            if (size == row && isTheOrb(c[0].orb))
            {
                // Record all encountered x
                std::map<int, int> xs;
                for (const auto &loc : c)
                {
                    // Track x because it should have the number of row
                    xs[loc.first]++;
                }

                // Sometimes, it is a long L so just check which x has more than row times
                for (auto curr = xs.begin(); curr != xs.end(); curr++)
                {
                    if (curr->second == row)
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

class OneColumnProfile : public ShapeProfile
{
public:
    OneColumnProfile() : ShapeProfile() {}
    OneColumnProfile(std::vector<Orb> orbs) : ShapeProfile(orbs) {}

    std::string getProfileName() const override
    {
        return "column";
    }

    int getScore(const ComboList &list, const Board &board, int moveCount, int minEraseCondition) const override
    {
        int score = 0;
        int column = board[0].size();
        for (const auto &c : list)
        {
            int size = c.size();
            // It is a column
            if (size == column && isTheOrb(c[0].orb))
            {
                // Record all encountered x
                std::map<int, int> xs;
                for (const auto &loc : c)
                {
                    // Track x because it should have the number of row
                    xs[loc.second]++;
                }

                // Sometimes, it is a long L so just check which x has more than row times
                for (auto curr = xs.begin(); curr != xs.end(); curr++)
                {
                    if (curr->second == column)
                    {
                        score += pad::TIER_EIGHT_PLUS_SCORE;
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
    // Aim for nothing less
    int targetNumber = 0;

public:
    OrbProfile() {}
    OrbProfile(int count) : targetNumber(count) {}

    std::string getProfileName() const override
    {
        return "orb remains";
    }

    int getScore(const ComboList &list, const Board &board, int moveCount, int minEraseCondition) const override
    {
        int score = 0;
        if (list.size() == 0)
            return score;

        int orbCount = board.size() * board[0].size();

        // Track how many orbs are erased and left
        int orbErased = 0;
        int orbLeft = 0;

        // Encourage to connect more orbs in a combo
        for (const auto &c : list)
        {
            int size = c.size();
            score += (size - minEraseCondition) * pad::TIER_FIVE_SCORE;
            orbErased += size;
        }

        orbLeft = orbCount - orbErased;
        // More points for erasing more orbs
        score -= pad::TIER_SIX_SCORE * orbLeft;
        if (orbLeft <= targetNumber)
        {
            score += pad::TIER_NINE_SCORE;
        }

        return score;
    }
};

#endif
