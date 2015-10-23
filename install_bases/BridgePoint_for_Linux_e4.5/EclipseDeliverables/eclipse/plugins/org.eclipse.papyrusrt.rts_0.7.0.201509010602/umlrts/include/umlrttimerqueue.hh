// umlrttimerqueue.hh

/*******************************************************************************
* Copyright (c) 2014-2015 Zeligsoft (2009) Limited  and others.
* All rights reserved. This program and the accompanying materials
* are made available under the terms of the Eclipse Public License v1.0
* which accompanies this distribution, and is available at
* http://www.eclipse.org/legal/epl-v10.html
*******************************************************************************/

#ifndef UMLRTTIMERQUEUE_HH
#define UMLRTTIMERQUEUE_HH

#include "umlrtqueue.hh"
#include "umlrttimespec.hh"
#include "umlrttimerid.hh"

// Queue of running timers.

struct UMLRTTimer;
class UMLRTNotify;

class UMLRTTimerQueue : public UMLRTQueue
{
public:
    // Create an empty queue.
    UMLRTTimerQueue();

    virtual ~UMLRTTimerQueue();

    // Remove the first timer on the queue. Returns NULL if none or the first timer has not yet expired.
    UMLRTTimer * dequeue();

    // Add a timer to the queue in order of when they will expire.
    void enqueue( const UMLRTTimer * timer );

    // Cancel a timer (remove from the queue). Return true if ok.
    bool cancel( UMLRTTimerId id );

    // Calculate how much time left before timer on the head of the queue is due.
    UMLRTTimespec timeRemaining() const;

    // return umlrtnotify fd[READ]
    int getNotifyFd();

    // Clear notifications of pending commands.
    void clearNotifyFd();

private:

    UMLRTNotify * notifyPtr;
};

#endif // UMLRTTIMERQUEUE_HH
