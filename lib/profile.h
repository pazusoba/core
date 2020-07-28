/**
 * profile.h
 * P&D has many teams and profiles are for different teams and play style
 * 
 * by Yiheng Quan
 */

#ifndef PROFILE_H
#define PROFILE_H

#include <vector>
#include <string>
#include "board.h"
#include "pad.h"

// The base of all profiles, it only requires a getScore function
class Profile
{
public:
    virtual ~Profile() = default;
    virtual std::string getProfileName() const;
    virtual int getScore(ComboList &list, Board &board, int moveCount) const;
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

    // Add a profile to current list
    void addProfile(Profile *p)
    {
        profiles.push_back(p);
    }

    // Clear all profiles and set it to a new one
    void updateProfile(std::vector<Profile *> &p)
    {
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
public:
    std::string getProfileName() const override
    {
        return "combo";
    }

    int getScore(ComboList &list, Board &board, int moveCount) const override
    {
        int score = 0;
        int combo = list.size();
        // Get column and row based on board size
        int column = board.size();
        int row = board[0].size();

        // Check if there are orbs next to each other
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

                        // Check orbs are the same
                        if (curr == board[i + a][j + b])
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
        return 0;
    }
};

// More points for more colour erased
class ColourProfile : public Profile
{
public:
    std::string getProfileName() const override
    {
        return "colour";
    }

    int getScore(ComboList &list, Board &board, int moveCount) const override
    {
        return 0;
    }
};

// More points if a combo has a certain shape
class ShapeProfile : public Profile
{
public:
    std::string getProfileName() const override
    {
        return "shape";
    }

    int getScore(ComboList &list, Board &board, int moveCount) const override
    {
        return 0;
    }
};

// More points if there are less orbs left
class OrbProfile : public Profile
{
public:
    std::string getProfileName() const override
    {
        return "orb remains";
    }

    int getScore(ComboList &list, Board &board, int moveCount) const override
    {
        return 0;
    }
};

#endif
