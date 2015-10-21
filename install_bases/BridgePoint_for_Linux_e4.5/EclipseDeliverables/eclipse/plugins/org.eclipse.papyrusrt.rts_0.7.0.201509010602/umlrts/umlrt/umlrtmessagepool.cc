// umlrtmessagepool.cc

/*******************************************************************************
 * Copyright (c) 2014-2015 Zeligsoft (2009) Limited  and others.
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * which accompanies this distribution, and is available at
 * http://www.eclipse.org/legal/epl-v10.html
 *******************************************************************************/

// See umlrtmessagepool.hh for documentation.
#include <stdlib.h>
#include "basefatal.hh"
#include "umlrtmessagepool.hh"
#include "umlrtguard.hh"

UMLRTMessagePool::UMLRTMessagePool(UMLRTMessage messages[], size_t arraySize, size_t incrementSize) : UMLRTPool(incrementSize)
{
    UMLRTGuard g(mutex);

    if (arraySize && messages != NULL)
    {
        for (size_t i = arraySize - 1; i > 0; --i)
        {
            messages[i - 1].next = &messages[i];
            messages[i - 1].qid = qid++;
        }
        head = &messages[0];
    }
}

UMLRTMessagePool::UMLRTMessagePool(size_t incrementSize) : UMLRTPool(incrementSize)
{

}

void UMLRTMessagePool::grow()
{
    UMLRTMessage * newElements = new UMLRTMessage[increment]();

    for (size_t i = increment - 1; i > 0; --i)
    {
        newElements[i - 1].next = &newElements[i];
        newElements[i - 1].qid = qid++;
    }
    newElements[increment - 1].next = head;
    head = &newElements[0];
}

