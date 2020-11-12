//
// timer.h
// Track time taken
//
// Created by Yiheng Quan on 11/11/2020
//

#ifndef _TIMER_H_
#define _TIMER_H_

#include <iostream>
#include <chrono>
#include <string>

namespace pazusoba
{
    using namespace std;
    using namespace std::chrono;

    class Timer
    {
    private:
        /// Track the starting time
        time_point<high_resolution_clock> start;
        /// Used for know which timer only
        string name;

    public:
        Timer(string name) : name(name)
        {
            start = high_resolution_clock::now();
        }

        /// When the timer is out of the scope, this function will be called
        ~Timer()
        {
            duration<double> duration = high_resolution_clock::now() - start;
            auto second = duration.count();
            auto ms = second * 1000;
            cout << name << " | " << second << "s (" << ms << "ms)" << endl;
        }
    };
} // namespace pazusoba

#endif
