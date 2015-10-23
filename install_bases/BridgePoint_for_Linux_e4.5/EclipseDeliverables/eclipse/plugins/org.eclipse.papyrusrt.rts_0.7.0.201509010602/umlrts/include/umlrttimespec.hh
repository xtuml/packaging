// umlrttimespec.hh

/*******************************************************************************
* Copyright (c) 2014-2015 Zeligsoft (2009) Limited  and others.
* All rights reserved. This program and the accompanying materials
* are made available under the terms of the Eclipse Public License v1.0
* which accompanies this distribution, and is available at
* http://www.eclipse.org/legal/epl-v10.html
*******************************************************************************/

#ifndef UMLRTTIMESPEC_HH
#define UMLRTTIMESPEC_HH

#include "umlrtobjectclass.hh"
#include <time.h>

// Platform independent time-value.

// Either clock-time or a relative time can be stored in these.

// tv_nsec is always positive [0..999999999], so "-1 nanoseconds" is [-1 second + 999999999 nanoseconds].

class UMLRTTimespec
{
public:
    UMLRTTimespec();

    UMLRTTimespec( const UMLRTTimespec & tm );
    UMLRTTimespec( long seconds, long nanoseconds );
    ~UMLRTTimespec() { }

    UMLRTTimespec & operator=( const UMLRTTimespec & tm );

    bool operator==( const UMLRTTimespec & tm ) const;
    bool operator!=( const UMLRTTimespec & tm ) const;
    bool operator<( const UMLRTTimespec & tm ) const;
    bool operator<=( const UMLRTTimespec & tm ) const;
    bool operator>( const UMLRTTimespec & tm ) const;
    bool operator>=( const UMLRTTimespec & tm ) const;

    UMLRTTimespec operator+( const UMLRTTimespec & tm ) const;
    UMLRTTimespec & operator+=( const UMLRTTimespec & tm );
    UMLRTTimespec operator-( const UMLRTTimespec & tm ) const;
    UMLRTTimespec & operator-=( const UMLRTTimespec & tm );

    // Return true if the time specification is zero or negative.
    bool isZeroOrNegative() const;

    static void getClock( UMLRTTimespec * tm );

    long tv_sec;
    long tv_nsec; // Nanoseconds (one-billionths of a second). Always < one billion. Always >= 0.

    static const long ONE_BILLION = 1000000000L; // Used for time calculations.
    static const long ONE_MILLION = 1000000L;    // Used for time calculations.
    static const long SECONDS_PER_DAY = (60*60*24);
    static const long SECONDS_PER_HOUR = (60*60);
    static const long SECONDS_PER_MINUTE = (60);
    static const long NANOSECONDS_PER_MILLISECOND = (1000000);

    static void timespecAbsAddMsec( struct timespec * timeout, long msec );

    static const int TIMESPEC_TOSTRING_SZ = 64;
    char * toString( char * buf, int size ) const;
    char * toStringRelative( char * buf, int size ) const;

private:
    static void timespecAdjust( UMLRTTimespec * tm );
};

extern const UMLRTObject_class * const UMLRTType_UMLRTTimespec;

#endif // UMLRTTIMESPEC_HH
