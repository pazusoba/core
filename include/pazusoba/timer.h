//
// timer.h
// Track time taken
//
// Created by Yiheng Quan on 11/11/2020
//

#ifndef _TIMER_H_
#define _TIMER_H_

#include <fmt/core.h>
#include <chrono>
#include <string>

namespace pazusoba {
using namespace std;
using namespace std::chrono;

class Timer {
private:
    /// Track the starting time
    time_point<high_resolution_clock> start;
    /// Used for know which timer only
    const string& name;

public:
    Timer(const string& name) : name(name) {
        start = high_resolution_clock::now();
    }

    /// When the timer is out of the scope, this function will be called
    ~Timer() { stop(); }

    void stop() {
        duration<double> duration = high_resolution_clock::now() - start;
        auto second = duration.count();
        auto ms = second * 1000;
        fmt::print("{}, {}s ({}ms)\n", name, second, ms);
    }
};
}  // namespace pazusoba

#endif
