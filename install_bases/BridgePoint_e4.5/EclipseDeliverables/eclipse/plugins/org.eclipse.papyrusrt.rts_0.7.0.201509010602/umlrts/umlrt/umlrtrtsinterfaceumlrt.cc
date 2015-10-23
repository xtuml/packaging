// umlrtrtsinterfaceumlrt.cc

/*******************************************************************************
* Copyright (c) 2015 Zeligsoft (2009) Limited  and others.
* All rights reserved. This program and the accompanying materials
* are made available under the terms of the Eclipse Public License v1.0
* which accompanies this distribution, and is available at
* http://www.eclipse.org/legal/epl-v10.html
*******************************************************************************/

#include "umlrtcommsport.hh"
#include "umlrtcommsportfarend.hh"
#include "umlrtframeservice.hh"
#include "umlrtrtsinterfaceumlrt.hh"

void UMLRTRtsInterfaceUmlrt::connectPorts ( const UMLRTCommsPort * p1, size_t p1Index, const UMLRTCommsPort * p2, size_t p2Index ) const
{
    UMLRTFrameService::connectPorts(p1, p1Index, p2, p2Index);
}

void UMLRTRtsInterfaceUmlrt::connectRelayPort ( const UMLRTCommsPort * relayPort, size_t relayIndex, const UMLRTCommsPort * destPort, size_t destIndex ) const
{
    UMLRTFrameService::connectRelayPort(relayPort, relayIndex, destPort, destIndex);
}

const UMLRTCommsPort * * UMLRTRtsInterfaceUmlrt::createBorderPorts ( UMLRTSlot * slot, size_t numPorts ) const
{
    return UMLRTFrameService::createBorderPorts(slot, numPorts);
}

const UMLRTCommsPort * * UMLRTRtsInterfaceUmlrt::createInternalPorts ( UMLRTSlot * slot, const UMLRTCapsuleClass * capsuleClass ) const
{
    return UMLRTFrameService::createInternalPorts(slot, capsuleClass);
}

void UMLRTRtsInterfaceUmlrt::bindPort ( const UMLRTCommsPort * * ports, int portId, int farEndIndex ) const
{
    UMLRTFrameService::sendBoundUnbound(ports, portId, farEndIndex, true/*isBind*/);
}

void UMLRTRtsInterfaceUmlrt::bindSubcapsulePort ( bool isBorder, UMLRTCapsule * subcapsule, int portIndex, int farEndIndex ) const
{
    UMLRTFrameService::bindSubcapsulePort(isBorder, subcapsule, portIndex, farEndIndex);
}

void UMLRTRtsInterfaceUmlrt::unbindPort ( const UMLRTCommsPort * * ports, int portId, int farEndIndex ) const
{
    UMLRTFrameService::sendBoundUnbound(ports, portId, farEndIndex, false/*isBind*/);
    UMLRTFrameService::disconnectPort(ports[portId], farEndIndex);
}

void UMLRTRtsInterfaceUmlrt::unbindSubcapsulePort ( bool isBorder, UMLRTCapsule * subcapsule, int portIndex, int farEndIndex ) const
{
    UMLRTFrameService::unbindSubcapsulePort(isBorder, subcapsule, portIndex, farEndIndex);
}
