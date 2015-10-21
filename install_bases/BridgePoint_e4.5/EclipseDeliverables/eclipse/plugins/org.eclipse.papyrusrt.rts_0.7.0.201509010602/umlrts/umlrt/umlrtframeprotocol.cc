// umlrtframeprotocol.cc

/*******************************************************************************
* Copyright (c) 2014-2015 Zeligsoft (2009) Limited  and others.
* All rights reserved. This program and the accompanying materials
* are made available under the terms of the Eclipse Public License v1.0
* which accompanies this distribution, and is available at
* http://www.eclipse.org/legal/epl-v10.html
*******************************************************************************/

#include "umlrtcapsule.hh"
#include "umlrtcapsuleid.hh"
#include "umlrtcapsulepart.hh"
#include "umlrtcommsport.hh"
#include "umlrtcontroller.hh"
#include "umlrtframeprotocol.hh"
#include "umlrtframeservice.hh"
#include "basefatal.hh"

// Return true if a subClass is a sub-class of baseClass.
bool UMLRTFrameProtocol::OutSignals::classIsKindOf( const UMLRTCommsPort * srcPort, const UMLRTCapsuleClass & subClass, const UMLRTCapsuleClass & baseClass )
{
    bool isKindOf = false;
    const UMLRTCapsuleClass * parentClass = &subClass;

    while ((parentClass != NULL) && !isKindOf)
    {
        if (parentClass == &baseClass)
        {
            isKindOf = true;
        }
        parentClass = parentClass->super;
    }
    return isKindOf;
}

// Return the name of a capsule class.
const char * UMLRTFrameProtocol::OutSignals::className( const UMLRTCommsPort * srcPort, const UMLRTCapsuleClass & capsuleClass )
{
    return capsuleClass.name;
}

// Return the capsule class of a capsule instance.
const UMLRTCapsuleClass & UMLRTFrameProtocol::OutSignals::classOf( const UMLRTCommsPort * srcPort, const UMLRTCapsuleId id )
{
    return *id.getCapsule()->getClass();
}


bool UMLRTFrameProtocol::OutSignals::deport( const UMLRTCommsPort * srcPort, const UMLRTCapsuleId id, const UMLRTCapsulePart * part )
{
    bool ok = false;

    // Initialize error code to E_NONE, in case anybody looks at it after success.
    srcPort->slot->controller->setError(UMLRTController::E_OK);

    if (!id.isValid())
    {
        srcPort->slot->controller->setError(UMLRTController::E_DEPORT_INVLD);
    }
    else if (!part->role()->plugin)
    {
        srcPort->slot->controller->setError(UMLRTController::E_DEPORT_NONPLUG);
    }
    else
    {
        UMLRTCapsule * capsule = id.getCapsule();

        // Only specified the the capsule id within the part - we must find the slot being deported.
        for (size_t i = 0; (i < part->role()->multiplicityUpper) && !ok; ++i)
        {
            if (part->slots[i]->capsule == capsule)
            {
                ok = true;
                UMLRTFrameService::requestControllerDeport(part->slots[i], false/*lockAcquired*/);
            }
        }
        if (!ok)
        {
            srcPort->slot->controller->setError(UMLRTController::E_DEPORT_NOINST);
        }
    }
    return ok;
}

bool UMLRTFrameProtocol::OutSignals::destroy( const UMLRTCommsPort * srcPort, const UMLRTCapsuleId id )
{
    bool ok = false;
    UMLRTCapsule * capsule = id.getCapsule();

    // Initialize error code to E_NONE, in case anybody looks at it after success.
    srcPort->slot->controller->setError(UMLRTController::E_OK);

    if (capsule == NULL)
    {
        srcPort->slot->controller->setError(UMLRTController::E_DESTR_INVALID);
    }
    else if (capsule->getSlot()->role() == NULL)
    {
        FATAL("Destroying capsule without a role. Destroying Top?");
    }
    else if (!capsule->getSlot()->role()->optional)
    {
        srcPort->slot->controller->setError(UMLRTController::E_DESTR_NONOPT);
    }
    else
    {
        // Either we destroy the capsule in this context or we send a DESTROY Controller Command to execute the destroy.
        UMLRTFrameService::requestControllerDestroy(capsule->getSlot(), true/*isTopSlot*/, false/*lockAcquired*/);
        ok = true;
    }
    return ok;
}

