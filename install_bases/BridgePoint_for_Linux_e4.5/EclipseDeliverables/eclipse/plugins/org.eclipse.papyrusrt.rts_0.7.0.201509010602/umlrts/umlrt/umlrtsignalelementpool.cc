// umlrtsignalelementpool.cc

/*******************************************************************************
 * Copyright (c) 2014-2015 Zeligsoft (2009) Limited  and others.
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * which accompanies this distribution, and is available at
 * http://www.eclipse.org/legal/epl-v10.html
 *******************************************************************************/

// See umlrtsignalpool.hh for documentation.
#include <stdlib.h>
#include "basefatal.hh"
#include "umlrtguard.hh"
#include "umlrtsignalelementpool.hh"

UMLRTSignalElementPool::UMLRTSignalElementPool(UMLRTSignalElement signalElements[],
        size_t arraySize, size_t incrementSize) :
        UMLRTPool(incrementSize)
{
    UMLRTGuard g(mutex);

    if (arraySize && signalElements != NULL)
    {
        for (size_t i = arraySize - 1; i > 0; --i)
        {
            signalElements[i - 1].next = &signalElements[i];
            signalElements[i - 1].qid = qid++;
        }
        head = &signalElements[0];
    }
}

UMLRTSignalElementPool::UMLRTSignalElementPool(size_t incrementSize) :
        UMLRTPool(incrementSize)
{

}

void UMLRTSignalElementPool::grow()
{
    UMLRTSignalElement * newElements = new UMLRTSignalElement[increment]();

    for (size_t i = increment - 1; i > 0; --i)
    {
        newElements[i - 1].next = &newElements[i];
    }
    newElements[increment - 1].next = head;
    head = &newElements[0];
}

