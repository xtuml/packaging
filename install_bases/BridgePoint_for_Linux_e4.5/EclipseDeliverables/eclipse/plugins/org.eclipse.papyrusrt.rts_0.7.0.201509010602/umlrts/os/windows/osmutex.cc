// osmutex.cc

/*******************************************************************************
 * Copyright (c) 2015 Zeligsoft (2009) Limited  and others.
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * which accompanies this distribution, and is available at
 * http://www.eclipse.org/legal/epl-v10.html
 *******************************************************************************/

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include "basefatal.hh"
#include "ostime.hh"
#include "umlrtmutex.hh"
#include "umlrttimespec.hh"

// platform-dependent implementation of mutual-exclusion.

UMLRTMutex::UMLRTMutex()
{
    mutex = CreateMutex(0, FALSE, 0);

    if (!mutex)
    {
        FATAL_ERRNO("CreateMutex");
    }
}

// Can create a mutex which starts life as taken already.
UMLRTMutex::UMLRTMutex(bool taken)
{
    mutex = CreateMutex(0, FALSE, 0);

    if (!mutex)
    {
        FATAL_ERRNO("CreateMutex");
    }

    if (taken)
    {
        take();
    }
}

UMLRTMutex::~UMLRTMutex()
{
    CloseHandle((HANDLE) mutex);
}

// Wait forever for mutex.
void UMLRTMutex::take()
{
    if (WaitForSingleObject((HANDLE) mutex, INFINITE) != WAIT_OBJECT_0)
    {
        FATAL_ERRNO("WaitForSingleObject");
    }
}

// Timed - returns non-zero for success, zero for timeout.
int UMLRTMutex::take(uint32_t msec)
{
    struct timespec timeout;
    int success = !0;
    DWORD rc;

    UMLRTTimespec::timespecAbsAddMsec(&timeout, msec);
    DWORD absMsec = (DWORD) (timeout.tv_sec * 1000) + (timeout.tv_nsec / 1000000);
    rc = WaitForSingleObject((HANDLE) mutex, absMsec);
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

// Give mutex back.
void UMLRTMutex::give()
{
    if (!ReleaseMutex((HANDLE) mutex))
    {
        FATAL_ERRNO("ReleaseMutex");
    }
}

