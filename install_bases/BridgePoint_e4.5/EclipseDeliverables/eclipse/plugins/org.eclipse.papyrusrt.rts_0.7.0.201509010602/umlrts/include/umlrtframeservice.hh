// umlrtframeservice.hh

/*******************************************************************************
* Copyright (c) 2015 Zeligsoft (2009) Limited  and others.
* All rights reserved. This program and the accompanying materials
* are made available under the terms of the Eclipse Public License v1.0
* which accompanies this distribution, and is available at
* http://www.eclipse.org/legal/epl-v10.html
*******************************************************************************/

#ifndef UMLRTFRAMESERVICE_HH
#define UMLRTFRAMESERVICE_HH

#include "umlrtcapsuleid.hh"
#include "umlrtmutex.hh"
#include "umlrtrtsinterfaceumlrt.hh"

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

class UMLRTFrameService
{
public:
    // Bind notifications for sub-capsules. If index == -1, the entire port (all far end instances) are implied.
    static void bindSubcapsulePort ( bool isBorder, UMLRTCapsule * subcapsule, int portIndex, int farEndIndex );
    static void unbindSubcapsulePort ( bool isBorder, UMLRTCapsule * subcapsule, int portIndex, int farEndIndex );

    // Bind SAP and SPP ports.
    static bool bindServicePort ( const UMLRTCommsPort * sapPort, const UMLRTCommsPort * sppPort );
    static bool unbindServicePort ( const UMLRTCommsPort * sapPort, const UMLRTCommsPort * sppPort );

    // Connect two ports. If the far-end of p1 is defined, it is the far-end of p1 that is connected to p2.
    static void connectPorts ( const UMLRTCommsPort * p1, size_t p1Index, const UMLRTCommsPort * p2, size_t p2Index );

    // Create a connection between the far end of the 'relay' port to the 'dest' port. The relay port itself is not modified.
    static void connectRelayPort ( const UMLRTCommsPort * relayPort, size_t relayIndex, const UMLRTCommsPort * destPort, size_t destIndex );

    // Destroy a slot - run from the controller.
    static void controllerDeport ( UMLRTSlot * slot, bool synchronous, bool lockAcquired );

    // Destroy a slot - run from the controller.
    static void controllerDestroy ( UMLRTSlot * slot, bool isTopSlot, bool synchronous, bool lockAcquired );

    // Import a capsule - run from the controller.
    static bool controllerImport ( UMLRTSlot * slot, UMLRTCapsule * capsule, bool synchronous, bool lockAcuired );

    // Recurse into sub-structure requesting a destroy of the slot from the associated controller.
    static void controllerIncarnate ( UMLRTCapsule * capsule, size_t sizeSerializedData, void * serializedData );

    // Count the number of free far-ends on a port.
    static size_t freeFarEndsCount ( const UMLRTCommsPort * port );

    // Create a port list - either slot ports, port map or internal ports. No ports are connected yet.
    static const UMLRTCommsPort * createPortArray ( UMLRTSlot * slot, const UMLRTCapsuleClass * capsuleClass );

    static const UMLRTCommsPort * * createInternalPorts ( UMLRTSlot * slot, const UMLRTCapsuleClass * capsuleClass );

    // Create the trivial border port list from the slot ports.
    static const UMLRTCommsPort * * createBorderPorts ( UMLRTSlot * slot, size_t numPorts );

    // Disconnect a port.
    static void disconnectPort ( const UMLRTCommsPort * port, int farEndIndex );

    // Initialize a capsule - either from controller for static slots or from controllerIncarnate for dynamic capsules.
    static void initializeCapsule ( UMLRTCapsule * capsule, size_t sizeSerializedData, void * serializedData );

    // Instantiate a capsule using it's class. Called from code gen.
    static void instantiate ( UMLRTSlot * slot, const UMLRTCapsuleClass * capsuleClass );

    // Obtain an instance of the RTS library interface class for capsule access to RTS.
    static const UMLRTRtsInterface * getRtsInterface ( );

