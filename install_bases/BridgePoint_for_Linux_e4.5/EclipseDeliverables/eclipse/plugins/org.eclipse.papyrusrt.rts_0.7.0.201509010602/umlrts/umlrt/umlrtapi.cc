// umlrtapi.cc - catch-all RTS API called from everywhere
// Definitions here can be placed elsewhere at a later date...

/*******************************************************************************
* Copyright (c) 2014-2015 Zeligsoft (2009) Limited  and others.
* All rights reserved. This program and the accompanying materials
* are made available under the terms of the Eclipse Public License v1.0
* which accompanies this distribution, and is available at
* http://www.eclipse.org/legal/epl-v10.html
*******************************************************************************/

#include "umlrtapi.hh"
#include "umlrtslot.hh"
#include "umlrtcommsport.hh"
#include "umlrtcommsportrole.hh"
#include "umlrtcontroller.hh"
#include "umlrtmessagepool.hh"
#include "umlrtsignalelementpool.hh"
#include "umlrttimerpool.hh"
#include "basefatal.hh"
#include "basedebug.hh"

namespace umlrt
{
    // Allocate a signal from the system-pool.
    UMLRTSignalElement * SignalElementGetFromPool()
    {
        UMLRTSignalElementPool * pool = UMLRTController::getSignalElementPool();

        if (!pool)
        {
            FATAL("no signal pool defined (get)");
        }
        UMLRTSignalElement * element = (UMLRTSignalElement *)pool->get();
        BDEBUG(BD_SIGNALALLOC, "get from pool signal-qid[%d] %s\n", (element == NULL) ? -1 : element->qid, (element == NULL) ? "IGNORE NULL" : "ok");

        element->setAllocated(true);

        return element;
    }

    // Deallocate a signal back to the system pool.
    void SignalElementPutToPool( UMLRTSignalElement * element )
    {
        UMLRTSignalElementPool * pool = UMLRTController::getSignalElementPool();

        if (!pool)
        {
            FATAL("no signal pool defined (put)");
        }
        BDEBUG(BD_SIGNALALLOC, "put to pool signal-qid[%d] %s\n", (element == NULL) ? -1 : element->qid, (element == NULL) ? "IGNORE NULL" : "ok");

        element->setAllocated(false);

        return pool->put(element);
    }

    // Allocate a message from the system-pool.
    UMLRTMessage * MessageGetFromPool()
    {
        UMLRTMessagePool * pool = UMLRTController::getMessagePool();
        UMLRTMessage * msg;

        if (!pool)
        {
            FATAL("no message pool defined (get)");
        }
        if ((msg = (UMLRTMessage *)pool->get()) != NULL)
        {
            if (msg->allocated)
            {
                FATAL("Obtained an allocated message from the pool.");
            }
            msg->allocated = true;
        }
        return msg;
    }

    // Put a signal back to the system pool.
    void MessagePutToPool( UMLRTMessage * message )
    {
        UMLRTMessagePool * pool = UMLRTController::getMessagePool();

        UMLRTSignal invalid;

        message->signal = invalid; // Causes application signal element to be 'dereferenced'.

        if (!pool)
        {
            FATAL("no message pool defined (put)");
        }
        if (!message->allocated)
        {
            FATAL("Putting an unallocated message on the pool.");
        }
        message->allocated = false;

        pool->put(message);
    }

    // Allocate a timer from the system-pool.
    UMLRTTimer * TimerGetFromPool()
    {
        UMLRTTimerPool * pool = UMLRTController::getTimerPool();
        UMLRTTimer * timer;

        if (!pool)
        {
            FATAL("no timer pool defined (get)");
        }
        if ((timer = (UMLRTTimer *)pool->get()) != NULL)
        {
            if (timer->allocated)
            {
                FATAL("timer obtained from pool was already allocated.");
            }
            timer->allocated = true;
        }
        BDEBUG(BD_TIMERALLOC, "get from pool timer-qid[%d] %s\n", (timer == NULL) ? -1 : timer->qid, (timer == NULL) ? "IGNORE NULL" : "ok");

        return timer;
    }

    // Put a timer back to the system pool.
    void TimerPutToPool( UMLRTTimer * timer )
    {
        UMLRTTimerPool * pool = UMLRTController::getTimerPool();

        if (!pool)
        {
            FATAL("no timer pool defined (put)");
        }
        if (!timer->allocated)
        {
            FATAL("attempting to return an unallocated timer to the pool.");
        }
        timer->allocated = false;

        BDEBUG(BD_TIMERALLOC, "put to pool timer-qid[%d] signal-qid[%d] %s\n",
                timer->qid, timer->signal.getQid());

        UMLRTOutSignal invalid;

        timer->signal = invalid; // Causes application signal element to be 'dereferenced'.

        pool->put(timer);
    }
}
