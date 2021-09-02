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

typedef std::vector<State> StateList;
/// A special queue for multithreading.
/// Similar scores will be grouped together.
class SobaQueue {
    pint _thread;
    std::vector<StateList> _list;

public:
    SobaQueue(pint thread) : _thread(thread) {
        for (pint i = 0; i < thread; i++) {
            _list.emplace_back(StateList());
        }
    }

    pint size() const { return _list.size(); }
    const std::vector<StateList>& list() const { return _list; }
    StateList& operator[](pint index) { return _list[index]; }
};
}  // namespace pazusoba

#endif
