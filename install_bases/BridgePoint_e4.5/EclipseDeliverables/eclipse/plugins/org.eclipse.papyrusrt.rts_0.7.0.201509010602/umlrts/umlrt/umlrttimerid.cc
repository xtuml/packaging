// umlrttimerid.cc

/*******************************************************************************
* Copyright (c) 2014-2015 Zeligsoft (2009) Limited  and others.
* All rights reserved. This program and the accompanying materials
* are made available under the terms of the Eclipse Public License v1.0
* which accompanies this distribution, and is available at
* http://www.eclipse.org/legal/epl-v10.html
*******************************************************************************/

#include "umlrttimer.hh"
#include "umlrttimerid.hh"
#include "basefatal.hh"

// Must have previously confirmed the timer #isValid.
const UMLRTTimer * UMLRTTimerId::getTimer() const
{
    if (!timer)
    {
        FATAL("timer is invalid");
    }
    return timer;
}
