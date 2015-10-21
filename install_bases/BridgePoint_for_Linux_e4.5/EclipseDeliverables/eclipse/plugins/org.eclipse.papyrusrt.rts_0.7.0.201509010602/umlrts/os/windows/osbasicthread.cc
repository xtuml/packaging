// osbasicthread.cc

/*******************************************************************************
 * Copyright (c) 2015 Zeligsoft (2009) Limited  and others.
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * which accompanies this distribution, and is available at
 * http://www.eclipse.org/legal/epl-v10.html
 *******************************************************************************/

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <process.h>    /* _beginthreadex, _endthreadex */
#include "basefatal.hh"
#include "umlrtbasicthread.hh"

// Start the thread, passing in a single argument.

void UMLRTBasicThread::start(void * args)
{
    threadargs.inst = this;
    threadargs.args = args;

    uintptr_t tid_ = _beginthreadex(NULL, // security
            0,  // stack size, same size as the main thread stack
            (unsigned (__stdcall*)(void*)) static_entrypoint,   // thread routine
            &threadargs    ,// thread args
            NULL,// initial state flag
            NULL// thrdaddr not used
    );

    if (tid_ == 0)
    {
        FATAL_ERRNO("_beginthreadex");
    }

    tid = (osthreadid_t) tid_;
}

// Wait for the thread to complete and get returned value.

void * UMLRTBasicThread::join()
{
    void * ret = 0;
    DWORD rc;

    rc = WaitForSingleObject(tid, INFINITE);
    if (rc == WAIT_OBJECT_0)
    {
        // the thread has finished, get the status code
        ::GetExitCodeThread(tid, (LPDWORD) & ret);
    }
    else
    {
        FATAL_ERRNO("WaitForSingleObject");
    }

    return ret;
}

// Returns true if this thread is currently running thread.

bool UMLRTBasicThread::isMyThread()
{
    return UMLRTBasicThread::selfId() == tid;
}

// Return running thread id.
/*static*/
osthreadid_t UMLRTBasicThread::selfId()
{
    return (osthreadid_t) GetCurrentThreadId();
}
