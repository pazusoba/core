/**
 * queue.cpp
 * by Yiheng Quan
 */

#include <iostream>
#include "queue.h"

PPriorityQueue::~PPriorityQueue()
{
    foreach ([](PElement *curr) {
        delete curr;
    });
}

PState *PPriorityQueue::pop()
{
    if (size > 0)
    {
        // This is element we want to return
        auto toPop = top;
        // Get the state and remove queue
        auto topState = toPop->state;

        // Rearrange the queue
        auto second = toPop->next;
        if (second == nullptr)
        {
            // only one element, remove it
            this->top = nullptr;
            this->bottom = nullptr;
        }
        else
        {
            // make second top
            this->top = second;
            second->previous = nullptr;
        }

        delete toPop;
        size--;
        return topState;
    }

    return nullptr;
}

void PPriorityQueue::insert(PState *newState)
{
    if (newState == nullptr)
        return;

    if (size == 0)
    {
        // Make top and bottom the new state
        this->top = new PElement(newState);
        this->bottom = this->top;
        this->size = 1;
    }
    else if (size < maxSize)
    {
        bool hasInserted = false;
        // Loop though the queue and find a great place tp insert
        auto it = this->top;
        while (it != nullptr)
        {
            if (*newState > *(it->state))
            {
                // Insert here
                PElement *q = new PElement(newState);
                auto beforeIt = it->previous;
                if (beforeIt == nullptr)
                {
                    // There is only one element in the queue
                    this->top = q;
                    q->next = it;
                    // Link previous as well
                    it->previous = q;
                    // There was only one element so it is now bottom
                    if (size == 1)
                        this->bottom = it;
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
            PElement *q = new PElement(newState);
            bottom->next = q;
            q->previous = bottom;
            this->bottom = q;
        }

        this->size++;
    }
    else if (*newState > *(top->state))
    {
        PElement *newTop = new PElement(newState);
        // It was top but now it is second
        auto topNext = top;
        // Update top and link second with top
        this->top = newTop;
        newTop->next = topNext;
        topNext->previous = newTop;

        // Remove bottom and make second last the last
        auto toRemove = bottom;
        auto secondLast = toRemove->previous;
        this->bottom = secondLast;
        secondLast->next = nullptr;
        delete toRemove;
    }
}

void PPriorityQueue::printQueue()
{
    using namespace std;
    foreach ([](PElement *curr) {
        cout << curr->state->score << " -> ";
    });
    cout << "nullptr\n";
}

void PPriorityQueue::foreach (void func(PElement *))
{
    auto it = top;
    while (it != nullptr)
    {
        auto curr = it;
        it = it->next;
        func(curr);
    }
}