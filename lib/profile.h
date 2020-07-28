/**
 * profile.h
 * P&D has many teams and profiles are for different teams and play style
 * 
 * by Yiheng Quan
 */

#ifndef PROFILE_H
#define PROFILE_H

#include <vector>

// The base of all profiles, it only requires a getScore function
class Profile
{
public:
    virtual int getScore();
    virtual ~Profile() {}
};

// This is a singleton class to update profiles at run time
class ProfileManager
{
    std::vector<Profile> profiles;
    ProfileManager() 
    {
        // By default it has only combo profile
    }
public:
    // Return a shared instance of profile manager
    static ProfileManager &shared()
    {
        static ProfileManager p;
        return p;
    }

    // Loop through all profiles and add scores based on every profile
    int getScore()
    {
        int score = 0;

        for (auto &p : profiles)
        {
            score += p.getScore();
        }

        return score;
    }

    // Add a profile to current list
    void addProfile(Profile &p)
    {
        profiles.push_back(p);
    }

    // Clear all profiles and set it to a new one
    void updateProfile(std::vector<Profile> &p)
    {
        profiles.clear();
        profiles = p;
    }
};


#endif
