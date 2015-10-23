// umlrtcapsuleclass.hh

/*******************************************************************************
* Copyright (c) 2014-2015 Zeligsoft (2009) Limited  and others.
* All rights reserved. This program and the accompanying materials
* are made available under the terms of the Eclipse Public License v1.0
* which accompanies this distribution, and is available at
* http://www.eclipse.org/legal/epl-v10.html
*******************************************************************************/

#include <stdlib.h>

#ifndef UMLRTCAPSULECLASS_HH
#define UMLRTCAPSULECLASS_HH

class UMLRTCapsule;
struct UMLRTSlot;
class UMLRTCapsuleId;
struct UMLRTCapsulePart;
struct UMLRTCapsuleRole;
struct UMLRTCommsPort;
struct UMLRTCommsPortRole;
struct UMLRTCommsPortMap;
class UMLRTRtsInterface;


// This struct captures information needed to validate and execution incarnation on a dynamic capsule.

// An instance of this class is generated for each capsule type defined by the user.
// The 'super' is non-NULL if the capsule type is inherited.
// The 'capsuleParts' point to information about the capsule's sub-capsules.

struct UMLRTCapsuleClass
{
    const char * const name; // Type name, not the instance name.

    const UMLRTCapsuleClass * const super; // Inherited type

    void ( * const instantiate )( const UMLRTRtsInterface * rtsif, UMLRTSlot * slot, const UMLRTCommsPort * * borderPorts );

    const size_t numSubcapsuleRoles;
    const UMLRTCapsuleRole * const subcapsuleRoles; // Contained

    const size_t numPortRolesBorder;
    const UMLRTCommsPortRole * const portRolesBorder; // Border ports

    const size_t numPortRolesInternal;
    const UMLRTCommsPortRole * const portRolesInternal; // Internal ports

};

#endif // UMLRTCAPSULECLASS_HH
