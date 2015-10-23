// umlrttimerid.hh

/*******************************************************************************
* Copyright (c) 2014-2015 Zeligsoft (2009) Limited  and others.
* All rights reserved. This program and the accompanying materials
* are made available under the terms of the Eclipse Public License v1.0
* which accompanies this distribution, and is available at
* http://www.eclipse.org/legal/epl-v10.html
*******************************************************************************/

#ifndef UMLRTTIMERID_HH
#define UMLRTTIMERID_HH

#include <stdlib.h>

// This is a 'handle' returned when a timer is created and can be used to cancel a timer.
// User must check 'isValid' to confirm the timer got created ok.

struct UMLRTTimer;
class UMLRTTimerProtocol;
class UMLRTTimerQueue;

class UMLRTTimerId
{
public:
    UMLRTTimerId( ) : timer(NULL) { }

    UMLRTTimerId( UMLRTTimer * timer_ ) : timer(timer_) { }

    bool isValid( ) const { return timer != NULL; }

    // #isValid must be checked before getting the timer.
    const UMLRTTimer * getTimer( ) const;

private:

    const UMLRTTimer * timer;

};
#endif // UMLRTTIMERID_HH
