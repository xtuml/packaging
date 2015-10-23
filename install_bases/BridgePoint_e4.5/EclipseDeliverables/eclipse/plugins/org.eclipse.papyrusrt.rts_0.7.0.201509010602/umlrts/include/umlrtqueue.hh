// umlrtqueue.hh

/*******************************************************************************
* Copyright (c) 2014-2015 Zeligsoft (2009) Limited  and others.
* All rights reserved. This program and the accompanying materials
* are made available under the terms of the Eclipse Public License v1.0
* which accompanies this distribution, and is available at
* http://www.eclipse.org/legal/epl-v10.html
*******************************************************************************/

#ifndef UMLRTQUEUE_HH
#define UMLRTQUEUE_HH

#include <stddef.h>
#include "umlrtmutex.hh"

struct UMLRTQueueElement;

// UMLRTQueue - base type for queues.

class UMLRTQueue
{
public:
    typedef bool (*match_compare_t)( const UMLRTQueueElement * element, void * userData );
    typedef void (*match_notify_t)( const UMLRTQueueElement * element, void * userData );
    typedef bool (*walk_callback_t)( const UMLRTQueueElement * element, void * userData );

    // Create an empty queue.
    UMLRTQueue();

    // Queue creation can (optionally) add elements.
    UMLRTQueue( UMLRTQueueElement * first, size_t arraySize, size_t elementSize);

    // Remove the first element on the queue.
    const UMLRTQueueElement * dequeue();

    // Add element on the tail of the queue.
    void enqueue( const UMLRTQueueElement * element, bool front = false );

    // Return all elements from the queue, still linked, and empty the queue.

    // Also returns the tail (as 'last') so the returned elements can be
    // efficiently appended to another queue.

    const UMLRTQueueElement * dequeueAll( const UMLRTQueueElement * * last );

    // Append a list of elements (already linked together) to the queue.

    void enqueueAll( const UMLRTQueueElement * all, const UMLRTQueueElement * last );

    // isEmpty() only true the instant it is checked. It can be used by a sole consumer of the queue assuming other
    // synchronization is used to ensure the queue gets checked again (as something can be added
    // immediately following (or during) a call to this method which may return 'true').
    bool isEmpty() const { return head == NULL; }

    // Remove elements by visiting each and seeing if a callback function says the element should be deleted. Return count of elements removed.
    int remove( match_compare_t compare, match_notify_t notify, void * userData, bool one = false );

    UMLRTMutex& getMutex() const { return mutex; }

    // Walk the elements of the queue, calling the callback. Abort if the callback returns true. Return number of elements walked.
    int walk( walk_callback_t callback, void * userData ) const;

protected:
    mutable const UMLRTQueueElement * head;
    mutable const UMLRTQueueElement * tail; // WARNING: undefined if 'head == 0'.
    mutable UMLRTMutex mutex;
    mutable int qid;
};



#endif // UMLRTQUEUE_HH
