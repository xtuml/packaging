// umlrtrtsinterfaceumlrt.hh

/*******************************************************************************
* Copyright (c) 2015 Zeligsoft (2009) Limited  and others.
* All rights reserved. This program and the accompanying materials
* are made available under the terms of the Eclipse Public License v1.0
* which accompanies this distribution, and is available at
* http://www.eclipse.org/legal/epl-v10.html
*******************************************************************************/

#ifndef UMLRTRTSINTERFACEUMLRT_HH
#define UMLRTRTSINTERFACEUMLRT_HH

#include "umlrtrtsinterface.hh"

class UMLRTCapsule;
struct UMLRTCapsuleClass;
struct UMLRTCommsPort;
struct UMLRTSlot;

// This class is instantiated but not used by the current implementation - except in test models supported internally.
// The RTS-interface from generated capsule code is due to be refactored.

class UMLRTRtsInterfaceUmlrt : public UMLRTRtsInterface
{
public:
    // Used to connect two ports. If the far-end of 'p1' already exists, it is the far-end of 'p1' that gets connected to 'p2'.
    virtual void connectPorts ( const UMLRTCommsPort * p1, size_t p1Index, const UMLRTCommsPort * p2, size_t p2Index ) const;

    // Create a connection between the far end of the 'relay' port to the 'dest' port. The relay port itself is not modified.
    virtual void connectRelayPort ( const UMLRTCommsPort * relayPort, size_t relayIndex, const UMLRTCommsPort * destPort, size_t destIndex ) const;

    // Used for creating a border-port-list from a slot, passed to a capsule instantiate function.
    virtual const UMLRTCommsPort * * createBorderPorts( UMLRTSlot * slot, size_t numPorts ) const;

    // Create a internal port list. No ports are connected yet.
    virtual const UMLRTCommsPort * * createInternalPorts ( UMLRTSlot * slot, const UMLRTCapsuleClass * capsuleClass ) const;

    // The capsule is declaring the port as 'bound'. The port is not a relay port.
    virtual void bindPort ( const UMLRTCommsPort * * ports, int portId, int index ) const;

    // The capsule is binding the port to a sub-capsule port. The port a relay port.
    virtual void bindSubcapsulePort ( bool isBorder, UMLRTCapsule * subcapsule, int portIndex, int farEndIndex ) const;

    // The capsule is unbinding its port. The port is not a relay port.
    virtual void unbindPort ( const UMLRTCommsPort * * ports, int portId, int index ) const;

    // The capsule is unbinding the port from a sub-capsule port. The port a relay port.
    virtual void unbindSubcapsulePort ( bool isBorder, UMLRTCapsule * subcapsule, int portIndex, int farEndIndex ) const;

};

#endif // UMLRTRTSINTERFACEUMLRT_HH
