// ostime.cc

/*******************************************************************************
 * Copyright (c) 2015 Zeligsoft (2009) Limited  and others.
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * which accompanies this distribution, and is available at
 * http://www.eclipse.org/legal/epl-v10.html
 *******************************************************************************/

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <stdint.h>
#include "ostime.hh"

/*static*/int OSTime::get_timeofday(struct timeval * tv)
{
    static const uint64_t EPOCH = ((uint64_t) 116444736000000000ULL);
    int result = 0;

    SYSTEMTIME system_time;
    FILETIME file_time;
    uint64_t time;

    GetSystemTime(&system_time);
    if (SystemTimeToFileTime(&system_time, &file_time) == 0)
    {
        result = -1;
    }
    else
    {
        time = ((uint64_t) file_time.dwLowDateTime);
        time += ((uint64_t) file_time.dwHighDateTime) << 32;

        tv->tv_sec = (long) ((time - EPOCH) / 10000000L);
        tv->tv_usec = (long) (system_time.wMilliseconds * 1000);
    }

    return result;
}

/*static*/int OSTime::clock_gettime(struct timespec * ts)
{
    LARGE_INTEGER t;
    FILETIME f;
    double microseconds;
    static LARGE_INTEGER offset;
    static double frequencyToMicroseconds;
    static int initialized = 0;

    if (!initialized)
    {
        LARGE_INTEGER performanceFrequency;
        initialized = 1;
        QueryPerformanceFrequency(&performanceFrequency);
        QueryPerformanceCounter(&offset);
        frequencyToMicroseconds = (double) performanceFrequency.QuadPart / 1000000.;
    }

    QueryPerformanceCounter(&t);
    t.QuadPart -= offset.QuadPart;
    microseconds = (double) t.QuadPart / frequencyToMicroseconds;
    t.QuadPart = (long long) microseconds;
    ts->tv_sec = (long) t.QuadPart / 1000000;
    ts->tv_nsec = (t.QuadPart % 1000000) * 1000;
    return (0);
}
