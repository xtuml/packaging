// umlrttimerprotocol.hh

/*******************************************************************************
* Copyright (c) 2014-2015 Zeligsoft (2009) Limited  and others.
* All rights reserved. This program and the accompanying materials
* are made available under the terms of the Eclipse Public License v1.0
* which accompanies this distribution, and is available at
* http://www.eclipse.org/legal/epl-v10.html
*******************************************************************************/

#ifndef UMLRTTIMERPROTOCOL_HH
#define UMLRTTIMERPROTOCOL_HH

#include "umlrtmessage.hh"
#include "umlrttimerid.hh"
#include "umlrttimespec.hh"
#include "umlrtobjectclass.hh"
#include "umlrtprotocol.hh"

struct UMLRTCommsPort;

// Protocol for timer ports.

// Timer protocol for the timer port
class UMLRTTimerProtocol
{
public:
    enum SignalId { signal_timeout };

    class InSignals {  };
    class OutSignals {
    public:
        // Generate a time-out from a clock-time.
        // User must check the returned Id for 'isValid'.
        // First variant for passing no user-defined data. Second and third variants allow a user-defined data-type.
        const UMLRTTimerId informAt( const UMLRTCommsPort * srcPort, const UMLRTTimespec & clockTime, UMLRTSignalElement::Priority priority = UMLRTSignalElement::PRIORITY_NORMAL ) const;
        const UMLRTTimerId informAt( const UMLRTCommsPort * srcPort, const UMLRTTimespec & clockTime, const void * userData, const UMLRTObject_class * type, UMLRTSignalElement::Priority priority = UMLRTSignalElement::PRIORITY_NORMAL ) const;
        const UMLRTTimerId informAt( const UMLRTCommsPort * srcPort, const UMLRTTimespec & clockTime, const UMLRTTypedValue & typedValue, UMLRTSignalElement::Priority priority = UMLRTSignalElement::PRIORITY_NORMAL ) const;


        // Generate a time-out from a relative-time.
        // User must check the returned Id for 'isValid'.
        const UMLRTTimerId informIn( const UMLRTCommsPort * srcPort, const UMLRTTimespec & relativeTime, UMLRTSignalElement::Priority priority = UMLRTSignalElement::PRIORITY_NORMAL ) const;
        const UMLRTTimerId informIn( const UMLRTCommsPort * srcPort, const UMLRTTimespec & relativeTime, void * userData, const UMLRTObject_class * type, UMLRTSignalElement::Priority priority = UMLRTSignalElement::PRIORITY_NORMAL ) const;
        const UMLRTTimerId informIn( const UMLRTCommsPort * srcPort, const UMLRTTimespec & relativeTime, const UMLRTTypedValue & typedValue, UMLRTSignalElement::Priority priority = UMLRTSignalElement::PRIORITY_NORMAL ) const;


        // Generate an interval timer that delivers one timeout signal per interval.
        // Only relative times are valid for interval timers.
        // User must check the returned Id for 'isValid'.
        const UMLRTTimerId informEvery( const UMLRTCommsPort * srcPort, const UMLRTTimespec & relativeTime, UMLRTSignalElement::Priority priority = UMLRTSignalElement::PRIORITY_NORMAL ) const;
        const UMLRTTimerId informEvery( const UMLRTCommsPort * srcPort, const UMLRTTimespec & relativeTime, void * userData, const UMLRTObject_class * type, UMLRTSignalElement::Priority priority = UMLRTSignalElement::PRIORITY_NORMAL ) const;
        const UMLRTTimerId informEvery( const UMLRTCommsPort * srcPort, const UMLRTTimespec & relativeTime, const UMLRTTypedValue & typedValue, UMLRTSignalElement::Priority priority = UMLRTSignalElement::PRIORITY_NORMAL ) const;

        // Cancel a timer. Returns true of timer was cancelled before time-out.
        bool cancelTimer( const UMLRTCommsPort * srcPort, const UMLRTTimerId id ) const;

        // Clock adjustments must be made between calls to 'timeAdjustStart()' and 'timeAdjustComplete()'
        // to ensure the timing service delivers clock-time-based timeouts correctly.
        // The resulting change in the platform clock must be computed and passed to the timer service
        // after the adjustment has been performed.
        // Timing services are suspended between these two method calls.
        void timeAdjustStart( const UMLRTCommsPort * srcPort ) const;
        void timeAdjustComplete(const UMLRTCommsPort * srcPort, const UMLRTTimespec & delta ) const;
    };

