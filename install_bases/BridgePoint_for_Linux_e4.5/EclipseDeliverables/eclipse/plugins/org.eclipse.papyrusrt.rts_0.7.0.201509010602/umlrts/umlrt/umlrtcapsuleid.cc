// umlrtcapsuleid.c

/*******************************************************************************
* Copyright (c) 2014-2015 Zeligsoft (2009) Limited  and others.
* All rights reserved. This program and the accompanying materials
* are made available under the terms of the Eclipse Public License v1.0
* which accompanies this distribution, and is available at
* http://www.eclipse.org/legal/epl-v10.html
*******************************************************************************/

#include "umlrtcapsule.hh"
#include "umlrtcapsuleid.hh"
#include "umlrtobjectclassgeneric.hh"
#include "basefatal.hh"
#include <stdlib.h>
#include <new>

// Must have verified #isValid before calling this.
UMLRTCapsule * UMLRTCapsuleId::getCapsule() const
{
    if (!capsule)
    {
        FATAL("no capsule defined");
    }
    return capsule;
}

static int UMLRTObject_fprintf_UMLRTCapsuleId( FILE *ostream, const UMLRTObject_class * desc, const void * data, int nest, int arraySize )
{
    int nchar = 0;
    const UMLRTCapsuleId *array = (const UMLRTCapsuleId *)data;
    for (int i = 0; i < arraySize; ++i)
    {
        nchar += fprintf(ostream, "{UMLRTCapsuleId");
        if (arraySize > 1)
        {
            nchar += fprintf(ostream, "[%d]", i);
        }
        nchar += fprintf(ostream, " %s}", array[i].getCapsule()->name());
    }
    return nchar;
}

static const UMLRTObject_class UMLRTType_UMLRTCapsuleId_
= {
        "UMLRTCapsuleId",
        UMLRTObjectInitialize<UMLRTCapsuleId>,
        UMLRTObjectCopy<UMLRTCapsuleId>,
        UMLRTObject_decode,
        UMLRTObject_encode,
        UMLRTObjectDestroy<UMLRTCapsuleId>,
        UMLRTObject_getSize,
        UMLRTObject_fprintf_UMLRTCapsuleId,
        NULL, // super
        UMLRTOBJECTCLASS_DEFAULT_VERSION, // version
        UMLRTOBJECTCLASS_DEFAULT_BACKWARDS, // backwards
        sizeof(UMLRTCapsuleId),
        sizeof(UMLRTCapsuleId),
        NULL, // fields
};

const UMLRTObject_class * const UMLRTType_UMLRTCapsuleId = &UMLRTType_UMLRTCapsuleId_;

