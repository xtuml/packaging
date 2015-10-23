// umlrtcommsport.hh

/*******************************************************************************
* Copyright (c) 2014-2015 Zeligsoft (2009) Limited  and others.
* All rights reserved. This program and the accompanying materials
* are made available under the terms of the Eclipse Public License v1.0
* which accompanies this distribution, and is available at
* http://www.eclipse.org/legal/epl-v10.html
*******************************************************************************/

#ifndef UMLRTCOMMSPORT_HH
#define UMLRTCOMMSPORT_HH

#include <stdlib.h>

#include "umlrtapi.hh"
#include "umlrtcapsuleclass.hh"
#include "umlrtcommsportrole.hh"
#include "umlrtmessagequeue.hh"
#include "umlrtslot.hh"

struct UMLRTSlot;
struct UMLRTCommsPortFarEnd;

// This is the port information used by the run-time.

struct UMLRTCommsPort
{
    const UMLRTCapsuleClass * containerClass;
    size_t roleIndex;
    UMLRTSlot * slot;

    size_t numFarEnd;
    UMLRTCommsPortFarEnd * farEnds; // List size > 1 for replicated ports.

    mutable UMLRTMessageQueue * deferQueue; // Deferred messages on this port.
    mutable char * registeredName;

    unsigned automatic : 1; // True if the port should be registered as SAP/SPP at startup or during creation.
    unsigned border : 1; // True for a border port. Used to consult the correct role list when fetching the port's role.
    unsigned generated : 1; // True for code-generated ports (registeredName is not from heap).
    unsigned locked : 1; // True if the port is application-locked.
    mutable unsigned notification : 1; // True when user requested binding notification.
    unsigned proxy : 1; // True for proxy border ports created if the slot port replication is less than the capsule border port replication.
    mutable unsigned relay : 1; // True if the port is a relay port, as indicated at run-time by the capsule instance callback.
    unsigned sap : 1; // True if the port is an SAP.
    unsigned spp : 1; // True if the port is an SPP.
    unsigned unbound : 1; // True to represent the unbound port. Has no far-end instances and is replaced when binding.
    unsigned wired : 1; // True for wired ports. Used for rtBound/rtUnbound notifications.

    // Struct passed to queue-remove routine on deferQueue for purge/recall operations.
    // Selects messages from deferQueue and specifies behaviour of the purge/recall of each matched message.
    struct PurgeRecall
    {
        int index;
        bool front;
        int id;
    };

    static bool purgeMatchCompare( UMLRTMessage * msg, const PurgeRecall * purge );
    static void purgeMatchNotify( UMLRTMessage * msg, const PurgeRecall * purge );

    // 'index' is the port far-end instance (-1 for all instances). 'id' is the signal id being purged (-1 for all.)
    int purge( int index = -1, int id = -1 ) const;

    static bool recallMatchCompare( UMLRTMessage * msg, const PurgeRecall * recall );
    static void recallMatchNotify( UMLRTMessage * msg, const PurgeRecall * recall );

    // 'index' is the port far-end instance destination of recalled message (-1 for all instances.)
    // 'front' is true if messages are being recalled to front of capsule queue (false to queue messages on tail of queue.)
    // 'one' is true if only a signal message (matching the criteria) are recalled (false for 'all' messages.)
    int recall( int index = -1, bool front = false, bool one = false, int id = -1  ) const;

    const UMLRTCommsPortRole * role() const;
};

#endif // UMLRTCOMMSPORT_HH
