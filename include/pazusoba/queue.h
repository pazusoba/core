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

/// A special queue for multithreading.
/// Similar scores will be grouped together.
/// TODO: make this a template instead of hard coded type.
template <class T>
class SobaQueue {
    pint _thread;
    std::vector<std::deque<T>> _list;

    // Track the number of states
    int _count = 0;
    // int _currIndex = 9;
    // For now, only consider max 10 combo, we don't have to sort this
    // std::array<StateList, 10> _comboList;
    std::priority_queue<T, std::vector<T>, StateCompare> _comboList;

public:
    SobaQueue(pint thread) : _thread(thread) {
        for (pint i = 0; i < thread; i++) {
            _list.emplace_back(std::deque<T>());
        }
    }

    pint threadSize() const { return _list.size(); }
    int size() const { return _count; }
    const std::vector<std::deque<T>> list() const { return _list; }

    void insert(const State& state) {
        _comboList.push(state);
        _count++;
    }

    // Group all state based on score
    void group() {
        for (auto& l : _list) {
            for (const auto& s : l) {
                _comboList.push(s);
                _count++;
            }
            // Clear after grouping all nodes
            l.clear();
        }
    }

    bool empty() const { return _count == 0; }
    void pop() {
        _comboList.pop();
        _count--;
    }
    T next() { return _comboList.top(); }

    std::deque<T>& operator[](pint index) { return _list[index]; }
};
}  // namespace pazusoba

#endif
