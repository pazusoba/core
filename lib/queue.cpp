/**
 * queue.cpp
 * by Yiheng Quan
 */

#include "queue.h"

PadPriorityQueue::~PadPriorityQueue()
{
    auto it = top;
    while (it != NULL && it != bottom)
    {
        auto temp = it;
        it = it -> next;
        delete temp;
    }
}

void PadPriorityQueue::insert(State *newState)
{
    if (newState == NULL)
        return;

    if (size < maxSize)
    {
        // Insert normally
    }
    else
    {
        // if (*newState > top.state)
        // {

        // }
    }
}
