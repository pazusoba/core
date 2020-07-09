/**
 * queue.cpp
 * by Yiheng Quan
 */

#include <iostream>
#include "queue.h"

PadPriorityQueue::~PadPriorityQueue()
{
    auto it = top;
    while (it != NULL && it != bottom)
    {
        auto curr = it;
        it = it->next;
        delete curr;
    }
}

State *PadPriorityQueue::pop()
{
    if (top != NULL)
    {
        // This is element we want to return
        auto toPop = top;
        // Get the state and remove queue
        auto topState = toPop->state;

        // Rearrange the queue
        auto second = toPop->next;
        if (second == NULL)
        {
            // only one element, remove it
            this->top = NULL;
            this->bottom = NULL;
        }
        else
        {
            // make second top
            this->top = second;
            second->previous = NULL;
        }

        delete toPop;
        this->size--;
        return topState;
    }

    return NULL;
}

void PadPriorityQueue::insert(State *newState)
{
    if (newState == NULL)
        return;

    if (size == 0)
    {
        // Make top and bottom the new state
        this->top = new PadQueue(newState);
        this->bottom = this->top;
        this->size = 1;
    }
    else if (size < maxSize)
    {
        bool hasInserted = false;
        // Loop though the queue and find a great place tp insert
        auto it = this->top;
        while (it != NULL)
        {
            if (*newState > *(it->state))
            {
                // Insert here
                PadQueue *q = new PadQueue(newState);
                auto beforeIt = it->previous;
                if (beforeIt == NULL)
                {
                    // There is only one element in the queue
                    this->top = q;
                    q->next = it;
                    hasInserted = true;
                    break;
                }
                else
                {
                    // Insert in between
                    beforeIt->next = q;
                    q->previous = beforeIt;
                    q->next = it;
                    it->previous = q;
                }
                hasInserted = true;
                break;
            }
            it = it->next;
        };

        if (!hasInserted)
        {
            // This is the smallest
            PadQueue *q = new PadQueue(newState);
            bottom->next = q;
            q->previous = bottom;
            this->bottom = q;
        }

        this->size++;
    }
    else if (*newState > *(top->state))
    {
        PadQueue *newTop = new PadQueue(newState);
        // It was top but now it is second
        auto second = top;
        // Update top and link second with top
        this->top = newTop;
        newTop->next = second;
        second->previous = newTop;

        // Remove bottom and make second last the last
        auto secondLast = bottom->previous;
        delete this->bottom;
        secondLast->next = NULL;
        this->bottom = secondLast;
    }
}

void PadPriorityQueue::printQueue()
{
    using namespace std;
    auto it = top;
    while (it != NULL)
    {
        auto curr = it;
        cout << curr->state->score << " -> ";
        it = it->next;
    }
    cout << "NULL\n";
}
