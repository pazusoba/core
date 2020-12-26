/**
 * configuration.h
 * Once a board is read, some values won't change.
 * Configuration is just for this purpose.
 * 
 * by Yiheng Quan
 */

#ifndef CONFIGURATION_H
#define CONFIGURATION_H

class Configuration
{
    Configuration() {}

public:
    // Return a shared instance of profile manager
    static Configuration &shared()
    {
        static Configuration p;
        return p;
    }

};

#endif
