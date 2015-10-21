// umlrtpool.cc

/*******************************************************************************
 * Copyright (c) 2014-2015 Zeligsoft (2009) Limited  and others.
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * which accompanies this distribution, and is available at
 * http://www.eclipse.org/legal/epl-v10.html
 *******************************************************************************/

#include <stdlib.h>
#include "basefatal.hh"
#include "basedebug.hh"
#include "umlrtguard.hh"
#include "umlrtpool.hh"
#include "umlrtqueueelement.hh"
#include "umlrtuserconfig.hh"

// See umlrtqueue.hh for documentation.

// Create an empty queue.
UMLRTPool::UMLRTPool(size_t incrementSize) :
        head(0), increment(incrementSize), qid(0)
{
}

UMLRTPool::~UMLRTPool()
{
}

// Remove the first element on the pool.
// Grow the list if it is empty
const UMLRTQueueElement * UMLRTPool::get()
{
    UMLRTGuard g(mutex);

    if (head == NULL)
    {
        grow();
    }
    const UMLRTQueueElement * element = head;
    head = element->next;
    return element;
}

// Add element back to pool (at front).
void UMLRTPool::put(const UMLRTQueueElement * element)
{
    UMLRTGuard g(mutex);
    if (element)
    {
        element->next = head;
        head = element;
    }
}
