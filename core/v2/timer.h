//
// timer.h
// Track time taken
//
// Created by Yiheng Quan on 11/11/2020
//

#ifndef TIMER_H
#define TIMER_H

#include <iostream>
#include <chrono>
#include <map>

namespace pazusoba
{
    using namespace std::chrono;

    // TODO: improve this maybe?
    class Timer
    {
    private:
        // This tracks all starting time
        std::map<int, system_clock::time_point> timers;
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
            timers[key] = system_clock::now();
        }

        // Check how long it has passed
        void end(int key)
        {
            auto end = system_clock::now();
            duration<double> t = end - timers[key];
            std::cout << "Key " << key << " - time: " << t.count() << "s\n";
        }
    };
} // namespace pazusoba

#endif
