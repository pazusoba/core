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

typedef std::deque<State> StateList;

/// A special queue for multithreading.
/// Similar scores will be grouped together.
/// TODO: make this a template instead of hard coded type.
class SobaQueue {
    pint _thread;
    std::vector<StateList> _list;

    // Track the number of states
    int _count = 0;
    int _currIndex = 9;
    // For now, only consider max 10 combo, we don't have to sort this
    // std::array<StateList, 10> _comboList;
    StatePriorityQueue _comboList;

public:
    SobaQueue(pint thread) : _thread(thread) {
        for (pint i = 0; i < thread; i++) {
            _list.emplace_back(StateList());
        }
    }

    pint threadSize() const { return _list.size(); }
    int size() const { return _count; }
    const std::vector<StateList>& list() const { return _list; }

    // Group all state based on score
    void group() {
        for (const auto& l : _list) {
            for (const auto& s : l) {
                _comboList.push(s);
                _count++;
            }
        }
    }

    bool empty() const { return _count == 0; }
    void pop() {
        _comboList.pop();
        _count--;
    }
    State next() { return _comboList.top(); }

    StateList& operator[](pint index) { return _list[index]; }
};
}  // namespace pazusoba

#endif
