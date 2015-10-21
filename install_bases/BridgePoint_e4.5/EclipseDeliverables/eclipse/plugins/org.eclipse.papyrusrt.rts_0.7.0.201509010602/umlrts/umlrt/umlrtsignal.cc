// umlrtsignal.cc

/*******************************************************************************
* Copyright (c) 2014-2015 Zeligsoft (2009) Limited  and others.
* All rights reserved. This program and the accompanying materials
* are made available under the terms of the Eclipse Public License v1.0
* which accompanies this distribution, and is available at
* http://www.eclipse.org/legal/epl-v10.html
*******************************************************************************/

#include <stdlib.h>
#include "umlrtapi.hh"
#include "umlrtcommsport.hh"
#include "umlrtcommsportfarend.hh"
#include "umlrtcontroller.hh"
#include "umlrtframeservice.hh"
#include "umlrtsignal.hh"
#include "umlrtsignalelement.hh"
#include "umlrtuserconfig.hh"
#include "basefatal.hh"
#include "basedebugtype.hh"
#include "basedebug.hh"

// See umlrtsignal.hh for documentation.

// No signal element by default - equivalent to the 'invalid signal'.
// An element is allocated when signal is 'initialized'.
UMLRTSignal::UMLRTSignal ( ) : element(NULL)
{
}

UMLRTSignal::~UMLRTSignal ( )
{
    if (element)
    {
        element->decrementRefCount();
    }
}

UMLRTSignal::UMLRTSignal ( const UMLRTSignal &signal ) : element(signal.element)
{
    if (element)
    {
        element->incrementRefCount();
    }
}

UMLRTSignal &UMLRTSignal::operator= ( const UMLRTSignal &signal )
{
    if (&signal != this)
    {
        if (element)
        {
            element->decrementRefCount();
        }
        if (signal.element)
        {
            signal.element->incrementRefCount();
        }
        element = signal.element;
    }
    return *this;
}

// Deprecated
bool UMLRTSignal::initialize ( Id id, const UMLRTCommsPort * srcPort, size_t payloadSize, UMLRTSignalElement::Priority priority )
{
    return initialize("(unnamed)", id, srcPort, payloadSize, priority);
}

bool UMLRTSignal::initialize ( const char * name, Id id, const UMLRTCommsPort * srcPort, size_t payloadSize, UMLRTSignalElement::Priority priority )
{
    BDEBUG(BD_SIGNAL, "initialize signal-qid[%d] name %s id %d for slot %s port %s payloadSize %d\n",
            getQid(),
            name,
            id,
            (srcPort != NULL) ? srcPort->slot->name : "(slot unknown)",
            (srcPort != NULL) ? srcPort->role()->name : "(no source port)",
            payloadSize);

    // Initialize for a wired port. If the srcPort is 'the unbound port', there is no need for a signal element with a payload
    // as nothing will be sent. The current protocol code pattern requires the element with a payload, so we allocate one.
    // When Bug 231 is resolved, we can avoid allocating an element and payload when the srcPort is 'the unbound port'.
    if (element)
    {
        FATAL("initializing signal that already has an element signal-qid[%d] allocated.", element->qid);
    }
    element = umlrt::SignalElementGetFromPool();

    if (!element)
    {
        FATAL("failed to allocate signal element");
    }
    element->initialize(name, id, srcPort, payloadSize, priority);

    if (srcPort == NULL)
    {
        FATAL("initializing a signal that has no srcPort - should at least be the 'unbound port'");
    }
    return !srcPort->unbound;
}

// Deprecated.
void UMLRTSignal::initialize ( size_t payloadSize, UMLRTSignalElement::Priority priority )
{
    initialize("(unnamed)", payloadSize, priority);
}

// Basic initialization. Currently used for initialize message to capsules.
void UMLRTSignal::initialize ( const char * name, size_t payloadSize, UMLRTSignalElement::Priority priority )
{
    if (element)
    {
        FATAL("initializing signal that already has an element allocated.");
    }
    element = umlrt::SignalElementGetFromPool();

    if (!element)
    {
        FATAL("failed to allocate signal element");
    }
    BDEBUG(BD_SIGNAL, "allocate no id, srcPort or payloadSize\n");

    element->initialize(name, payloadSize, priority);
}

// Deprecated
void UMLRTSignal::initialize ( Id id, const UMLRTCommsPort * srcPort )
{
    initialize("(unnamed)", id, srcPort);
}

void UMLRTSignal::initialize ( const char * name, Id id, const UMLRTCommsPort * srcPort )
{
    if (element)
    {
        FATAL("initializing signal that already has an element allocated.");
    }
    element = umlrt::SignalElementGetFromPool();

    if (!element)
    {
        FATAL("failed to allocate signal element");
    }
    BDEBUG(BD_SIGNAL, "allocate no id, srcPort or payloadSize\n");

    element->initialize(name, id, srcPort);
}

// Get the payload buffer.
uint8_t *UMLRTSignal::getPayload ( ) const
{
    if (!element)
    {
        return NULL;
    }
    return element->getPayload();
}

// Get the payload buffer size.
size_t UMLRTSignal::getPayloadSize ( ) const
{
    if (!element)
    {
        return 0;
    }
    return element->getPayloadSize();
}

const UMLRTCommsPort * UMLRTSignal::getSrcPort ( ) const
{
    if (!element)
    {
        return NULL;
    }
    return element->getSrcPort();
}

UMLRTSignal::Id UMLRTSignal::getId ( ) const
{
    if (!element)
    {
        return -1; // Another example of using '-1' when we don't have a signal id.
    }
    return element->getId();
}
