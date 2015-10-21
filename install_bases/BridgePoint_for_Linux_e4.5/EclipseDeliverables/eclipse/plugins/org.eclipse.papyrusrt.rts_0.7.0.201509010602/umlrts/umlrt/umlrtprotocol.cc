// umlrtprotocol.cc

/*******************************************************************************
 * Copyright (c) 2015 Zeligsoft (2009) Limited  and others.
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * which accompanies this distribution, and is available at
 * http://www.eclipse.org/legal/epl-v10.html
 *******************************************************************************/

#include "umlrtcommsport.hh"
#include "umlrtcommsportfarend.hh"
#include "umlrtcontroller.hh"
#include "umlrtframeservice.hh"
#include "umlrthashmap.hh"
#include "umlrtprotocol.hh"
#include "basefatal.hh"
#include "basedebugtype.hh"
#include "basedebug.hh"
#include <string.h>

/*static*/ UMLRTHashMap * UMLRTProtocol::portToName; // key and object are associated with the port - not allocated/deallocated.
/*static*/ UMLRTHashMap * UMLRTProtocol::nameToSppPort; // key and object are associated with the port - not allocated/deallocated.
/*static*/ UMLRTHashMap * UMLRTProtocol::nameToUnboundSapPorts; // key and object are both allocated and must be deallocated.

/*static*/ void UMLRTProtocol::addSapToQueue( const char * service, const UMLRTCommsPort * sapPort )
{
    // Assumes RTS lock obtained.
    UMLRTQueue * unboundSapPorts;
    if ((unboundSapPorts = (UMLRTQueue *)getNameToUnboundSapPortsMap()->getObject(service)) == NULL)
    {
        // This service had no SAP queue - create it.
        unboundSapPorts = new UMLRTQueue();

        // Both key and object (queue) must be deallocated when we remove it from the map.
        getNameToUnboundSapPortsMap()->insert(strdup(service), unboundSapPorts);
    }
    // Queue elements must be deleted when they are removed from the queue.
    UnboundSapPort * element = new UnboundSapPort(sapPort);

    BDEBUG(BD_SAP,"add slot %s port %s to SPP %s pending queue - ENQUEUE\n", sapPort->slot->name, sapPort->role()->name, service);
    unboundSapPorts->enqueue(element);
}

void UMLRTProtocol::bindingNotification( bool on )
{
    srcPort->notification = on;
}

bool UMLRTProtocol::bindingNotificatioRequested() const
{
    return srcPort->notification;
}

/*static*/ void UMLRTProtocol::bindQueuedSapsToSpp( const UMLRTCommsPort * sppPort )
{
    // Assumes RTS lock obtained.
    int freeFarEnds = UMLRTFrameService::freeFarEndsCount(sppPort);

    // Get queue of waiting SAP ports.
    UMLRTQueue * unboundSapPorts = (UMLRTQueue *)getNameToUnboundSapPortsMap()->getObject(sppPort->registeredName);

    for (size_t i = 0; (unboundSapPorts != NULL) && (i < sppPort->numFarEnd) && (freeFarEnds > 0); ++i)
    {
        // Get next waiting SAP port.
        UnboundSapPort * element;
        if ((element = (UnboundSapPort *)unboundSapPorts->dequeue()) != NULL)
        {
            // Bind the next SAP.
            UMLRTFrameService::bindServicePort(element->port, sppPort);
            delete element;
            // One less free port on SPP.
            --freeFarEnds;
        }
    }
    if (unboundSapPorts != NULL)
    {
        if (unboundSapPorts->isEmpty())
        {
            // No more queued SAP ports for this SPP - remove the queue from the unbound-map.
            // Both the key and the object (queue) must be deallocated.
            const void * key = getNameToUnboundSapPortsMap()->remove(sppPort->registeredName);
            if (key != NULL)
            {
                free((void *)key);
            }
            delete unboundSapPorts;
        }
    }
}

