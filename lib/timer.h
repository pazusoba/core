/**
 * timer.h
 * a simple timer singleton to track time
 * by Yiheng Quan
 */

#ifndef TIMER_H
#define TIMER_H

#include <iostream>
#include <chrono>
#include <map>

class Timer
{
private:
    // This tracks all starting time
    std::map<int, std::chrono::system_clock::time_point> timers;
    Timer() {}

public:
    // A shared instance
    static Timer &shared()
    {
        static Timer instance;
        return instance;
    }

    // Use int as a key
    void start(int key)
    {
        timers[key] = std::chrono::system_clock::now();
    }

    // Check how long it has passed
    void end(int key)
    {
        auto end = std::chrono::system_clock::now();
        std::chrono::duration<double> t = end - timers[key];
        std::cout << "Key " << key << " - time: " << t.count() << "s\n";
    }
};

#endif
