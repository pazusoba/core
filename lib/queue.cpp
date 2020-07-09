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

void PadPriorityQueue::insert(State *newState)
{
    if (newState == NULL)
        return;

    if (size == 0)
    {
        // Make top and bottom the new state
        this->top = new PadQueue(newState);
        this->bottom = this->top;
        this->size++;
    }
    else if (size < maxSize)
    {
        bool hasInserted = false;
        // Loop though the queue and find a great place tp insert
        auto it = top;
        do
        {
            if (*newState > *(it->state))
            {
                // Insert here
                PadQueue *q = new PadQueue(newState);
                auto beforeIt = it->previous;
                if (beforeIt == NULL)
                {
                    // There is only one element in the queue
                    it->previous = q;
                }
                else
                {
                    // Insert in between
                    beforeIt->next = q;
                    q->previous = beforeIt;
                    it->previous = q;
                }
                q->next = it;
                hasInserted = true;
                break;
            }
            it = it->next;
        } while (it != NULL && it != bottom);

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
    while (it != NULL && it != bottom)
    {
        auto curr = it;
        cout << curr->state->score << " -> ";
        it = it->next;
    }
    cout << "NULL\n";
}
