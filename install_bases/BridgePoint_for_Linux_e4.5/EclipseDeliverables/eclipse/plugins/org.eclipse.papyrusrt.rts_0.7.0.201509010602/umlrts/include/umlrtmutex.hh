// umlrtmutex.hh

/*******************************************************************************
* Copyright (c) 2014-2015 Zeligsoft (2009) Limited  and others.
* All rights reserved. This program and the accompanying materials
* are made available under the terms of the Eclipse Public License v1.0
* which accompanies this distribution, and is available at
* http://www.eclipse.org/legal/epl-v10.html
*******************************************************************************/

#ifndef UMLRTMUTEX_HH
#define UMLRTMUTEX_HH

#include <stdint.h>

// UMLRTMutex is platform-independent mutual-exclusion.

typedef void * osmutex_t;

class UMLRTMutex
{
public:
    UMLRTMutex();

    // Can create a mutex which starts life as taken already.
    UMLRTMutex( bool taken );

    ~UMLRTMutex();

    // Wait forever for it to be available.
    void take(void);

    // Wait only so many milliseconds.
    // Timed - returns non-zero for success, zero for timeout.
    int take( uint32_t msec );

    // Give it back.
    void give(void);

private:
    osmutex_t mutex;
};

#endif // UMLRTMUTEX_HH