/*static*/ void UMLRTProtocol::completeRegistration( const UMLRTCommsPort * port, const char * service )
{
    // Assumes RTS lock obtained.
    if (port->registeredName == NULL)
    {
        port->registeredName = strdup(service);
    }
    if (strcmp(port->registeredName, service))
    {
        if (!port->generated)
        {
            free((void *)port->registeredName);
        }
        port->registeredName = strdup(service);
    }
    // Memory for key/object in portToNameMap not managed with map.
    getPortToNameMap()->insert(port, port->registeredName);
}

/*static*/ void UMLRTProtocol::completeDeregistration( const UMLRTCommsPort * port )
{
    // Assumes RTS lock obtained.
    // Memory for key/object in portToNameMap not managed with map.
    getPortToNameMap()->remove(port);
    if (port->registeredName != NULL)
    {
        if (!port->generated)
        {
            free((void *)port->registeredName);
            port->registeredName = NULL;
        }
    }
}

/*static*/ bool UMLRTProtocol::deregisterSapPort( const UMLRTCommsPort * sapPort )
{
    bool ok = false;
    if (!sapPort->sap)
    {
        sapPort->slot->controller->setError(UMLRTController::E_SAPDREG_NOT_SAP);
    }
    else if (sapPort->registeredName == NULL)
    {
        ok = true; // Wasn't registered.
    }
    else
    {
        const char * service;
        if ((service = (const char *)getPortToNameMap()->getObject(sapPort)) != NULL)
        {
            // Was registered. Remove this SAP if it was queued for the SPP.
            removeSapFromQueue(sapPort);

            if (sapPort->numFarEnd > 0)
            {
                const UMLRTCommsPort * sppPort;
                if ((sppPort = sapPort->farEnds[0].port) != NULL)
                {
                    // If this SAP was bound to the SPP, unbind it.
                    UMLRTFrameService::unbindServicePort(sapPort, sppPort);

                    // Go to pending SAP queue for this SPP and bind another SAP.
                    bindQueuedSapsToSpp(sppPort);
                }
            }
            completeDeregistration(sapPort);
        }
        ok = true;
    }
    BDEBUG(BD_SAP, "SAP de-register slot %s port %s %s %s\n", sapPort->slot->name, sapPort->role()->name, ok ? "ok" : "failed", ok ? "" : sapPort->slot->controller->strerror());
    return ok;
}

bool UMLRTProtocol::deregisterSAP()
{
    bool ok = false;
    if (srcPort->locked)
    {
        srcPort->slot->controller->setError(UMLRTController::E_SAPDREG_SAP_LOCKED);
    }
    else
    {
        UMLRTFrameService::rtsLock();

        ok = deregisterSapPort(srcPort);

        UMLRTFrameService::rtsUnlock();
    }
    return ok;
}

/*static*/ bool UMLRTProtocol::deregisterSppPort( const UMLRTCommsPort * sppPort )
{
    bool ok = false;

    if (!sppPort->spp)
    {
        sppPort->slot->controller->setError(UMLRTController::E_SPPDREG_NOT_SPP);
    }
    else
    {
        const char * service;
        if ((service = (const char *)getPortToNameMap()->getObject(sppPort)) != NULL)
        {
            // For every currently bound SAP port, unbind it and queue for re-binding later.
            for (size_t i = 0; i < sppPort->numFarEnd; ++i)
            {
                const UMLRTCommsPort * sapPort = sppPort->farEnds[i].port;
                if (sapPort != NULL)
                {
                    // A SAP was bound on this port. Unbind it and queue it to the service.
                    UMLRTFrameService::unbindServicePort(sapPort, sppPort);
                    addSapToQueue(sppPort->registeredName, sapPort);
                }
            }
            // This port no longer registered as an SPP. Memory is associated with the port.
            getNameToSppPortMap()->remove(sppPort->registeredName);
            completeDeregistration(sppPort);
        }
        ok = true;
    }
    BDEBUG(BD_SAP, "SPP de-register slot %s port %s %s %s\n", sppPort->slot->name, sppPort->role()->name, ok ? "ok" : "failed", ok ? "" : sppPort->slot->controller->strerror());
    return ok;
}

