// umlrtpool.hh

/*******************************************************************************
 * Copyright (c) 2014-2015 Zeligsoft (2009) Limited  and others.
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * which accompanies this distribution, and is available at
 * http://www.eclipse.org/legal/epl-v10.html
 *******************************************************************************/

#ifndef UMLRTPOOL_HH
#define UMLRTPOOL_HH

#include "umlrtmutex.hh"
#include <stddef.h>

struct UMLRTQueueElement;

// UMLRTPool - base type for resource pools of elements typed by UMLRTQueueElement.

// Basic operations are put() and get(). The implementation is a LIFO queue.

class UMLRTPool
{
public:
    UMLRTPool(size_t incrementSize);
    virtual ~UMLRTPool() = 0;

    // Get an element from the pool.
    const UMLRTQueueElement * get();

    // Add element to the pool.
    void put(const UMLRTQueueElement * element);

protected:
    const UMLRTQueueElement * head;
    const size_t increment;  // the size to grow the pool if needed
    int qid; // For debug. Give elements a qid for easier tracking.
    UMLRTMutex mutex;

private:
    virtual void grow() = 0;
};

#endif // UMLRTPOOL_HH
