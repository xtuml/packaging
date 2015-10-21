// ostimespec.cc

/*******************************************************************************
 * Copyright (c) 2015 Zeligsoft (2009) Limited  and others.
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * which accompanies this distribution, and is available at
 * http://www.eclipse.org/legal/epl-v10.html
 *******************************************************************************/

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <time.h>
#include <sstream>
#include <iomanip>
#include "basefatal.hh"
#include "ostime.hh"
#include "umlrttimespec.hh"

/*static*/
void UMLRTTimespec::getClock(UMLRTTimespec * tm)
{
    struct timeval tv;

    if (OSTime::get_timeofday(&tv) < 0)
    {
        FATAL_ERRNO("get_timeofday");
    }
    tm->tv_sec = tv.tv_sec;
    tm->tv_nsec = tv.tv_usec * 1000;
}

/*static*/
void UMLRTTimespec::timespecAbsAddMsec(struct timespec * timeout, long msec)
{
    OSTime::clock_gettime(timeout);
    timeout->tv_sec += (msec / 1000);
    timeout->tv_nsec += (msec % 1000) * ONE_MILLION;
    if (timeout->tv_nsec >= ONE_BILLION)
    {
        timeout->tv_sec += 1;
        timeout->tv_nsec -= ONE_BILLION;
    }
}