    // Move the end-point of a connection. This causes the far-end of srcPort to become the far-end of destPort. The srcPort gets disconnected.
    static void moveFarEnd ( const UMLRTCommsPort * srcPort, size_t srcIndex, const UMLRTCommsPort * destPort, size_t destIndex );

    // Import a capsule into a slot.
    static bool importCapsule ( const UMLRTCommsPort * srcPort, UMLRTCapsule * capsule, const UMLRTCapsulePart * dest, int index );

    // Perform the incarnate work - called from all overloaded incarnate methods.
    static const UMLRTCapsuleId incarnateCapsule ( const UMLRTCommsPort * srcPort, const UMLRTCapsulePart * part, const UMLRTCapsuleClass * capsuleClass, const void * userData, const UMLRTObject_class * type, const char * logThread, UMLRTController * controller, int index );

    // Request a deport.
    static void requestControllerDeport ( UMLRTSlot * slot, bool lockAcquired );

    // Traverse sub-structure enqueuing destory commands to the associated controllers.
    static void requestControllerDestroy ( UMLRTSlot * slotToDestroy, bool isTopSlot, bool lockAcuired );

    // For locking the RTS - performed outside frame service in context of signal send.
    static void rtsLock ( );
    static void rtsUnlock ( );

    // Send rtBound/rtUnbound to port.
    static void sendBoundUnbound( const UMLRTCommsPort * * ports, int portId, int farEndIndex, bool isBind );

private:
    // Define (or augment) a capsule border port list (during incarnate or import) by mapping requested class with the slot class.
    // If 'bind' param is false, just check. If 'bind' param is true, perform the binding.
    static const UMLRTCommsPort * * bindPorts ( UMLRTSlot * slot, UMLRTCapsule * capsule, const UMLRTCapsuleClass * requestedClass,
            const UMLRTCapsuleClass * slotClass, const UMLRTCommsPort * * borderPorts, bool bind, bool import );

    // Take all the far-ends of a slot port and bind them to free instances of an existing import capsule border port.
    static void bindPortsAddFarEnds ( const UMLRTCommsPort * borderPort, int portId, const UMLRTCommsPort * slotport, UMLRTCapsule * capsule, bool import );

    // Create a new comms port for the capsule border port with sufficient far ends, and connect the far-ends appropriately.
    static const UMLRTCommsPort * bindPortsCreateProxyPort ( UMLRTSlot * slot, UMLRTCapsule * capsule, const UMLRTCommsPort * * borderPorts, const UMLRTCommsPort * slotPort, const UMLRTCapsuleClass * requestedClass, int req_i, bool bind, bool import );

    // Count bound port instances for import check.
    static size_t bindPortsCountBound ( const UMLRTCommsPort * port );

    // Recurse into sub-structure setting slots as 'condemned'.
    static void condemnParts ( UMLRTSlot * slot );

    // Call capsule #disconnect methods for this deport.
    static void controllerDeportUnbind ( UMLRTSlot * slot, const UMLRTCommsPort * * borderPorts );

    // Create a capsule name based on parent. Using when constructing slots.
    static char * createName ( const char * prefix, const char * suffix, size_t index, bool appendIndex );

    // Recurse and create the sub-structure of a slot.
    static UMLRTCapsulePart * createParts ( const char * containerName, const UMLRTCapsuleClass * containerClass);

    // Create a single port.
    static const UMLRTCommsPort * createPort ( UMLRTSlot * slot, const UMLRTCapsuleClass * capsuleClass, int roleIndex,
            bool border, bool importProxy = false, bool isUnbound = false );

    // Define port contents.
    static void definePort ( UMLRTCommsPort * port, UMLRTSlot * slot, const UMLRTCapsuleClass * capsuleClass, int roleIndex,
            bool border, bool importProxy = false, bool isUnbound = false );

