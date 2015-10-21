// umlrttimespec.cc

/*******************************************************************************
* Copyright (c) 2014-2015 Zeligsoft (2009) Limited  and others.
* All rights reserved. This program and the accompanying materials
* are made available under the terms of the Eclipse Public License v1.0
* which accompanies this distribution, and is available at
* http://www.eclipse.org/legal/epl-v10.html
*******************************************************************************/

#include "umlrttimespec.hh"
#include "umlrtobjectclassgeneric.hh"
#include "basefatal.hh"
#include "osutil.hh"
#include "ostime.hh"
#include <stdlib.h>
#include <stdio.h>

UMLRTTimespec::UMLRTTimespec( const UMLRTTimespec & tm ) : tv_sec(tm.tv_sec), tv_nsec(tm.tv_nsec) { }

UMLRTTimespec::UMLRTTimespec( long seconds, long nanoseconds ): tv_sec(seconds), tv_nsec(nanoseconds)
{
    timespecAdjust(this);
}

UMLRTTimespec::UMLRTTimespec() { UMLRTTimespec::getClock(this); }

UMLRTTimespec & UMLRTTimespec::operator=( const UMLRTTimespec & tm )
{
    tv_sec = tm.tv_sec;
    tv_nsec = tm.tv_nsec;

    return *this;
}

bool UMLRTTimespec::operator==( const UMLRTTimespec & tm ) const
{
    return (tv_sec == tm.tv_sec) && (tv_nsec == tm.tv_nsec);
}

bool UMLRTTimespec::operator!=( const UMLRTTimespec & tm ) const
{
    return (tv_sec != tm.tv_sec) || (tv_nsec != tm.tv_nsec);
}

bool UMLRTTimespec::operator<( const UMLRTTimespec & tm ) const
{
    if (tv_sec < tm.tv_sec)
        return true;

    else if (tv_sec == tm.tv_sec)
        return (tv_nsec < tm.tv_nsec);

    return false;
}

bool UMLRTTimespec::operator<=( const UMLRTTimespec & tm ) const
{
    if (tv_sec < tm.tv_sec)
        return true;

    else if (tv_sec == tm.tv_sec)
        return (tv_nsec <= tm.tv_nsec);

    return false;
}

bool UMLRTTimespec::operator>( const UMLRTTimespec & tm ) const
{
    if (tv_sec > tm.tv_sec)
            return true;

        else if (tv_sec == tm.tv_sec)
            return (tv_nsec > tm.tv_nsec);

        return false;
}

bool UMLRTTimespec::operator>=( const UMLRTTimespec & tm ) const
{
    if (tv_sec > tm.tv_sec)
        return true;

    else if (tv_sec == tm.tv_sec)
        return (tv_nsec >= tm.tv_nsec);

    return false;
}

// Ensure tv_nsec is always 0..999999999.
/*static*/ void UMLRTTimespec::timespecAdjust( UMLRTTimespec * tm )
{
    // Times must be adjusted after a single add/subtract, so tv_nsec is assumed to be
    // between -999999999 and 2*999999999 before it is adjusted.
    // We do a sanity check after the adjustment to confirm our assumptions are correct.
    if (tm->tv_nsec > ONE_BILLION)
    {
        tm->tv_sec += 1;
        tm->tv_nsec -= ONE_BILLION;
    }
    if (tm->tv_nsec < 0)
    {
        tm->tv_sec -= 1;
        tm->tv_nsec += ONE_BILLION;
    }
    // For now, we FATAL() if tm->tv_nsec was not sufficiently adjusted.
    if (tm->tv_nsec > ONE_BILLION)
    {
        FATAL("time tv_nsec(%ld) > ONE_BILLION", tm->tv_nsec);
    }
    if (tm->tv_nsec < 0)
    {
        FATAL("time tv_nsec(%ld) < 0", tm->tv_nsec);
    }
}

