/**
 * queue.h
 * This is a special queue only for `PadSolver`.
 * - It has a fixed size and will reject new states if it is not better than the current best
 * - It also stores a list of best states based on their state 
 *
 * by Yiheng Quan
 */

#include "state.h"

struct PadQueue
{
    // Current state
    State *state = NULL;
    // Link to next state
    PadQueue *next = NULL;
    // Link to next state
    PadQueue *previous = NULL;

    PadQueue(State *s) : state(s) {}
};

struct PadPriorityQueue {
    int maxSize;
    int size = 0;

    PadQueue *top = NULL;
    PadQueue *bottom = NULL;

    PadPriorityQueue(int size) : maxSize(size) {}
    ~PadPriorityQueue();

    // Insert a new state if size < maxSize or this state is better than top state
    void insert(State *);
    // Print out all elements in this queue
    void printQueue();
};
