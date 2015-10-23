// umlrtsemaphore.hh

/*******************************************************************************
* Copyright (c) 2014-2015 Zeligsoft (2009) Limited  and others.
* All rights reserved. This program and the accompanying materials
* are made available under the terms of the Eclipse Public License v1.0
* which accompanies this distribution, and is available at
* http://www.eclipse.org/legal/epl-v10.html
*******************************************************************************/

#ifndef UMLRTSEMAPHORE_HH
#define UMLRTSEMAPHORE_HH

#include <stdint.h>

// UMLRTSemaphore is platform-independent semaphore.

typedef void * ossem_t;

class UMLRTSemaphore
{
public:
    UMLRTSemaphore( int value ); // Must explicitly set initial value.

    ~UMLRTSemaphore();

    // Wait forever for one to be available.
    void wait(void);

    // Wait only so many milliseconds for one to be available.
    // Timed - returns non-zero for success, zero for timeout.
    int wait( uint32_t msec );

    // Give one back.
    void post(void);

private:
    UMLRTSemaphore();

    ossem_t sem;
};

#endif // UMLRTMUTEX_HH
