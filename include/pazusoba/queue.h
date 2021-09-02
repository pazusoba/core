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
/// TODO: make this a template instead of hard coded type.
class SobaQueue {
    pint _thread;
    std::vector<StateList> _list;

    // Track the number of states
    pint _count = 0;
    // For now, only consider max 10 combo, we don't have to sort this
    std::array<StateList, 10> _comboList;

public:
    SobaQueue(pint thread) : _thread(thread) {
        for (pint i = 0; i < thread; i++) {
            _list.emplace_back(StateList());
        }
    }

    pint size() const { return _list.size(); }
    const std::vector<StateList>& list() const { return _list; }
    bool empty() const { return _count == 0; }
    State pop() {
        // Start from highest
        for (int i = 10; i > 0; i--) {
            auto list = _comboList[i - 1];
            if (list.empty())
                continue;
            auto last = list.back();
            list.pop_back();
            return last;
        }
    }

    StateList& operator[](pint index) { return _list[index]; }
};
}  // namespace pazusoba

#endif