UMLRTTimespec UMLRTTimespec::operator+( const UMLRTTimespec & tm ) const
{
    return UMLRTTimespec(tv_sec + tm.tv_sec, tv_nsec + tm.tv_nsec);
}

UMLRTTimespec UMLRTTimespec::operator-( const UMLRTTimespec & tm ) const
{
    return UMLRTTimespec(tv_sec - tm.tv_sec, tv_nsec - tm.tv_nsec);
}

UMLRTTimespec & UMLRTTimespec::operator+=( const UMLRTTimespec & tm )
{
    tv_sec += tm.tv_sec;
    tv_nsec += tm.tv_nsec;

    timespecAdjust(this);

    return *this;
}

UMLRTTimespec & UMLRTTimespec::operator-=( const UMLRTTimespec & tm )
{
    tv_sec -= tm.tv_sec;
    tv_nsec -= tm.tv_nsec;

    timespecAdjust(this);

    return *this;
}

bool UMLRTTimespec::isZeroOrNegative() const
{
    return (tv_sec < 0) || ((tv_sec == 0) && (tv_nsec == 0));
}

char * UMLRTTimespec::toString( char * buffer, int size ) const
{
    struct tm from_localtime;
    const time_t seconds = tv_sec;

    localtime_r( &seconds, &from_localtime );

    size_t length = strftime(buffer, size, "%Y-%m-%d:%H:%M:%S", &from_localtime);
    long msec = tv_nsec / NANOSECONDS_PER_MILLISECOND;
    snprintf( &buffer[length], size-length, ".%03ld", msec );

    return buffer;
}

char * UMLRTTimespec::toStringRelative( char * buffer, int size ) const
{
    long int seconds = tv_sec;
    long int days = seconds / SECONDS_PER_DAY;
    seconds -= (days * SECONDS_PER_DAY);
    long int hours = seconds / SECONDS_PER_HOUR;
    seconds -= (hours * SECONDS_PER_HOUR);
    long int minutes = seconds / SECONDS_PER_MINUTE;
    seconds -= (minutes * SECONDS_PER_MINUTE);

    long int msec = tv_nsec / NANOSECONDS_PER_MILLISECOND;

    snprintf( buffer, size, "%ld:%02ld:%02ld:%02ld.%03ld", days, hours, minutes, seconds, msec );

    return buffer;
}

static int UMLRTObject_fprintf_UMLRTTimespec( FILE *ostream, const UMLRTObject_class * desc, const void * data, int nest, int arraySize )
{
    char buf[UMLRTTimespec::TIMESPEC_TOSTRING_SZ];
    int nchar = 0;
    const UMLRTTimespec * array = (const UMLRTTimespec *)data;
    for (int i = 0; i < arraySize; ++i)
    {
        nchar += fprintf(ostream, "{%s", desc->name);
        if (arraySize > 1)
        {
            nchar += fprintf(ostream, "[%d]", i);
        }
        nchar += fprintf(ostream, " %s}", array[i].toString(buf, sizeof(buf)));
    }
    return nchar;
}

static const UMLRTObject_class UMLRTType_UMLRTTimespec_
= {
        "UMLRTTimespec",
        UMLRTObjectInitialize<UMLRTTimespec>,
        UMLRTObjectCopy<UMLRTTimespec>,
        UMLRTObject_decode,
        UMLRTObject_encode,
        UMLRTObjectDestroy<UMLRTTimespec>,
        UMLRTObject_getSize,
        UMLRTObject_fprintf_UMLRTTimespec,
        NULL, // super
        UMLRTOBJECTCLASS_DEFAULT_VERSION, // version
        UMLRTOBJECTCLASS_DEFAULT_BACKWARDS, // backwards
        sizeof(UMLRTTimespec),
        sizeof(UMLRTTimespec),
        NULL, // fields
};

const UMLRTObject_class * const UMLRTType_UMLRTTimespec = &UMLRTType_UMLRTTimespec_;
