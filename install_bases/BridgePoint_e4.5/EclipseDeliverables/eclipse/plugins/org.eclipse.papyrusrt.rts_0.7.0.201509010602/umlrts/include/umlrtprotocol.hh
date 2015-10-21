// umlrtprotocol.hh

/*******************************************************************************
 * Copyright (c) 2015 Zeligsoft (2009) Limited  and others.
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * which accompanies this distribution, and is available at
 * http://www.eclipse.org/legal/epl-v10.html
 *******************************************************************************/

#ifndef UMLRTPROTOCOL_HH
#define UMLRTPROTOCOL_HH

#include "umlrtcommsport.hh"
#include "umlrtqueueelement.hh"
#include "umlrthashmap.hh"
#include <stdlib.h>

// UMLRTProtocol - base type for port protocols.

// Basic operations are put() and get(). The implementation is a LIFO queue.

class UMLRTCapsuleId;
class UMLRTHashMap;

class UMLRTProtocol
{
public:
    // Turn port bindin notification on or off.
    void bindingNotification( bool on );

    // Return whether port notification is turned on for this port.
    bool bindingNotificatioRequested() const;

    // De-register this SAP port.
    bool deregisterSAP();

    // De-register this SPP port.
    bool deregisterSPP();

    // Get service port registration name.
    const char * getRegisteredName() const;

    // Find the smallest replication index connected to the far-end capsule.
    int indexTo( const UMLRTCapsule * capsule ) const;

    // Return true of port instance is bound.
    bool isBoundAt( int index) const;

    // Return true if far-end capsule is bound to port instance.
    bool isIndexTo( int index, const UMLRTCapsule * capsule ) const;

    // Return true if the service port is registered.
    bool isRegistered() const;

    // Empty the defer queue associated with all port instances. Returns # of messages deleted.
    int purge();

    // Empty the defer queue for a port instance. Return # of messages deleted.
    int purgeAt( int index );

    // Recall a deferred message from one of all instances. Returns the number of recalled messages.
    int recall();

    // Recall all messages from all port instances. Returns the number of recalled messages.
    int recallAll();

    // Recall all messages from a single port instance. Returns the number of recalled messages.
    int recallAllAt( int index, bool front = false );

    // Recall all messages to the front of the queue. Returns the number of recalled messages.
    int recallAllFront();

    // Recall a message on a port instance. Returns the number of recalled messages (0 or 1).
    int recallAt( int index, bool front = false );

    // Recall the first deferred message on a port instance. Returns the number of recalled messages (0 or 1).
    int recallFront();

    // Register this SAP port with the RTS to allow dynamic binds from SPPs. Return true for success.
    bool registerSAP( const char * service );

    // Register this SPP port with the serivce SAPs. Return true for success.
    bool registerSPP( const char * service );

    // Return the replication factor of the port.
    size_t size() const { return srcPort->numFarEnd; }

    static bool deregisterSppPort( const UMLRTCommsPort * sppPort );
    static bool deregisterSapPort( const UMLRTCommsPort * sapPort );
    static bool registerSppPort( const UMLRTCommsPort * sppPort, const char * service );
    static bool registerSapPort( const UMLRTCommsPort * sapPort, const char * service );
    static bool isRegistered( const UMLRTCommsPort * port );

    // Debugging
    static void debugOutputServiceRegistration();

private:
    struct UnboundSapPort : public UMLRTQueueElement
    {
        UnboundSapPort( const UMLRTCommsPort *srcPort ) : port(srcPort) { }
        const UMLRTCommsPort * port;
    };

    static void addSapToQueue( const char * service, const UMLRTCommsPort * sapPort );
    static void bindQueuedSapsToSpp( const UMLRTCommsPort * sppPort );
    static void completeRegistration( const UMLRTCommsPort * port, const char * service );
    static void completeDeregistration( const UMLRTCommsPort * port );
    static UMLRTHashMap * getNameToSppPortMap();
    static UMLRTHashMap * getNameToUnboundSapPortsMap();
    static UMLRTHashMap * getPortToNameMap();
    static void removeSapFromQueue( const UMLRTCommsPort * sapPort );
    static bool removeSapFromQueueMatchCompare( UnboundSapPort * element, const UMLRTCommsPort * port );
    static void removeSapFromQueueMatchNotify( UnboundSapPort * element, const UMLRTCommsPort * port );

    static void debugOutputPortToNameMap();
    static void debugOutputNameToSppPortMap();
    static void debugOutputNameToUnboundSapPortsMap();
    static bool debugOutputQueuedSap( UnboundSapPort * unboundSapPort, const char * service );

    static UMLRTHashMap * portToName;
    static UMLRTHashMap * nameToSppPort;
    static UMLRTHashMap * nameToUnboundSapPorts;

protected:
    UMLRTProtocol( const UMLRTCommsPort *& srcPort_ ) : srcPort(srcPort_) { }
    const UMLRTCommsPort *& srcPort;
};

#endif // UMLRTPROTOCOL_HH