    // Recurse into sub-structure assigning controllers to capsules.
    static void defineSlotControllers ( UMLRTSlot * slot, const UMLRTCapsulePart * part, const UMLRTCapsuleClass * capsuleClass,
            const char * logThread, UMLRTController * assignedController, UMLRTController * defaultController, int index );

    // Destroy the parts book-keeping within a slot. Doesn't destroy the parts themselves (controllerRequestDestroy used to destroy parts).
    static void destroyPartsBookkeeping ( size_t numParts, const UMLRTCapsulePart * parts );

    // Recurse into sub-structure deporting any slots that require deporting.
    static void deportParts ( UMLRTSlot * slot );

    // Destroy a port's contents. Optionally delete the port itself.
    static void destroyPort ( const UMLRTCommsPort * port, bool deletePort );

    // Destroy a port array. Called when destroying slot ports.
    static void destroyPortArray ( const UMLRTCommsPort * ports, size_t numPorts );

    // Destroy a port list. Called when destroying capsule border ports.
    static void destroyPortList ( const UMLRTCommsPort * * ports, size_t numPorts, bool proxiesOnly );

    // Find instance of 'borderPort' connected to 'farEnd'.
    static int findFarEndInstance ( const UMLRTCommsPort * farEnd, size_t farEndIndex, const UMLRTCommsPort * port );

    // Obtain the controller for this capsule based on a list of rules for assigning controllers
    static UMLRTController * getCapsuleController ( const UMLRTSlot * slot, const UMLRTCapsulePart * part, const char * logThread, UMLRTController * assignedController, UMLRTController * defaultController, int slotIndex );

    // If user doesn't explicitly specify the slot index, we get the next free slot-index available.
    static int getNextFreeCapsuleSlot ( const UMLRTCapsulePart * part );

    // Send rtBound to far-ends of slot not associated with relay ports.
    static void incarnateSendBoundSlotFarEnd ( UMLRTSlot * slot );

    // Initialize ports, including registration of service ports for a given capsule.
    static void initializeCapsulePorts ( UMLRTCapsule * capsule );

    // Initialize a port, including creation of defer queue and registration of service ports.
    static void initializePort ( const UMLRTCommsPort * port );

    // Initialize a list of ports.
    static void initializePortList ( const UMLRTCommsPort * * ports, size_t numPorts );

    // Return far-end port if port is bound - far-end exists and is occupied and is a two-way connection back to near-end.
    static const UMLRTCommsPort * isBound ( const UMLRTCommsPort * port, int index, int * farEndIndexP );

    // Request an import.
    static bool requestControllerImport ( UMLRTSlot * slot, UMLRTCapsule * capsule, bool lockAcquired );

    // Recurse into sub-structure requesting a destroy of the slot from the associated controller.
    static void requestControllerIncarnate ( UMLRTSlot * slot, const void * userData, const UMLRTObject_class * type );

    // Send rtBound/rtUnbound for 'port' and 'index' - farEndPort is used as 'src' of rtBound/rtUnbound signal.
    static void sendBoundUnbound( const UMLRTCommsPort * port, int index, const UMLRTCommsPort * farEndPort, int farEndIndex, bool isBind );

    // Send rtBound/rtUnbound for all capsule local ports as appropriate.
    static void sendBoundUnboundForCapsule ( UMLRTCapsule * capsule, bool isBind );

    // Send rtBound/rtUnbound for local port.
    static void sendBoundUnboundForPort ( const UMLRTCommsPort * port, bool isBind );

    // Send rtBound/rtUnbound for local port.
    static void sendBoundUnboundForPortList ( const UMLRTCommsPort * * ports, size_t numPorts, bool isBind );

    // A global lock is used when modifying ports and accessing port far-ends for message delivery.
    // More efficient locking can be used in a future version.
    static UMLRTMutex rtsGlobalLock;

    // Interface object for capsule instances to interact with the RTS.
    static UMLRTRtsInterfaceUmlrt rtsifUmlrt;
};

#endif // UMLRTFRAMESERVICE_HH
