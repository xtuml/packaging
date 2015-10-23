// osbasicthread.cc

/*******************************************************************************
 * Copyright (c) 2015 Zeligsoft (2009) Limited  and others.
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * which accompanies this distribution, and is available at
 * http://www.eclipse.org/legal/epl-v10.html
 *******************************************************************************/

#include <pthread.h>
#include <string.h>
#include "basefatal.hh"
#include "umlrtbasicthread.hh"

#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif

// Start the thread, passing in a single argument.

void UMLRTBasicThread::start(void * args)
{
    if (sizeof(pthread_t) > sizeof(void *))
    {
        FATAL_ERRNO("UMLRTBasicThread::start");
    }

    pthread_attr_t attr;
    pthread_t tid_;

    memset(&attr, 0, sizeof(attr));

    if (pthread_attr_init(&attr) < 0)
    {
        FATAL_ERRNO("pthread_attr_init");
    }
    threadargs.inst = this;
    threadargs.args = args;

    if (pthread_create(&tid_, &attr, static_entrypoint, &threadargs) < 0)
    {
        FATAL_ERRNO("pthread_create");
    }
    else
    {
        tid = (osthreadid_t) tid_;
    }
#if 0
    // Not universally available. Take it out for now.
    if (pthread_setname_np(tid, name) < 0)
    {
        FATAL_ERRNO("pthread_setname_np");
    }
#endif
}

// Wait for the thread to complete and get returned value.

void * UMLRTBasicThread::join()
{
    void *ret;
    if (pthread_join((pthread_t) tid, &ret) < 0)
    {
        FATAL_ERRNO("pthread_join");
    }
    return ret;
}

// Returns true if this thread is currently running thread.

bool UMLRTBasicThread::isMyThread()
{
    return pthread_self() == (pthread_t) tid;
}

// Return running thread id.
/*static*/
osthreadid_t UMLRTBasicThread::selfId()
{
    return (osthreadid_t) pthread_self();
}
