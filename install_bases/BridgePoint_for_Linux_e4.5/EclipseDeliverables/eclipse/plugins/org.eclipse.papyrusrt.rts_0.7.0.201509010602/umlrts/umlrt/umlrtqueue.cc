// umlrtqueue.cc

/*******************************************************************************
* Copyright (c) 2014-2015 Zeligsoft (2009) Limited  and others.
* All rights reserved. This program and the accompanying materials
* are made available under the terms of the Eclipse Public License v1.0
* which accompanies this distribution, and is available at
* http://www.eclipse.org/legal/epl-v10.html
*******************************************************************************/

#include <stdlib.h>
#include <string.h>
#include "basefatal.hh"
#include "basedebug.hh"
#include "umlrtguard.hh"
#include "umlrtqueue.hh"
#include "umlrtqueueelement.hh"

// See umlrtqueue.hh for documentation.

// Create an empty queue.
UMLRTQueue::UMLRTQueue() : head(0), tail(0), qid(0) {}

// Queue creation can (optionally) add elements.
UMLRTQueue::UMLRTQueue( UMLRTQueueElement * first, size_t arraySize, size_t elementSize) : head(0), tail(0), qid(0)
{
    if (arraySize)
    {
        head = first;

        uint8_t * p = (uint8_t *)first;

        tail = (UMLRTQueueElement *)(p + elementSize*(arraySize-1));

        // Need to do pointer arithmetic to point to elements
        UMLRTQueueElement * element = (UMLRTQueueElement *)p;
        for (size_t i = 0; i < arraySize; ++i)
        {
            memset(p, 0, elementSize); // In case elements were not from BSS.
            p += elementSize;
            element->next = (UMLRTQueueElement *)p;
            element->qid = qid++;
            element = (UMLRTQueueElement *)p;

        }
        tail->next = NULL; // Above loop left tail->next pointing to first byte after pool.
    }
}

// Remove the first element on the queue.
const UMLRTQueueElement * UMLRTQueue::dequeue()
{
    UMLRTGuard g( mutex );
    const UMLRTQueueElement * element = head;
    if (element)
    {
        head = element->next;
    }
    return element;
}

// Add element on the tail of the queue.
void UMLRTQueue::enqueue( const UMLRTQueueElement * element, bool front )
{
    UMLRTGuard g( mutex );

    if (!front)
    {
        // Queue to tail.
        element->next = NULL;
        if (head == NULL)
        {
            // Queue was empty.
            head = tail = element;
        }
        else
        {
            // Append to tail.
            tail->next = element;
            tail = element;
        }
    }
    else
    {
        // Queue to front.
        element->next = head;
        if (head == NULL)
        {
            // Queue was empty.
            tail = element;
        }
        head = element;
    }
}

// Return all elements from the queue, still linked, and empty the queue.

// Also returns the tail (as 'last') so the returned elements can be
// efficiently appended to another queue.

const UMLRTQueueElement * UMLRTQueue::dequeueAll( const UMLRTQueueElement * * last )
{
    UMLRTGuard g( mutex );

    const UMLRTQueueElement * all = head;

    *last = tail;

    tail = head = NULL;

    return( all );
}

// Append a list of elements (that are already linked together) to the queue.

void UMLRTQueue::enqueueAll( const UMLRTQueueElement * all, const UMLRTQueueElement * last )
{
    UMLRTGuard g( mutex );

    if (all)
    {
        if (!last)
        {
            FATAL("all != 0, last == 0");
        }
        if (!head)
        {
            // This queue was empty - input list is the new queue.
            head = all;
            tail = last;
        }
        else
        {
            // Append input list to the tail of this queue.
            tail->next = all;
            tail = last;
        }
    }
}

int UMLRTQueue::remove( match_compare_t compare, match_notify_t notify, void * userData, bool one )
{
    int count = 0;

    UMLRTGuard g( mutex );

    // Start at the head and delete until the head no longer needs deleting.
    const UMLRTQueueElement * headcopy;
    bool done = false;
    bool notified = false;
    while (((headcopy = head) != NULL) && !done)
    {
        if (compare(head, userData))
        {
            // This head has to go. Unlink it - we've remembered it for notify.
            head = head->next;

            notify(headcopy, userData);

            notified = true;
            ++count;
            if (one)
            {
                done = true;
            }
        }
        else
        {
            done = true;
        }
    }
    // Head is either NULL or not to be purged (a 'keeper')- we've checked it above.
    // Go through queue and potentially delete the ones after the last 'keeper'.
    done = (one && notified);

    if (head != NULL && !done)
    {
        const UMLRTQueueElement * keeper = head;
        const UMLRTQueueElement * candidate;

        while ((candidate = keeper->next) != NULL)
        {
            // See if this candidate needs removal.
            if (compare(candidate, userData))
            {
                // Unlink candidate from queue.
                keeper->next = candidate->next;

                // Notify caller that we've removed it.
                notify(candidate, userData);
                ++count;
                if (one)
                {
                    done = true;
                }
            }
            else
            {
                // Didn't delete candidate. It's the next 'keeper'.
                keeper = candidate;
            }
        }
        tail = keeper;
    }
    return count;
}

int UMLRTQueue::walk( walk_callback_t callback, void * userData ) const
{
    UMLRTGuard g( mutex );

    int count = 0;

    const UMLRTQueueElement * element = head;

    while (element != NULL)
    {
        callback( element, userData );
        count++;
        element = element->next;
    }
    return count;
}