bool UMLRTProtocol::deregisterSPP()
{
    bool ok = false;
    if (srcPort->locked)
    {
        srcPort->slot->controller->setError(UMLRTController::E_SPPDREG_SPP_LOCKED);
    }
    else
    {
        UMLRTFrameService::rtsLock();

        ok = deregisterSppPort(srcPort);

        UMLRTFrameService::rtsUnlock();
    }
    return ok;
}

/*static*/ UMLRTHashMap * UMLRTProtocol::getPortToNameMap()
{
    if (portToName == NULL)
    {
        portToName = new UMLRTHashMap("portToName", UMLRTHashMap::compareValue, true/*objectIsString*/);
    }
    return portToName;
}

/*static*/ UMLRTHashMap * UMLRTProtocol::getNameToSppPortMap()
{
    if (nameToSppPort == NULL)
    {
        nameToSppPort = new UMLRTHashMap("nameToSppPort", UMLRTHashMap::compareString, false/*objectIsString*/);
    }
    return nameToSppPort;
}

/*static*/ UMLRTHashMap * UMLRTProtocol::getNameToUnboundSapPortsMap()
{
    if (nameToUnboundSapPorts == NULL)
    {
        nameToUnboundSapPorts = new UMLRTHashMap("nameToUnboundSapPorts", UMLRTHashMap::compareString, false/*objectIsString*/);
    }
    return nameToUnboundSapPorts;
}

const char * UMLRTProtocol::getRegisteredName() const
{
    return srcPort->registeredName;
}

/*static*/ bool UMLRTProtocol::isRegistered( const UMLRTCommsPort * port )
{
    UMLRTFrameService::rtsLock();

    bool registered = (getPortToNameMap()->getObject(port) != NULL);

    UMLRTFrameService::rtsUnlock();

    return registered;
}

bool UMLRTProtocol::isRegistered() const
{
    return isRegistered(srcPort);
}

bool UMLRTProtocol::isBoundAt( int index ) const
{
    bool bound = false;

    UMLRTFrameService::rtsLock();

    if ((index >= 0) && (index < (int)srcPort->numFarEnd))
    {
        bound = (srcPort->farEnds[index].port != NULL);
    }
    UMLRTFrameService::rtsUnlock();

    return bound;
}

int UMLRTProtocol::indexTo( const UMLRTCapsule * capsule ) const
{
    int index = -1;

    UMLRTFrameService::rtsLock();

    for (size_t i = 0; (i < srcPort->numFarEnd) && (index == -1); ++i)
    {
        if (srcPort->farEnds[i].port)
        {
            if (srcPort->farEnds[i].port->slot->capsule == capsule)
            {
                index = i;
            }
        }
    }
    UMLRTFrameService::rtsUnlock();

    return index;
}

bool UMLRTProtocol::isIndexTo( int index, const UMLRTCapsule * capsule ) const
{
    bool isIndex = false;

    UMLRTFrameService::rtsLock();

    if ((index >= 0) && (index <= (int)srcPort->numFarEnd))
    {
        isIndex = (srcPort->farEnds[index].port->slot->capsule == capsule);
    }
    UMLRTFrameService::rtsUnlock();

    return isIndex;
}

int UMLRTProtocol::purge()
{
    int count;

    UMLRTFrameService::rtsLock();

    count = srcPort->purge();

    UMLRTFrameService::rtsUnlock();

    return count;
}

int UMLRTProtocol::purgeAt( int index )
{
    int count;

    UMLRTFrameService::rtsLock();

    count = srcPort->purge(index);

    UMLRTFrameService::rtsUnlock();

    return count;
}

