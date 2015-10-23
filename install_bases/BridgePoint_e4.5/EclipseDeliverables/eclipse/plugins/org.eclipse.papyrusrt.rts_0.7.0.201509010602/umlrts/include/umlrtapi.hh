// umlrtapi.hh - catch-all RTS API called from everywhere
// Definitions here can be placed elsewhere at a later date...

/*******************************************************************************
* Copyright (c) 2014-2015 Zeligsoft (2009) Limited  and others.
* All rights reserved. This program and the accompanying materials
* are made available under the terms of the Eclipse Public License v1.0
* which accompanies this distribution, and is available at
* http://www.eclipse.org/legal/epl-v10.html
*******************************************************************************/

#include <stdlib.h>

struct UMLRTCommsPort;
struct UMLRTDynamicCapsuleDescriptor;
class UMLRTMessage;
class UMLRTSignal;
class UMLRTSignalElement;
struct UMLRTTimer;

namespace umlrt
{
    // Get a signal from the application-wide pool.
    UMLRTSignalElement * SignalElementGetFromPool();

    // Return a signal back to the pool.
    void SignalElementPutToPool( UMLRTSignalElement * signal );

    // Get a message from the application-wide pool.
    UMLRTMessage * MessageGetFromPool();

    // Put a message back on the system-wide pool.
    void MessagePutToPool( UMLRTMessage * message );

    // Get a timer from the application-wide pool.
    UMLRTTimer * TimerGetFromPool();

    // Put a timer back on the system-wide pool.
    void TimerPutToPool( UMLRTTimer * timer );
}
