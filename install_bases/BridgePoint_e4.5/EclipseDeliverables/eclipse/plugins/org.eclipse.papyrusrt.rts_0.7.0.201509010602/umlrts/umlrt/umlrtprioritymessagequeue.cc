// umlrtprioritymessagequeue.cc

/*******************************************************************************
* Copyright (c) 2014-2015 Zeligsoft (2009) Limited  and others.
* All rights reserved. This program and the accompanying materials
* are made available under the terms of the Eclipse Public License v1.0
* which accompanies this distribution, and is available at
* http://www.eclipse.org/legal/epl-v10.html
*******************************************************************************/

#include "basedebug.hh"
#include "basedebugtype.hh"
#include "basefatal.hh"
#include "umlrtapi.hh"
#include "umlrtslot.hh"
#include "umlrtcommsportrole.hh"
#include "umlrtprioritymessagequeue.hh"
#include "umlrtmessage.hh"
#include "umlrttimer.hh"
#include "osnotify.hh"
#include <stdlib.h>
#include <stdio.h>

// See umlrtprioritymessagequeue.hh for documentation.

UMLRTPriorityMessageQueue::UMLRTPriorityMessageQueue( const char * owner_ ) : owner(owner_)
{
    notifyPtr = new UMLRTNotify();
}

UMLRTPriorityMessageQueue::~UMLRTPriorityMessageQueue()
{
    delete notifyPtr;
}

// Return the queue associated with a priority.
UMLRTMessageQueue & UMLRTPriorityMessageQueue::getQueue( UMLRTSignalElement::Priority priority )
{
    return queue[priority];
}

// Used to transfer all messages from each priority queue to
// their associated priority-queue

void UMLRTPriorityMessageQueue::moveAll( UMLRTPriorityMessageQueue & fromQueue )
{
    for (UMLRTSignalElement::Priority priority = 0; priority < UMLRTSignalElement::PRIORITY_MAX; ++priority)
    {
        const UMLRTQueueElement * last; // These are UMLRTMessage's.
        const UMLRTQueueElement * all = fromQueue.getQueue(priority).dequeueAll(&last);

        queue[priority].enqueueAll(all, last);
    }
}

// Get all expired timers from the timer-queue and enqueue these to this the capsule priority message queue.
// The 'incomingQueue' is also a priority queue, but timers are never queued to the incomingQueue instance.
void UMLRTPriorityMessageQueue::queueTimerMessages( UMLRTTimerQueue * timerQueue )
{
    UMLRTTimer * timer = timerQueue->dequeue();

    while (timer != NULL)
    {
        if (!timer->allocated)
        {
            FATAL("%s:timer (%p) obtained from timer-queue is not allocated.", owner, timer);
        }
        UMLRTMessage * msg = umlrt::MessageGetFromPool();

        if (!msg)
        {
            FATAL("message allocation failed during timout message creation.");
        }
        msg->signal = timer->signal;
        msg->destPort = timer->destPort;
        msg->destSlot = timer->destSlot;
        msg->srcPortIndex = 0; // Timer ports are not replicated.
        msg->isCommand = false;

        char tmbuf[UMLRTTimespec::TIMESPEC_TOSTRING_SZ];
        BDEBUG(BD_TIMER, "%s: queue timer msg signal id(%d) to %s(%s) isInterval(%d) due(%s)\n",
                owner, msg->signal.getId(), msg->destPort->slot->name, msg->destPort->role()->name, timer->isInterval,
                timer->isInterval ?  timer->due.toStringRelative(tmbuf, sizeof(tmbuf)): timer->due.toString(tmbuf, sizeof(tmbuf)));

        enqueue(msg);

        if (timer->isInterval)
        {
            // This is an interval timer. Adjust it's due-time and requeue it.
            timer->due += timer->interval;
            timerQueue->enqueue(timer);
        }
        else
        {
            // This timer can be put back on the system pool.
            umlrt::TimerPutToPool(timer);
        }

        // See if there's another one expired.
        timer = timerQueue->dequeue();
    }
}