int UMLRTProtocol::recall()
{
    int count;

    UMLRTFrameService::rtsLock();

    count = srcPort->recall(-1/*index*/, false/*front*/, true/*one*/);

    UMLRTFrameService::rtsUnlock();

    return count;
}

int UMLRTProtocol::recallAll()
{
    int count;

    UMLRTFrameService::rtsLock();

    count = srcPort->recall(-1/*index*/, false/*front*/, false/*one*/);

    UMLRTFrameService::rtsUnlock();

    return count;
}

int UMLRTProtocol::recallAllAt( int index, bool front )
{
    int count;

    UMLRTFrameService::rtsLock();

    count = srcPort->recall(index, front, false/*one*/);

    UMLRTFrameService::rtsUnlock();

    return count;
}

int UMLRTProtocol::recallAllFront()
{
    int count;

    UMLRTFrameService::rtsLock();

    count = srcPort->recall(-1/*index*/, true/*front*/, false/*one*/);

    UMLRTFrameService::rtsUnlock();

    return count;
}

int UMLRTProtocol::recallAt( int index, bool front )
{
    int count;

    UMLRTFrameService::rtsLock();

    count = srcPort->recall(index, front, true/*one*/);

    UMLRTFrameService::rtsUnlock();

    return count;
}

int UMLRTProtocol::recallFront()
{
    int count;

    UMLRTFrameService::rtsLock();

    count = srcPort->recall(-1/*index*/, true/*front*/, true/*one*/);

    UMLRTFrameService::rtsUnlock();

    return count;
}

/*static*/ bool UMLRTProtocol::registerSapPort( const UMLRTCommsPort * sapPort, const char * service )
{
    bool ok = false;

    if (!sapPort->sap)
    {
        sapPort->slot->controller->setError(UMLRTController::E_SAPREG_NOT_SAP);
    }
    else
    {
        bool continueRegistration = false; // Set true if registration needs to be performed.

        char * registeredName = (char *)getPortToNameMap()->getObject(sapPort);
        if (registeredName != NULL)
        {
            // Already registered. See if it's registered to the same service.
            if (strcmp(registeredName, service))
            {
                // Was registered to an another SPP - deregister it first, then continue.
                deregisterSapPort(sapPort);
                continueRegistration = true;
            }
            // else it was already registered to this service and we can skip the remainder of registration.
        }
        else
        {
            // Wasn't registered.
            continueRegistration = true;
        }
        if (continueRegistration)
        {
            // Bind to SPP if possible.
            const UMLRTCommsPort * sppPort = (const UMLRTCommsPort *)getNameToSppPortMap()->getObject(service);
            bool bound = false;

            if (sppPort != NULL)
            {
                // SPP found. Attempt to bind.
                bound = UMLRTFrameService::bindServicePort(sapPort, sppPort);
            }
            if (!bound)
            {
                // SAP unable to bind to SPP. Queue for binding when an SPP port comes free.
                addSapToQueue(service, sapPort);
            }
            completeRegistration(sapPort, service);
        }
        ok = true;
    }
    BDEBUG(BD_SAP, "SAP register slot %s port %s %s %s\n", sapPort->slot->name, sapPort->role()->name, ok ? "ok" : "failed", ok ? "" : sapPort->slot->controller->strerror());
    return ok;
}

bool UMLRTProtocol::registerSAP( const char * service )
{
    UMLRTFrameService::rtsLock();

    bool ok = registerSapPort(srcPort, service);

    UMLRTFrameService::rtsUnlock();

    return ok;
}

