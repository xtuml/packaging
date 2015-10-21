// umlrtframeprotocol.hh

/*******************************************************************************
* Copyright (c) 2014-2015 Zeligsoft (2009) Limited  and others.
* All rights reserved. This program and the accompanying materials
* are made available under the terms of the Eclipse Public License v1.0
* which accompanies this distribution, and is available at
* http://www.eclipse.org/legal/epl-v10.html
*******************************************************************************/

#ifndef UMLRTFRAMEPROTOCOL_HH
#define UMLRTFRAMEPROTOCOL_HH

#include "umlrtcapsuleid.hh"
#include "umlrtmutex.hh"
#include "umlrtprotocol.hh"

#include <stdlib.h>

struct UMLRTCapsuleClass;
struct UMLRTSlot;
struct UMLRTCapsulePart;
struct UMLRTCapsuleRole;
struct UMLRTCommsPort;
struct UMLRTCommsPortRole;
class UMLRTController;
struct UMLRTObject_class;
struct UMLRTTypedValue;

// Protocol for frame ports.

class UMLRTFrameProtocol
{
public:
    class InSignals {  };
    class OutSignals {
    public:

        // Return true if a subClass is a sub-class of baseClass.
        bool classIsKindOf( const UMLRTCommsPort * srcPort, const UMLRTCapsuleClass & subClass, const UMLRTCapsuleClass & baseClass );

        // Return the name of a capsule class.
        const char * className( const UMLRTCommsPort * srcPort, const UMLRTCapsuleClass & capsuleClass );

        // Return the capsule class of a capsule instance.
        const UMLRTCapsuleClass & classOf( const UMLRTCommsPort * srcPort, const UMLRTCapsuleId id );

        // Deport a capsule instance from a plugin slot.
        bool deport( const UMLRTCommsPort * srcPort, const UMLRTCapsuleId id, const UMLRTCapsulePart * part );

        // Destroy can either destroy all capsules in a slot or just a single capsule.
        bool destroy( const UMLRTCommsPort * srcPort, const UMLRTCapsuleId id );
        bool destroy( const UMLRTCommsPort * srcPort, const UMLRTCapsulePart * part );

        // Import a capsule instance into a plugin slot.
        bool import( const UMLRTCommsPort * srcPort, const UMLRTCapsuleId id, const UMLRTCapsulePart * destPart, int index = -1 );
        bool import( const UMLRTCommsPort * srcPort, const UMLRTCapsulePart * srcPart, const UMLRTCapsulePart * destPart, int index = -1 );

        // #incarnate methods create a dynamic capsule.
        const UMLRTCapsuleId incarnate( const UMLRTCommsPort * srcPort, const UMLRTCapsulePart * part, const char * logThread = NULL, int index = -1 ) const;
        const UMLRTCapsuleId incarnate( const UMLRTCommsPort * srcPort, const UMLRTCapsulePart * part, UMLRTController * controller, int index = -1 ) const;

        const UMLRTCapsuleId incarnate( const UMLRTCommsPort * srcPort, const UMLRTCapsulePart * part, const UMLRTCapsuleClass & capsuleClass, const char * logThread = NULL, int index = -1 ) const;
        const UMLRTCapsuleId incarnate( const UMLRTCommsPort * srcPort, const UMLRTCapsulePart * part, const UMLRTCapsuleClass & capsuleClass, UMLRTController * controller, int index = -1 ) const;

        const UMLRTCapsuleId incarnate( const UMLRTCommsPort * srcPort, const UMLRTCapsulePart * part, const void * userData, const UMLRTObject_class * type, const char * logThread = NULL, int index = -1 ) const;
        const UMLRTCapsuleId incarnate( const UMLRTCommsPort * srcPort, const UMLRTCapsulePart * part, const void * userData, const UMLRTObject_class * type, UMLRTController * controller, int index = -1 ) const;

        const UMLRTCapsuleId incarnate( const UMLRTCommsPort * srcPort, const UMLRTCapsulePart * part, const UMLRTCapsuleClass & capsuleClass, const void * userData, const UMLRTObject_class * type, const char * logThread = NULL, int index = -1 ) const;
        const UMLRTCapsuleId incarnate( const UMLRTCommsPort * srcPort, const UMLRTCapsulePart * part, const UMLRTCapsuleClass & capsuleClass, const void * userData, const UMLRTObject_class * type, UMLRTController * controller, int index = -1 ) const;

