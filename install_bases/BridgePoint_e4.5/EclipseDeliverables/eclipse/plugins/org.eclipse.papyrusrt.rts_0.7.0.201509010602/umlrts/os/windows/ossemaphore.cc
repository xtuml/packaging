// ossemaphore.cc

/*******************************************************************************
 * Copyright (c) 2015 Zeligsoft (2009) Limited  and others.
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * which accompanies this distribution, and is available at
 * http://www.eclipse.org/legal/epl-v10.html
 *******************************************************************************/

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <limits.h>
#include "basefatal.hh"
#include "ostime.hh"
#include "umlrttimespec.hh"
#include "umlrtsemaphore.hh"

UMLRTSemaphore::UMLRTSemaphore(int value)
{
    if(value < 0 || value > LONG_MAX) {
        FATAL("UMLRTSemaphore: Invalid initial count");
    }

    sem = CreateSemaphore(NULL, // default security attributes
            value,              // initial count
            LONG_MAX,           // maximum count
            NULL);              // unnamed semaphore

    if (!sem)
    {
        FATAL_ERRNO("CreateSemaphore");
    }
}

UMLRTSemaphore::~UMLRTSemaphore()
{
    if (!CloseHandle((HANDLE) sem))
    {
        FATAL_ERRNO("CloseHandle");
    }
}

// Wait forever for semaphore.
void UMLRTSemaphore::wait()
{
    if (WaitForSingleObject((HANDLE) sem, INFINITE) != WAIT_OBJECT_0)
    {
        FATAL_ERRNO("WaitForSingleObject");
    }
}

// Timed - returns non-zero for success, zero for timeout.
int UMLRTSemaphore::wait(uint32_t msec)
{
    struct timespec timeout;
    int success = !0;
    DWORD rc;

    UMLRTTimespec::timespecAbsAddMsec( &timeout, msec );
    DWORD absMsec = (DWORD) (timeout.tv_sec * 1000) + (timeout.tv_nsec / 1000000);
    rc = WaitForSingleObject((HANDLE) sem, absMsec);
    if (rc != WAIT_OBJECT_0)
    {
        if (rc != WAIT_TIMEOUT)
        {
            FATAL_ERRNO("WaitForSingleObject");
        }
        success = 0;
    }

    return success;
}

// Give semaphore back.
void UMLRTSemaphore::post()
{
    if (!ReleaseSemaphore((HANDLE) sem,  // handle to semaphore
            1,                  // increase count by one
            NULL))              // not interested in previous count
    {
        FATAL_ERRNO("ReleaseSemaphore");
    }
}
