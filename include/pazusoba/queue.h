//
// queue.h
// Priority Queue for Beam Search
//
// Created by Yiheng Quan on 22/08/2021
//

#ifndef _QUEUE_H_
#define _QUEUE_H_

#include <mutex>
#include <queue>
#include "state.h"

namespace pazusoba {
class QueueCompare {
public:
    template <typename T>
    bool operator()(const T a, const T b) {
        return a < b;
    }
};

/// A special queue for multithreading.
/// Similar scores will be grouped together.
/// TODO: make this a template instead of hard coded type.
template <class T>
class SobaQueue {
    pint _thread;
    std::vector<std::deque<T>> _threadList;
    std::vector<std::deque<T>> _queueList;
    std::mutex _mtx;

    // Track the number of states
    // int _currIndex = 9;
    // For now, only consider max 10 combo, we don't have to sort this
    // std::array<StateList, 10> _comboList;
    std::priority_queue<T, std::vector<T>, QueueCompare> _queue;

public:
    SobaQueue(pint thread) : _thread(thread) {
        for (pint i = 0; i < thread; i++) {
            _threadList.emplace_back(std::deque<T>());
        }
    }

    pint threadSize() const { return _threadList.size(); }
    int size() const { return _queue.size(); }
    const std::vector<std::deque<T>> list() const { return _threadList; }

    void insert(const T& state) { _queue.push(state); }
    void push(T state, pint thread) {
        if (_thread <= 1) {
            _queue.push(state);
        } else if (_mtx.try_lock()) {
            _queue.push(state);
            _mtx.unlock();
        } else {
            // insert to the list first
            _threadList[thread].push_front(state);
        }
    }

    // Group all state based on score
    void group(pint size) {
        for (auto& l : _threadList) {
            for (const auto& s : l) {
                _queue.push(s);
            }
            // Clear after grouping all nodes
            l.clear();
        }
        
        // This will be super slow
        auto newQueue = std::priority_queue<T, std::vector<T>, QueueCompare>();
        for (pint i = 0; i < size; i++) {
            if (_queue.empty())
                break;
            auto top = _queue.top();
            _queue.pop();
            newQueue.push(top);
        }
        
        _queue = newQueue;
    }

    bool empty() const { return _queue.empty(); }
    void pop() { _queue.pop(); }
    T next() { return _queue.top(); }

    std::deque<T>& operator[](pint index) { return _threadList[index]; }
};
}  // namespace pazusoba

#endif
