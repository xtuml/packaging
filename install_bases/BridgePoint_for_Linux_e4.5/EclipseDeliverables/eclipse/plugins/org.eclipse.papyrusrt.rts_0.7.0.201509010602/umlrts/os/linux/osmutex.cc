// osmutex.cc

/*******************************************************************************
 * Copyright (c) 2015 Zeligsoft (2009) Limited  and others.
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * which accompanies this distribution, and is available at
 * http://www.eclipse.org/legal/epl-v10.html
 *******************************************************************************/

#include <pthread.h>
#include <errno.h>
#include <time.h>
#include "basefatal.hh"
#include "umlrtmutex.hh"
#include "umlrttimespec.hh"

// platform-dependent implementation of mutual-exclusion.

UMLRTMutex::UMLRTMutex()
{
    mutex = new pthread_mutex_t;

    if (pthread_mutex_init((pthread_mutex_t *) mutex, NULL) != 0)
    {
        FATAL_ERRNO("pthread_mutex_init");
    }
}

// Can create a mutex which starts life as taken already.
UMLRTMutex::UMLRTMutex(bool taken)
{
    mutex = new pthread_mutex_t;

    if (pthread_mutex_init((pthread_mutex_t *) mutex, NULL) != 0)
    {
        FATAL_ERRNO("pthread_mutex_init");
    }

    if (taken)
    {
        take();
    }
}

UMLRTMutex::~UMLRTMutex()
{
    pthread_mutex_destroy((pthread_mutex_t *) mutex);
    delete (pthread_mutex_t *) mutex;
}
// Wait forever for mutex.
void UMLRTMutex::take()
{
    if (pthread_mutex_lock((pthread_mutex_t *) mutex) < 0)
    {
        FATAL_ERRNO("pthread_mutex_lock");
    }
}

// Timed - returns non-zero for success, zero for timeout.
#if (((__GNUC__ * 100) + __GNUC_MINOR__) >= 406)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wmaybe-uninitialized"
#endif

int UMLRTMutex::take(uint32_t msec)
{
    struct timespec timeout;
    int success = !0;

    UMLRTTimespec::timespecAbsAddMsec(&timeout, msec);
    if (pthread_mutex_timedlock((pthread_mutex_t *) mutex, &timeout) < 0)
    {
        int errno_ = errno;
        if (errno_ != ETIMEDOUT)
        {
            FATAL_ERRNO("pthread_mutex_timedlock");
        }
        success = 0;
    }
    return success;
}
#if (((__GNUC__ * 100) + __GNUC_MINOR__) >= 406)
#pragma GCC diagnostic pop
#endif

// Give mutex back.
void UMLRTMutex::give()
{
    if (pthread_mutex_unlock((pthread_mutex_t *) mutex) < 0)
    {
        FATAL_ERRNO("pthread_mutex_unlock");
    }
}

