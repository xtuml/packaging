// umlrtcapsuleid.hh

/*******************************************************************************
* Copyright (c) 2014-2015 Zeligsoft (2009) Limited  and others.
* All rights reserved. This program and the accompanying materials
* are made available under the terms of the Eclipse Public License v1.0
* which accompanies this distribution, and is available at
* http://www.eclipse.org/legal/epl-v10.html
*******************************************************************************/

#ifndef UMLRTCAPSULEID_HH
#define UMLRTCAPSULEID_HH

#include "umlrtobjectclass.hh"
#include <stdlib.h>
#include <stdio.h>

// This is a 'handle' returned when a dynamic capsule is incarnated.
// The user must check #isValid to confirm the capsule got created without error.

class UMLRTCapsule;

class UMLRTCapsuleId
{
public:
    UMLRTCapsuleId( UMLRTCapsule * capsule_ = NULL ) : capsule(capsule_) { }

    bool isValid() const { return capsule != NULL; }

    // Must make sure #isValid before calling #getCapsule.
    UMLRTCapsule * getCapsule() const;

private:
    UMLRTCapsule * capsule;

};

extern const UMLRTObject_class * const UMLRTType_UMLRTCapsuleId;

#endif // UMLRTCAPSULEID_HH
