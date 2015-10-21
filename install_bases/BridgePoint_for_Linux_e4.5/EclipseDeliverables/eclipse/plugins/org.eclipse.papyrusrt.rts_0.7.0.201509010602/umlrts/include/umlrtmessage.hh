// umlrtmessage.hh

/*******************************************************************************
* Copyright (c) 2014-2015 Zeligsoft (2009) Limited  and others.
* All rights reserved. This program and the accompanying materials
* are made available under the terms of the Eclipse Public License v1.0
* which accompanies this distribution, and is available at
* http://www.eclipse.org/legal/epl-v10.html
*******************************************************************************/

#ifndef UMLRTMESSAGE_HH
#define UMLRTMESSAGE_HH

#include <stdint.h>
#include "umlrtqueueelement.hh"
#include "umlrtsignal.hh"

struct UMLRTCommsPort;
class UMLRTSignal;
struct UMLRTSlot;

// The RTS library transports signals within 'messages' (UMLRTMessage).

// Messages are small and are fixed in size. The 'message' is the
// entity that is queued in 'message queues' waiting to be consumed by their
// destination capsules.

// Each message contains a reference to a single signal. The signal's payload is the
// (optional) user data added to the signal. (See UMLRTSignal for more info.)

// Signals are separated from messages so that one signal can reside in
// multiple messages (to support broadcast operations).

class UMLRTMessage : public UMLRTQueueElement
{
public:

    UMLRTMessage ( ) : srcPortIndex(0), sapIndex0(0), destPort(NULL), destSlot(NULL), desc(NULL), decodeInfo(NULL), isCommand(false), allocated(false) {};

    size_t srcPortIndex; // The associated srcPort of the message is contained within the signal.
    size_t sapIndex0; // The port index on the receive side.

    const UMLRTCommsPort * destPort; // Message destination - capsule contained within.
    const UMLRTSlot * destSlot; // Destination slot.

    UMLRTSignal signal;

    mutable const UMLRTObject_class * desc;
    mutable const void * decodeInfo;

    unsigned isCommand : 1;   // true when it's a command and not a signal.
    unsigned allocated : 1;   // For sanity checking of message allocation.

    bool defer ( ) const;

    const char * getSignalName ( ) const { return signal.getName(); }
};


#endif // UMLRTMESSAGE_HH
