// umlrtslot.hh

/*******************************************************************************
* Copyright (c) 2014-2015 Zeligsoft (2009) Limited  and others.
* All rights reserved. This program and the accompanying materials
* are made available under the terms of the Eclipse Public License v1.0
* which accompanies this distribution, and is available at
* http://www.eclipse.org/legal/epl-v10.html
*******************************************************************************/

#ifndef UMLRTSLOT_HH
#define UMLRTSLOT_HH

#include <stdlib.h>
#include "umlrtsemaphore.hh"

#include "umlrtcapsuleclass.hh"
#include "umlrtcapsulerole.hh"
class UMLRTCapsule;
struct UMLRTCapsulePart;
class UMLRTController;
struct UMLRTCommsPort;


// This is the data container for information the RTS needs on each capsule
// instance in the system.
//
// It should be a plain-old data type (POD), meaning that a constructor is
// not synthesized so the instance is statically allocated and initialized.

struct UMLRTSlot
{
    const char * const name;
    const size_t capsuleIndex; // Index within a replicated capsule.

    const UMLRTCapsuleClass * capsuleClass; // Occupier's capsule's class. Follow 'role' to get model definition.
    const UMLRTCapsuleClass * containerClass; // Occupier's containing capsule type.
    const size_t roleIndex;

    UMLRTCapsule * capsule; // occupier's instance
    UMLRTController * controller; // AKA 'context'

    size_t numParts;
    const UMLRTCapsulePart * parts; // Sub-capsule parts.

    size_t  numPorts;
    const UMLRTCommsPort * ports; // Capsule's border ports. The actual capsule instance may have to use a port map to access these.

    // For plugin slots, importing defines this map during binding - deport uses this to remove connections during deport.
    int * slotToBorderMap;

    // Set true for statically allocated (generated) slots. False everywhere else.
    // Have this for sanity-check until incarnate/destroy/import/deport are soak tested.
    unsigned generated : 1;

    // Set true when the slot (or just the capsule instance in the slot) is going to be destroyed.
    // Each controller prevents injecting messages into capsules that are condemned.
    // Similarly, the send() API blocks messages that are sent to slots that are condemned.
    // Set by the controller for all sub-slots in a destruction - but reset to false for the top slot after its parts have been destroyed.
    unsigned condemned : 1;

    const UMLRTCapsuleRole * role() const
    {
        return (containerClass == NULL) ? NULL : &containerClass->subcapsuleRoles[roleIndex];
    }
};

// TODO We can remove this when incarnate/destroy, import/deport are soak-tested.
#define PORT_SEMAPHORE_WAIT_DEBUG_MSEC 10000

#endif // UMLRTSLOT_HH
