//
// timer.h
// Track time taken
//
// Created by Yiheng Quan on 11/11/2020
//

#ifndef _TIMER_H_
#define _TIMER_H_

#include <chrono>
#include <cstdio>
#include <string>

namespace pazusoba {
using namespace std;
using namespace std::chrono;

class timer {
private:
    /// Track the starting time
    time_point<high_resolution_clock> start;
    /// Used for know which timer only
    const string& name;

public:
    timer(const string& name) : name(name) {
        if (DEBUG_MODE)
            start = high_resolution_clock::now();
    }

    /// When the timer is out of the scope, this function will be called
    ~timer() {
        if (DEBUG_MODE) {
            duration<float> duration = high_resolution_clock::now() - start;
            auto second = duration.count();
            auto ms = second * 1000;
            printf("%s, %fs (%fms)\n", name.c_str(), second, ms);
        }
    }
};
}  // namespace pazusoba

#endif
