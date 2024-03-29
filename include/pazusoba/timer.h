//
// timer.h
// Track time with in the current scrope.
//
// Created by Yiheng Quan on 11/11/2020
//

#pragma once
#ifndef _TIMER_H_
#define _TIMER_H_

#include <chrono>
#include <cstdio>
#include <string>

namespace pazusoba {
using namespace std;
using namespace std::chrono;

class Timer {
private:
    /// Track the starting time
    time_point<high_resolution_clock> start;
    /// Used for know which timer only
    /// Don't do reference as it gets optimised in release
    string name;
    bool running = true;

    void stopTimer() {
        if (running) {
            running = false;
            duration<float> duration = high_resolution_clock::now() - start;
            auto second = duration.count();
            auto ms = second * 1000;
            printf("%s, %fs (%fms)\n", name.c_str(), second, ms);
        }
    }

public:
    Timer(const string& name) : name(name) {
        start = high_resolution_clock::now();
    }

    /// When the timer is out of the scope, this function will be called
    ~Timer() { stopTimer(); }

    /// Stop manually and won't start again
    void end() { stopTimer(); }
};
}  // namespace pazusoba

#endif
