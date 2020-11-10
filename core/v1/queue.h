/**
 * queue.h
 * This is a special queue only for `PSolver`.
 * - It has a fixed size and will reject new states if it is not better than the current best
 * - It also stores a list of best states based on their state 
 *
 * by Yiheng Quan
 */

#ifndef MY_QUEUE_H
#define MY_QUEUE_H

#include "state.h"

struct PElement
{
    // Current state
    PState *state = nullptr;
    // Link to next state
    PElement *next = nullptr;
    // Link to next state
    PElement *previous = nullptr;

    PElement(PState *s) : state(s) {}
};

struct PPriorityQueue
{
    // This is max size possible for this queue
    int maxSize;
    int size = 0;

    PElement *top = nullptr;
    PElement *bottom = nullptr;

    PPriorityQueue(int size) : maxSize(size) {}
    ~PPriorityQueue();

    // Insert a new state if size < maxSize or this state is better than top state
    void insert(PState *);
    // Return and remove the top state
    PState *pop();
    // Print out all elements in this queue
    void printQueue();
    // Loop through the queue
    void foreach (void func(PElement *));
};

#endif