    typedef OutSignals Base;
    typedef InSignals Conjugate;

private:
    static UMLRTTimerId allocateTimer( const UMLRTCommsPort * srcPort, bool isRelative, bool isInterval, const UMLRTTimespec & due,
            UMLRTSignalElement::Priority priority, const void * userData, const UMLRTObject_class * type );
};

class UMLRTTimerProtocol_baserole : protected UMLRTProtocol, private UMLRTTimerProtocol::Base
{
public:
    UMLRTTimerProtocol_baserole( const UMLRTCommsPort *& srcPort ) : UMLRTProtocol( srcPort ) { }

    // See UMLRTTimerProtocol.
    const UMLRTTimerId informAt( const UMLRTTimespec & clockTime, UMLRTSignalElement::Priority priority = UMLRTSignalElement::PRIORITY_NORMAL ) const
    {
        return UMLRTTimerProtocol::Base::informAt( srcPort, clockTime, priority );
    }
    const UMLRTTimerId informAt( const UMLRTTimespec & clockTime, const void * userData, const UMLRTObject_class * type, UMLRTSignalElement::Priority priority = UMLRTSignalElement::PRIORITY_NORMAL) const
    {
        return UMLRTTimerProtocol::Base::informAt( srcPort, clockTime, userData, type, priority );
    }
    const UMLRTTimerId informAt( const UMLRTTimespec & clockTime, const UMLRTTypedValue typedValue, UMLRTSignalElement::Priority priority = UMLRTSignalElement::PRIORITY_NORMAL ) const
    {
        return UMLRTTimerProtocol::Base::informAt( srcPort, clockTime, typedValue, priority );
    }

    // See UMLRTTimerProtocol.
    const UMLRTTimerId informIn( const UMLRTTimespec & relativeTime, UMLRTSignalElement::Priority priority = UMLRTSignalElement::PRIORITY_NORMAL ) const
    {
        return UMLRTTimerProtocol::Base::informIn( srcPort, relativeTime, priority );
    }
    const UMLRTTimerId informIn( const UMLRTTimespec & relativeTime, void * userData, const UMLRTObject_class * type, UMLRTSignalElement::Priority priority = UMLRTSignalElement::PRIORITY_NORMAL ) const
    {
        return UMLRTTimerProtocol::Base::informIn( srcPort, relativeTime, userData, type, priority );
    }
    const UMLRTTimerId informIn( const UMLRTTimespec & relativeTime, const UMLRTTypedValue typedValue, UMLRTSignalElement::Priority priority = UMLRTSignalElement::PRIORITY_NORMAL ) const
    {
        return UMLRTTimerProtocol::Base::informIn( srcPort, relativeTime, typedValue, priority );
    }


    // See UMLRTTimerProtocol.
    const UMLRTTimerId informEvery( const UMLRTTimespec & relativeTime, UMLRTSignalElement::Priority priority = UMLRTSignalElement::PRIORITY_NORMAL ) const
    {
        return UMLRTTimerProtocol::Base::informEvery( srcPort, relativeTime, priority );
    }
    const UMLRTTimerId informEvery( const UMLRTTimespec & relativeTime, void * userData, const UMLRTObject_class * type, UMLRTSignalElement::Priority priority = UMLRTSignalElement::PRIORITY_NORMAL ) const
    {
        return UMLRTTimerProtocol::Base::informEvery( srcPort, relativeTime, userData, type, priority );
    }
    const UMLRTTimerId informEvery( const UMLRTTimespec & relativeTime, const UMLRTTypedValue typedValue, UMLRTSignalElement::Priority priority = UMLRTSignalElement::PRIORITY_NORMAL ) const
    {
        return UMLRTTimerProtocol::Base::informEvery( srcPort, relativeTime, typedValue, priority );
    }

    // See UMLRTTimerProtocol.
    bool cancelTimer( const UMLRTTimerId id ) const
    {
        return UMLRTTimerProtocol::Base::cancelTimer( srcPort, id );
    }

    // See UMLRTTimerProtocol.
    void timeAdjustStart( ) const
    {
        UMLRTTimerProtocol::Base::timeAdjustStart( srcPort );
    }
    void timeAdjustComplete( const UMLRTTimespec & delta ) const
    {
        UMLRTTimerProtocol::Base::timeAdjustComplete( srcPort, delta );
    }
};

#endif // UMLRTTIMERPROTOCOL_HH