/*static*/ bool UMLRTProtocol::registerSppPort( const UMLRTCommsPort * sppPort, const char * service )
{
    bool ok = false;

    if (!sppPort->spp)
    {
        sppPort->slot->controller->setError(UMLRTController::E_SPPREG_NOT_SPP);
    }
    else
    {
        // See if another port has this service registered already.
        const UMLRTCommsPort * otherSpp = (const UMLRTCommsPort *)getNameToSppPortMap()->getObject(service);
        if ((otherSpp != NULL) && (otherSpp != sppPort))
        {
            sppPort->slot->controller->setError(UMLRTController::E_SPPREG_PREVIOUS);
        }
        else if (otherSpp != NULL)
        {
            // This SPP was already registered under this service. Nothing more to do.
            ok = true;
        }
        else
        {
            char * registeredName = (char *)getPortToNameMap()->getObject(sppPort);
            if (registeredName != NULL)
            {
                // Already registered. See if it's registered to the same service.
                if (strcmp(registeredName, service))
                {
                    // Registered to an another service - de-register it.
                    deregisterSppPort(sppPort);
                }
            }
            // Complete the registration and we'll use the port registeredName for the name-to-spp-port map.
            completeRegistration(sppPort, service);

            // Make this the SPP service provider. Memory associated with the port.
            getNameToSppPortMap()->insert(sppPort->registeredName, (void *)sppPort);

            // Bind any queued SAP port.
            bindQueuedSapsToSpp(sppPort);

            ok = true;
        }
    }
    BDEBUG(BD_SAP, "SPP register slot %s port %s %s %s\n", sppPort->slot->name, sppPort->role()->name, ok ? "ok" : "failed", ok ? "" : sppPort->slot->controller->strerror());

    return ok;
}

bool UMLRTProtocol::registerSPP( const char * service )
{
    UMLRTFrameService::rtsLock();

    bool ok = registerSppPort( srcPort, service );

    UMLRTFrameService::rtsUnlock();

    return ok;
}

/*static*/ void UMLRTProtocol::removeSapFromQueue( const UMLRTCommsPort * sapPort )
{
    // Assumes RTS lock obtained.
    UMLRTQueue * unboundSapPorts = (UMLRTQueue *)getNameToUnboundSapPortsMap()->getObject(sapPort->registeredName);
    if (unboundSapPorts != NULL)
    {
        // This SPP had queued SAPs. Remove the de-registering SAP from the queue.
        unboundSapPorts->remove((UMLRTQueue::match_compare_t)removeSapFromQueueMatchCompare, (UMLRTQueue::match_notify_t)removeSapFromQueueMatchNotify, (void *)sapPort, true/*one*/);
        if (unboundSapPorts->isEmpty())
        {
            // If this was the last queued SAP, remove the queue and delete it. The name key has to be freed.
            char * key = (char *)getNameToUnboundSapPortsMap()->remove(sapPort->registeredName);
            if (key)
            {
                free(key);
            }
            delete unboundSapPorts;
        }
    }
}

/*static*/ bool UMLRTProtocol::removeSapFromQueueMatchCompare( UnboundSapPort * element, const UMLRTCommsPort * port )
{
    return element->port == port;
}

/*static*/ void UMLRTProtocol::removeSapFromQueueMatchNotify( UnboundSapPort * element, const UMLRTCommsPort * port )
{
    delete element;
}

/*static*/ void UMLRTProtocol::debugOutputPortToNameMap()
{
    // Assumes RTS lock obtained.
    BDEBUG(BD_MODEL, "SAP/SPP Port to Service Name Registration: { <slot>, <port>, <service name>, <SAP/SPP>}\n");

    getPortToNameMap()->Lock();
    UMLRTHashMap::Iterator iter = getPortToNameMap()->getIterator();

    if (iter == iter.end())
    {
        BDEBUG(BD_MODEL, "    No registered SAP/SPP ports.\n");
    }
    else
    {
        while (iter != iter.end())
        {
            const UMLRTCommsPort * port = (const UMLRTCommsPort *)iter.getKey();
            const char * service = (const char *)iter.getObject();
            BDEBUG(BD_MODEL, "    { %s, %s, %s, %s }\n",
                    port->slot->name,
                    port->role()->name,
                    port->registeredName == NULL ? "???" : port->registeredName,
                    port->spp ? "SPP" : (port->sap ? "SAP" : "???"));

            if (strcmp(service, port->registeredName))
            {
                FATAL("slot %s port %s registered name %s != port-to-name-map service %s",
                        port->slot->name,
                        port->role()->name,
                        port->registeredName,
                        service );
            }
            iter = iter.next();
        }
    }
    getPortToNameMap()->Unlock();
}