bool UMLRTFrameProtocol::OutSignals::destroy( const UMLRTCommsPort * srcPort, const UMLRTCapsulePart * part )
{
    bool ok = false;

    // Initialize error code to E_NONE, in case anybody looks at it after success.
    srcPort->slot->controller->setError(UMLRTController::E_OK);

    if (part->role() == NULL)
    {
        FATAL("Destroying part without a role. Destroying Top?");
    }
    if (!part->role()->optional)
    {
        srcPort->slot->controller->setError(UMLRTController::E_DESTR_NONOPT);
    }
    else
    {
        ok = true; // Assume ok from this point.

        for (size_t i = part->role()->multiplicityLower; i < part->numSlot; ++i)
        {
            if (part->slots[i]->capsule != NULL)
            {
                // Either we destroy the capsule in this context or we send a DESTROY Controller Command to execute the destroy.
                // This is a destroy of multiple slots and each is the 'top slot' of its associated sub-structure.
                UMLRTFrameService::requestControllerDestroy(part->slots[i], true/*isTopSlot*/, false/*lockAcquired*/);
                ok = true;
            }
        }
    }
    return ok;
}

bool UMLRTFrameProtocol::OutSignals::import( const UMLRTCommsPort * srcPort, const UMLRTCapsuleId id, const UMLRTCapsulePart * destPart, int index )
{
    bool ok = false;

    if (!id.isValid())
    {
        srcPort->slot->controller->setError(UMLRTController::E_IMPORT_NOINST);
    }
    else
    {
        ok = UMLRTFrameService::importCapsule(srcPort, id.getCapsule(), destPart, index);
    }
    return ok;
}

bool UMLRTFrameProtocol::OutSignals::import( const UMLRTCommsPort * srcPort, const UMLRTCapsulePart * srcPart, const UMLRTCapsulePart * dest, int index )
{
    bool ok = false;

    if ((srcPart->numSlot > 1) && index == -1)
    {
        srcPort->slot->controller->setError(UMLRTController::E_IMPORT_PARTREPL);
    }
    else if (srcPart->slots[0]->capsule == NULL)
    {
        srcPort->slot->controller->setError(UMLRTController::E_IMPORT_NOINST);
    }
    else
    {
        ok = UMLRTFrameService::importCapsule(srcPort, srcPart->slots[0]->capsule, dest, index);
    }
    return ok;
}

const UMLRTCapsuleId UMLRTFrameProtocol::OutSignals::incarnate( const UMLRTCommsPort * srcPort, const UMLRTCapsulePart * part, const char * logThread, int index ) const
{
    return UMLRTFrameService::incarnateCapsule(
            srcPort,
            part, // part
            NULL, // capsuleClass
            NULL, // userData
            NULL, // type
            logThread,
            NULL, // controller
            index
            );
}

const UMLRTCapsuleId UMLRTFrameProtocol::OutSignals::incarnate( const UMLRTCommsPort * srcPort, const UMLRTCapsulePart * part, UMLRTController * controller, int index ) const
{
    return UMLRTFrameService::incarnateCapsule(
            srcPort,
            part,
            NULL, // capsuleClass
            NULL, // userData
            NULL, // type
            NULL, // logThread
            controller,
            index
            );
}

const UMLRTCapsuleId UMLRTFrameProtocol::OutSignals::incarnate( const UMLRTCommsPort * srcPort, const UMLRTCapsulePart * part, const UMLRTCapsuleClass & capsuleClass, const char * logThread, int index ) const
{
    return UMLRTFrameService::incarnateCapsule(
            srcPort,
            part,
            &capsuleClass,
            NULL, // userData
            NULL, // type
            logThread,
            NULL, // controller
            index
            );
}

const UMLRTCapsuleId UMLRTFrameProtocol::OutSignals::incarnate( const UMLRTCommsPort * srcPort, const UMLRTCapsulePart * part, const UMLRTCapsuleClass & capsuleClass, UMLRTController * controller, int index ) const
{
    return UMLRTFrameService::incarnateCapsule(
            srcPort,
            part,
            &capsuleClass,
            NULL, // userData
            NULL, // type
            NULL, // logThread
            controller,
            index
            );
}

const UMLRTCapsuleId UMLRTFrameProtocol::OutSignals::incarnate( const UMLRTCommsPort * srcPort, const UMLRTCapsulePart * part, const void * userData, const UMLRTObject_class * type, const char * logThread, int index ) const
{
    return UMLRTFrameService::incarnateCapsule(
            srcPort,
            part,
            NULL, // capsuleClass
            userData,
            type,
            logThread,
            NULL, // controller
            index
            );
}

