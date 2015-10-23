// ostime.hh

/*******************************************************************************
 * Copyright (c) 2015 Zeligsoft (2009) Limited  and others.
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * which accompanies this distribution, and is available at
 * http://www.eclipse.org/legal/epl-v10.html
 *******************************************************************************/

#ifndef OSTIME_HH
#define OSTIME_HH

#include <winsock2.h>  // struct timeval
#include <time.h>

#define localtime_r(A, B) localtime_s(B, A)

struct timespec {
    time_t   tv_sec;        /* seconds */
    long     tv_nsec;       /* nanoseconds */
};

class OSTime
{
public:
    static int get_timeofday(struct timeval * tv);

    static int clock_gettime(struct timespec * ts);
};

#endif // OSTIME_HH