/*static*/ void UMLRTProtocol::debugOutputNameToSppPortMap()
{
    // Assumes RTS lock obtained.
    BDEBUG(BD_MODEL, "Service Name to SPP Port: { <service name>, <slot>, <port> }\n");

    getNameToSppPortMap()->Lock();
    UMLRTHashMap::Iterator iter = getNameToSppPortMap()->getIterator();

    if (iter == iter.end())
    {
        BDEBUG(BD_MODEL, "    No registered SPP ports.\n");
    }
    else
    {
        while (iter != iter.end())
        {
            const UMLRTCommsPort * port = (const UMLRTCommsPort *)iter.getObject();
            const char * service = (const char *)iter.getKey();
            BDEBUG(BD_MODEL, "    { %s, %s, %s }\n",
                    service,
                    port->slot->name,
                    port->role()->name);
            if (strcmp(service, port->registeredName))
            {
                FATAL("slot %s port %s registered name %s != name-to-spp-port-map service %s",
                        port->slot->name,
                        port->role()->name,
                        port->registeredName,
                        service );
            }
            iter = iter.next();
        }
    }
    getNameToSppPortMap()->Unlock();
}

/*static*/ bool UMLRTProtocol::debugOutputQueuedSap( UnboundSapPort * unboundSapPort, const char * service )
{
    // Assumes RTS lock obtained.
    const UMLRTCommsPort * port = unboundSapPort->port;

    if (port == NULL)
    {
        FATAL("found NULL port in nameToUnboundSapPortsMap queue");
    }

    BDEBUG(BD_MODEL, "        { %s, %s }\n", port->slot->name, port->role()->name);

    if (strcmp(service, port->registeredName))
    {
        FATAL("slot %s port %s registered name %s != unbound-sap-ports-map service %s",
                port->slot->name,
                port->role()->name,
                port->registeredName,
                service );
    }
    return false; // Never abort.
}

/*static*/ void UMLRTProtocol::debugOutputNameToUnboundSapPortsMap()
{
    // Assumes RTS lock obtained.
    BDEBUG(BD_MODEL, "Service Name to unbound SAP Ports:\n");

    getNameToUnboundSapPortsMap()->Lock();
    UMLRTHashMap::Iterator iter = getNameToUnboundSapPortsMap()->getIterator();

    if (iter == iter.end())
    {
        BDEBUG(BD_MODEL, "    No unbound SAP ports.\n");
    }
    else
    {
        while (iter != iter.end())
        {
            const char * service = (const char *)iter.getKey();
            UMLRTQueue * unboundSapPorts = (UMLRTQueue *)iter.getObject();
            BDEBUG(BD_MODEL, "    service '%s' queued SAP ports: { <slot>, <port> }\n", service );
            if (!unboundSapPorts->walk((UMLRTQueue::walk_callback_t)debugOutputQueuedSap, (void *)service))
            {
                BDEBUG(BD_MODEL, "        No queued SAP ports.\n");
            }
            iter = iter.next();
        }
    }
    getNameToUnboundSapPortsMap()->Unlock();
}

/*static*/ void UMLRTProtocol::debugOutputServiceRegistration()
{
    // Assumes RTS lock obtained.
    debugOutputPortToNameMap();
    debugOutputNameToSppPortMap();
    debugOutputNameToUnboundSapPortsMap();
}
