// umlrttimer.hh

/*******************************************************************************
* Copyright (c) 2014-2015 Zeligsoft (2009) Limited  and others.
* All rights reserved. This program and the accompanying materials
* are made available under the terms of the Eclipse Public License v1.0
* which accompanies this distribution, and is available at
* http://www.eclipse.org/legal/epl-v10.html
*******************************************************************************/

#ifndef UMLRTTIMER_HH
#define UMLRTTIMER_HH

#include "umlrtcommsport.hh"
#include "umlrtmessage.hh"
#include "umlrtqueueelement.hh"
#include "umlrtoutsignal.hh"
#include "umlrttimespec.hh"

// An RTS timer.

// Timers are kept in a 'pool' and are allocated when required.

// The 'due' time is the clock-time that the timer is set to expire.

// If a timer is an interval-timer, it has an 'interval' time and is restarted
// by adding the 'interval' to the 'due' time (making that the new 'due' time).

// Timers either reside in the system pool (and are available for applications) or are
// queued on a controller's timerQueue.

struct UMLRTTimer : public UMLRTQueueElement
{
    UMLRTTimer() : isInterval(false), destPort(NULL), allocated(false) {}

    UMLRTTimespec due;
    UMLRTTimespec interval;
    bool isInterval;

    UMLRTOutSignal signal;
    const UMLRTCommsPort * destPort;
    const UMLRTSlot * destSlot;
    bool allocated; // For sanity-checking.
};

#endif // UMLRTTIMER_HH
