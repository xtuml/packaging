// umlrtcapsule.cc

/*******************************************************************************
* Copyright (c) 2015 Zeligsoft (2009) Limited  and others.
* All rights reserved. This program and the accompanying materials
* are made available under the terms of the Eclipse Public License v1.0
* which accompanies this distribution, and is available at
* http://www.eclipse.org/legal/epl-v10.html
*******************************************************************************/

#include "umlrtcapsuleclass.hh"
#include "umlrtcapsule.hh"
#include "umlrtframeservice.hh"
#include "basedebug.hh"
#include "basedebugtype.hh"

UMLRTCapsule::~UMLRTCapsule ( )
{
    BDEBUG(BD_INSTANTIATE, "%s destructor\n", slot->name);
    UMLRTCapsuleToControllerMap::removeCapsule(slot->name, this);
}

UMLRTCapsule::UMLRTCapsule ( const UMLRTRtsInterface * rtsif_, const UMLRTCapsuleClass * capsuleClass_, UMLRTSlot * slot, const UMLRTCommsPort * * borderPorts_, const UMLRTCommsPort * * internalPorts_, bool isStatic_ ) : msg(NULL), rtsif(rtsif_), capsuleClass(capsuleClass_), slot(slot), borderPorts(borderPorts_), internalPorts(internalPorts_), isStatic(isStatic_)
{
    BDEBUG(BD_INSTANTIATE, "slot %s constructor\n", slot->name);
    UMLRTCapsuleToControllerMap::addCapsule(slot->name, this);
}

void UMLRTCapsule::bindPort ( bool isBorder, int portIndex, int farEndIndex )
{
}

void UMLRTCapsule::unbindPort ( bool isBorder, int portIndex, int farEndIndex )
{
}

void UMLRTCapsule::unexpectedMessage ( ) const
{
    fprintf( stderr, "capsule %s(role %s) unexpected message on port %s protocol %s signal id %d\n",
            name(), getName(), getMsg()->destPort->role()->name, getMsg()->destPort->role()->protocol, getMsg()->signal.getId());
}