// Get the highest priority message from the collection of queues.

UMLRTInMessage * UMLRTPriorityMessageQueue::dequeueHighestPriority()
{
    UMLRTInMessage * msg = 0;

    for (UMLRTSignalElement::Priority priority = (UMLRTSignalElement::PRIORITY_MAX-1);
            (priority >= UMLRTSignalElement::PRIORITY_BACKGROUND) && (priority < (UMLRTSignalElement::PRIORITY_MAX)) && !msg; --priority)
    {
        msg = (UMLRTInMessage *)queue[priority].dequeue();

    }
    if (msg != NULL)
    {
        // Source port may no longer exist.
        BDEBUG(BD_MSG, "%s: msg dequeued priority(%d) -> %s %s[%d] signal id(%d)name(%s) payloadSz(%d)\n",
                owner,
                msg->signal.getPriority(),
                msg->isCommand ? "" : msg->destPort->slot->name,
                msg->isCommand ? "isCommand" : msg->destPort->role()->name,
                msg->isCommand ? 0 : msg->sapIndex0,
                msg->getSignalId(),
                msg->getSignalName(),
                msg->signal.getPayloadSize());
    }
    return msg;
}

// Put the message into its appropriate priority-queue.
void UMLRTPriorityMessageQueue::enqueue( UMLRTMessage * msg, bool front )
{
    UMLRTSignalElement::Priority priority = msg->signal.getPriority();

    if ((priority < 0) || (priority >= UMLRTSignalElement::PRIORITY_MAX))
    {
        FATAL("enqueue with bad priority (%d)", priority);
    }
    if (!msg->isCommand && msg->destPort == NULL)
    {
        FATAL("enqueuing a message with no destination port");
    }
    // Source port may no longer exist.
    BDEBUG(BD_MSG, "%s: msg enqueued priority(%d) -> capsule %s port %s[%d] signal id(%d)(%s) payloadSz(%d)\n",
            owner,
            msg->signal.getPriority(),
            msg->isCommand ? "(no capsule)" : msg->destPort->slot->name,
            msg->isCommand ? "isCommand" : msg->destPort->role()->name,
            msg->isCommand ? 0 : msg->sapIndex0,
            msg->signal.getId(),
            msg->getSignalName(),
            msg->signal.getPayloadSize());

    queue[priority].enqueue(msg, front);

    notifyPtr->sendNotification();
}

// See umlrtprioritymessagequeue.hh for documentation.
bool UMLRTPriorityMessageQueue::isEmpty()
{
    bool isempty = true;

    for (UMLRTSignalElement::Priority priority = (UMLRTSignalElement::PRIORITY_MAX-1);
            (priority >= UMLRTSignalElement::PRIORITY_BACKGROUND) && (priority < (UMLRTSignalElement::PRIORITY_MAX)) && isempty; --priority)
    {
        if (!queue[priority].isEmpty())
        {
            isempty = false;
        }
    }
    return isempty;
}

void UMLRTPriorityMessageQueue::remove( UMLRTQueue::match_compare_t callback, UMLRTQueue::match_notify_t notify, void * userData )
{
    for (UMLRTSignalElement::Priority priority = (UMLRTSignalElement::PRIORITY_MAX-1);
            (priority >= UMLRTSignalElement::PRIORITY_BACKGROUND) && (priority < (UMLRTSignalElement::PRIORITY_MAX)); --priority)
    {
        queue[priority].remove(callback, notify, userData);
    }
}

// return priorityMessageQueue notifyfd[READ]
int UMLRTPriorityMessageQueue::getNotifyFd()
{
    return notifyPtr->getNotifyFd();
}

// Clear notifications of pending commands.
void UMLRTPriorityMessageQueue::clearNotifyFd()
{
    notifyPtr->clearNotifyFd();
}