        const UMLRTCapsuleId incarnate( const UMLRTCommsPort * srcPort, const UMLRTCapsulePart * part, const UMLRTTypedValue * value, const char * logThread = NULL, int index = -1 ) const;
        const UMLRTCapsuleId incarnate( const UMLRTCommsPort * srcPort, const UMLRTCapsulePart * part, const UMLRTTypedValue * value, UMLRTController * controller, int index = -1 ) const;

        const UMLRTCapsuleId incarnate( const UMLRTCommsPort * srcPort, const UMLRTCapsulePart * part, const UMLRTCapsuleClass & capsuleClass, const UMLRTTypedValue * value, const char * logThread = NULL, int index = -1 ) const;
        const UMLRTCapsuleId incarnate( const UMLRTCommsPort * srcPort, const UMLRTCapsulePart * part, const UMLRTCapsuleClass & capsuleClass, const UMLRTTypedValue * value, UMLRTController * constroller, int index = -1 ) const;

        const UMLRTCapsuleId incarnateAt( const UMLRTCommsPort * srcPort, const UMLRTCapsulePart * part, int index = -1 );

        const UMLRTCapsuleId incarnationAt( const UMLRTCommsPort * srcPort, const UMLRTCapsulePart * part, int index = -1 );

        const UMLRTCapsuleId me( const UMLRTCommsPort * srcPort );

        const UMLRTCapsuleClass & myClass( const UMLRTCommsPort * srcPort );
    };
    typedef OutSignals Base;
    typedef InSignals Conjugate;

};

// Frame-port type.

class UMLRTFrameProtocol_baserole : public UMLRTProtocol, private UMLRTFrameProtocol::Base
{
public:
    UMLRTFrameProtocol_baserole( const UMLRTCommsPort *& srcPort ) : UMLRTProtocol( srcPort ) { }

    // Return true if a subClass is a sub-class of baseClass.
    bool classIsKindOf( const UMLRTCapsuleClass & subClass, const UMLRTCapsuleClass & baseClass )
    {
        return UMLRTFrameProtocol::Base::classIsKindOf( srcPort, subClass, baseClass );
    }

    // Return the name of a capsule class.
    const char * className( const UMLRTCommsPort * srcPort, const UMLRTCapsuleClass & capsuleClass )
    {
        return UMLRTFrameProtocol::Base::className( srcPort, capsuleClass );
    }

    // Return the capsule class of a capsule instance.
    const UMLRTCapsuleClass & classOf( const UMLRTCapsuleId id )
    {
        return UMLRTFrameProtocol::Base::classOf( srcPort, id );
    }

    // Deport a capsule instance from a plugin slot.
    bool deport( const UMLRTCapsuleId id, const UMLRTCapsulePart * part )
    {
        return UMLRTFrameProtocol::Base::deport( srcPort, id, part );
    }

    // Destroy can either destroy all capsules in a slot or just a single capsule.
    bool destroy( const UMLRTCapsuleId id )
    {
        return UMLRTFrameProtocol::Base::destroy( srcPort, id );
    }

    bool destroy( const UMLRTCapsulePart * part )
    {
        return UMLRTFrameProtocol::Base::destroy( srcPort, part );
    }

    bool import( const UMLRTCapsuleId id, const UMLRTCapsulePart * dest, int index = -1 )
    {
        return UMLRTFrameProtocol::Base::import( srcPort, id, dest, index );
    }

    bool import( const UMLRTCapsulePart * srcPart, const UMLRTCapsulePart * destPart, int index = -1 )
    {
        return UMLRTFrameProtocol::Base::import( srcPort, srcPart, destPart, index );
    }

    // #incarnate methods create a dynamic capsule.
    const UMLRTCapsuleId incarnate( const UMLRTCapsulePart * part, const char * const logThread = NULL, int index = -1  ) const
    {
        return UMLRTFrameProtocol::Base::incarnate( srcPort, part, logThread, index );
    }

