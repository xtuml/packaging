// umlrttimerpool.hh

/*******************************************************************************
 * Copyright (c) 2014-2015 Zeligsoft (2009) Limited  and others.
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * which accompanies this distribution, and is available at
 * http://www.eclipse.org/legal/epl-v10.html
 *******************************************************************************/

#ifndef UMLRTTIMERPOOL_HH
#define UMLRTTIMERPOOL_HH

#include "umlrtpool.hh"
#include "umlrttimer.hh"
#include "umlrtuserconfig.hh"

// UMLRTTimerPool is a pool of free timers.

// The initial version has a single application-wide free-pool used by all
// threads for all timers.

class UMLRTTimerPool: public UMLRTPool
{
public:
    UMLRTTimerPool(UMLRTTimer timerElements[], size_t arraySize, size_t incrementSize =
            USER_CONFIG_TIMER_POOL_INCR);

    UMLRTTimerPool(size_t incrementSize = USER_CONFIG_TIMER_POOL_INCR);

private:
    virtual void grow();
};

#endif // UMLRTTIMERPOOL_HH
