// ostimespec.cc

/*******************************************************************************
* Copyright (c) 2015 Zeligsoft (2009) Limited  and others.
* All rights reserved. This program and the accompanying materials
* are made available under the terms of the Eclipse Public License v1.0
* which accompanies this distribution, and is available at
* http://www.eclipse.org/legal/epl-v10.html
*******************************************************************************/

#include <sys/time.h>
#include <time.h>
#include "basefatal.hh"
#include <stdio.h>
#include "umlrttimespec.hh"

/*static*/ void UMLRTTimespec::getClock( UMLRTTimespec * tm )
{
    struct timeval tv;

    if (gettimeofday(&tv, NULL) < 0)
    {
        FATAL_ERRNO("gettimeofday");
    }
    tm->tv_sec = tv.tv_sec;
    tm->tv_nsec = tv.tv_usec * 1000;
}

/*static*/ void UMLRTTimespec::timespecAbsAddMsec( struct timespec * timeout, long msec )
{
    clock_gettime(CLOCK_REALTIME, timeout);
    timeout->tv_sec += (msec / 1000);
    timeout->tv_nsec += (msec % 1000) * ONE_MILLION;
    if (timeout->tv_nsec >= ONE_BILLION)
    {
        timeout->tv_sec += 1;
        timeout->tv_nsec -= ONE_BILLION;
    }
}