    const UMLRTCapsuleId incarnate( const UMLRTCapsulePart * part, UMLRTController * controller, int index = -1  ) const
    {
        return UMLRTFrameProtocol::Base::incarnate( srcPort, part, controller, index );
    }

    const UMLRTCapsuleId incarnate( const UMLRTCapsulePart * part, const UMLRTCapsuleClass & capsuleClass, const char * logThread = NULL, int index = -1   ) const
    {
        return UMLRTFrameProtocol::Base::incarnate( srcPort, part, capsuleClass, logThread, index );
    }

    const UMLRTCapsuleId incarnate( const UMLRTCapsulePart * part, const UMLRTCapsuleClass & capsuleClass, UMLRTController * controller, int index = -1   ) const
    {
        return UMLRTFrameProtocol::Base::incarnate( srcPort, part, capsuleClass,  controller, index );
    }

    const UMLRTCapsuleId incarnate( const UMLRTCapsulePart * part, const void * userData, const UMLRTObject_class * type, const char * logThread = NULL, int index = -1 ) const
    {
        return UMLRTFrameProtocol::Base::incarnate( srcPort, part, userData, type, logThread, index );
    }

    const UMLRTCapsuleId incarnate( const UMLRTCapsulePart * part, const void * userData, const UMLRTObject_class * type, UMLRTController * controller, int index = -1 ) const
    {
        return UMLRTFrameProtocol::Base::incarnate( srcPort, part, userData, type, controller, index );
    }

    const UMLRTCapsuleId incarnate( const UMLRTCapsulePart * part, const UMLRTCapsuleClass & capsuleClass, const void * userData, const UMLRTObject_class * type, const char * logThread = NULL, int index = -1 ) const
    {
        return UMLRTFrameProtocol::Base::incarnate( srcPort, part, capsuleClass, userData, type, logThread, index );
    }

    const UMLRTCapsuleId incarnate( const UMLRTCapsulePart * part, const UMLRTCapsuleClass & capsuleClass, const void * userData, const UMLRTObject_class * type, UMLRTController * controller, int index = -1 ) const
    {
        return UMLRTFrameProtocol::Base::incarnate( srcPort, part, capsuleClass, userData, type, controller, index );
    }

    const UMLRTCapsuleId incarnate( const UMLRTCapsulePart * part, const UMLRTTypedValue * value, const char * logThread = NULL, int index = -1 ) const
    {
        return UMLRTFrameProtocol::Base::incarnate( srcPort, part, value, logThread, index );
    }

    const UMLRTCapsuleId incarnate( const UMLRTCapsulePart * part, const UMLRTTypedValue * value, UMLRTController * controller, int index = -1 ) const
    {
        return UMLRTFrameProtocol::Base::incarnate( srcPort, part, value, controller, index );
    }

    const UMLRTCapsuleId incarnate( const UMLRTCapsulePart * part, const UMLRTCapsuleClass & capsuleClass, const UMLRTTypedValue * value, const char * logThread = NULL, int index = -1 ) const
    {
        return UMLRTFrameProtocol::Base::incarnate( srcPort, part, capsuleClass, value, logThread, index );
    }

    const UMLRTCapsuleId incarnate( const UMLRTCapsulePart * part, const UMLRTCapsuleClass & capsuleClass, const UMLRTTypedValue * value, UMLRTController * controller, int index = -1 ) const
    {
        return UMLRTFrameProtocol::Base::incarnate( srcPort, part, capsuleClass, value, controller, index );
    }

    const UMLRTCapsuleId incarnateAt( const UMLRTCapsulePart * part, int index = -1  )
    {
        return UMLRTFrameProtocol::Base::incarnateAt( srcPort, part, index );
    }

    const UMLRTCapsuleId incarnationAt( const UMLRTCapsulePart * part, int index = -1 )
    {
        return UMLRTFrameProtocol::Base::incarnationAt( srcPort, part, index );
    }

    const UMLRTCapsuleId me()
    {
        return UMLRTFrameProtocol::Base::me( srcPort );
    }

    const UMLRTCapsuleClass & myClass()
    {
        return UMLRTFrameProtocol::Base::myClass( srcPort );
    }
};

#endif // UMLRTFRAMEPROTOCOL_HH
