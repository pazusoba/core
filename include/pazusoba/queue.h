//
// queue.h
// Priority Queue for Beam Search
//
// Created by Yiheng Quan on 22/08/2021
//

#ifndef _QUEUE_H_
#define _QUEUE_H_

#include <queue>
#include "state.h"

namespace pazusoba {
class StateCompare {
public:
    template <typename T>
    bool operator()(const T a, const T b) {
        return a < b;
    }
};

typedef std::priority_queue<State, std::vector<State>, StateCompare>
    StatePriorityQueue;
}  // namespace pazusoba

#endif