const UMLRTCapsuleId UMLRTFrameProtocol::OutSignals::incarnate( const UMLRTCommsPort * srcPort, const UMLRTCapsulePart * part, const void * userData, const UMLRTObject_class * type, UMLRTController * controller, int index ) const
{
    return UMLRTFrameService::incarnateCapsule(
            srcPort,
            part,
            NULL, // capsuleClass
            userData,
            type,
            NULL, // logThread
            controller,
            index
            );
}

const UMLRTCapsuleId UMLRTFrameProtocol::OutSignals::incarnate( const UMLRTCommsPort * srcPort, const UMLRTCapsulePart * part, const UMLRTCapsuleClass & capsuleClass, const void * userData, const UMLRTObject_class * type, const char * logThread, int index ) const
{
    return UMLRTFrameService::incarnateCapsule(
            srcPort,
            part,
            &capsuleClass,
            userData,
            type,
            logThread,
            NULL, // controller
            index
            );
}

const UMLRTCapsuleId UMLRTFrameProtocol::OutSignals::incarnate( const UMLRTCommsPort * srcPort, const UMLRTCapsulePart * part, const UMLRTCapsuleClass & capsuleClass, const void * userData, const UMLRTObject_class * type, UMLRTController * controller, int index ) const
{
    return UMLRTFrameService::incarnateCapsule(
            srcPort,
            part,
            &capsuleClass,
            userData,
            type,
            NULL, // logThread
            controller,
            index
            );
}

const UMLRTCapsuleId UMLRTFrameProtocol::OutSignals::incarnate( const UMLRTCommsPort * srcPort, const UMLRTCapsulePart * part, const UMLRTTypedValue * value, const char * logThread, int index ) const
{
    return UMLRTFrameService::incarnateCapsule(
            srcPort,
            part,
            NULL, // capsuleClass
            value->data, // userData
            value->type, // type
            logThread,
            NULL, // controller
            index
            );
}

const UMLRTCapsuleId UMLRTFrameProtocol::OutSignals::incarnate( const UMLRTCommsPort * srcPort, const UMLRTCapsulePart * part, const UMLRTTypedValue * value, UMLRTController * controller, int index ) const
{
    return UMLRTFrameService::incarnateCapsule(
            srcPort,
            part,
            NULL, // capsuleClass
            value->data, // userData
            value->type, // type
            NULL, // logThread
            controller,
            index
            );
}

const UMLRTCapsuleId UMLRTFrameProtocol::OutSignals::incarnate( const UMLRTCommsPort * srcPort, const UMLRTCapsulePart * part, const UMLRTCapsuleClass & capsuleClass, const UMLRTTypedValue * value, const char * logThread, int index ) const
{
    return UMLRTFrameService::incarnateCapsule(
            srcPort,
            part,
            &capsuleClass,
            value->data, // userData
            value->type, // type
            logThread,
            NULL, // controller
            index
            );
}

const UMLRTCapsuleId UMLRTFrameProtocol::OutSignals::incarnate( const UMLRTCommsPort * srcPort, const UMLRTCapsulePart * part, const UMLRTCapsuleClass & capsuleClass, const UMLRTTypedValue * value, UMLRTController * controller, int index ) const
{
    return UMLRTFrameService::incarnateCapsule(
            srcPort,
            part,
            &capsuleClass,
            value->data, // userData
            value->type, // type
            NULL, // logThread
            controller,
            index
            );
}

const UMLRTCapsuleId UMLRTFrameProtocol::OutSignals::incarnateAt( const UMLRTCommsPort * srcPort, const UMLRTCapsulePart * part, int index )
{
    return UMLRTFrameService::incarnateCapsule(
            srcPort,
            part, // part
            NULL, // capsuleClass
            NULL, // userData
            NULL, // type
            NULL, // logThread
            NULL, // controller
            index
            );
}

const UMLRTCapsuleId UMLRTFrameProtocol::OutSignals::me( const UMLRTCommsPort * srcPort )
{
    return UMLRTCapsuleId(srcPort->slot->capsule);
}

const UMLRTCapsuleClass & myClass( const UMLRTCommsPort * srcPort )
{
    return *srcPort->slot->capsule->getClass();
}
