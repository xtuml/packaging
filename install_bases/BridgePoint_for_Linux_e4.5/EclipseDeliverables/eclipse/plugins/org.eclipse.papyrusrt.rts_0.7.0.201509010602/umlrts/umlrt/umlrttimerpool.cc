// umlrttimerpool.cc

/*******************************************************************************
 * Copyright (c) 2014-2015 Zeligsoft (2009) Limited  and others.
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * which accompanies this distribution, and is available at
 * http://www.eclipse.org/legal/epl-v10.html
 *******************************************************************************/

// See umlrttimer.hh for documentation.
#include <stdlib.h>
#include "basefatal.hh"
#include "umlrttimerpool.hh"
#include "umlrtguard.hh"

UMLRTTimerPool::UMLRTTimerPool(UMLRTTimer timerElements[], size_t arraySize, size_t incrementSize) :
        UMLRTPool(incrementSize)
{
    UMLRTGuard g(mutex);

    if (arraySize && timerElements != NULL)
    {
        for (size_t i = arraySize - 1; i > 0; --i)
        {
            timerElements[i - 1].next = &timerElements[i];
            timerElements[i - 1].qid = qid++;
        }
        head = &timerElements[0];
    }
}

UMLRTTimerPool::UMLRTTimerPool(size_t incrementSize) :
        UMLRTPool(incrementSize)
{

}

void UMLRTTimerPool::grow()
{
    UMLRTTimer * newElements = new UMLRTTimer[increment]();

    for (size_t i = increment - 1; i > 0; --i)
    {
        newElements[i - 1].next = &newElements[i];
    }
    newElements[increment - 1].next = head;
    head = &newElements[0];
}

