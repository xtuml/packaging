// umlrtsignal.hh

/*******************************************************************************
* Copyright (c) 2014-2015 Zeligsoft (2009) Limited  and others.
* All rights reserved. This program and the accompanying materials
* are made available under the terms of the Eclipse Public License v1.0
* which accompanies this distribution, and is available at
* http://www.eclipse.org/legal/epl-v10.html
*******************************************************************************/

#ifndef UMLRTSIGNAL_HH
#define UMLRTSIGNAL_HH

#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include "umlrtqueueelement.hh"
#include "umlrtsignalelement.hh"

// UMLRTSignal is the 'signal' that the application sends via ports.

// This class is used by protocols and user-code to create, define and send
// signals, however the underlying signal contents are maintained within a 'signal element'.

// Multiple UMLRTSignal objects may refer to the same 'signal element'. Allocation
// and deallocation of the underlying 'signal element' is managed by this class.

// A protocol allocates a signal (and its associated 'signal element') and
// user-data is serialized into the 'signal element' 'payload' field (variable-sized) buffer.

// Henceforth, the signal may be copied and used locally to send to multiple destinations.

// A send operation on the signal causes a message to be allocated and a
// reference to the signal is put in the message for enqueuing on the
// destination queue.

// While individual signals may be destroyed, the underlying 'signal element' is
// not deallocated until the last signal referring to the 'signal element' is destroyed.

struct UMLRTCommsPort;
struct UMLRTObject_class;

class UMLRTSignal : public UMLRTQueueElement
{
public:
    UMLRTSignal ( );
    UMLRTSignal ( const UMLRTSignal &signal );
    UMLRTSignal& operator= ( const UMLRTSignal &signal );
    ~UMLRTSignal ( );

    typedef int Id;
    enum { invalidSignalId = -1, rtBound = 0, rtUnbound, FIRST_PROTOCOL_SIGNAL_ID };

    // Initialize a signal from the pool. This is where the signal-element is allocated.
    // Before a signal is 'initialize'd, there is no element and the signal is deemed 'an invalid signal'.
    // Initialize element and make sure the payload is large enough.
    // Currently, a signal element (and its associated payload are always allocated to the signal).
    /*deprecated*/bool initialize ( Id id, const UMLRTCommsPort * srcPort, size_t payloadSize, UMLRTSignalElement::Priority priority = UMLRTSignalElement::PRIORITY_NORMAL );
    bool initialize ( const char * name, Id id, const UMLRTCommsPort * srcPort, size_t payloadSize, UMLRTSignalElement::Priority priority = UMLRTSignalElement::PRIORITY_NORMAL );

    // Initialize a signal with a payload and an (RTS-internal) id (i.e. -1) and no srcPort.
    // Currently used for a 'capsule initialization signl' and a 'timeout signal send (i.e. started via UMLRTTimerProtocol service port).
    /*deprecated*/void initialize ( size_t payloadSize, UMLRTSignalElement::Priority priority = UMLRTSignalElement::PRIORITY_NORMAL );
    void initialize ( const char * name, size_t payloadSize, UMLRTSignalElement::Priority priority = UMLRTSignalElement::PRIORITY_NORMAL );

    // Initialize a signal with the src port and its id. This is used for in-signals.
    /*deprecated*/void initialize ( Id id, const UMLRTCommsPort * srcPort );
    void initialize ( const char * name, Id id, const UMLRTCommsPort * srcPort );

    // Get the payload buffer. Returns NULL if the signal is invalid.
    uint8_t *getPayload ( ) const;

    // Gets what the user set the payload size to be (payload buffer will be >= this).
    size_t getPayloadSize ( ) const;

    // Signal source - used to get destination port during sends. Returns NULL if the signal is invalid.
    const UMLRTCommsPort * getSrcPort ( ) const;

    // Check whether this signal is an 'invalid signal'.
    bool isInvalid ( ) const { return element == NULL; }

    Id getId ( ) const; // Returns -1 if the signal is invalid.

    const char * getName ( ) const { return !element ? "(uninitialized signal)" : element->getName(); }

    UMLRTSignalElement::Priority getPriority ( ) const { return (element == NULL) ? UMLRTSignalElement::PRIORITY_NORMAL : element->getPriority(); }

    // For debugging signals
    int getQid ( ) const { return (element != NULL) ? element->qid : -1; }

protected:
    UMLRTSignalElement * element;
};

#endif // UMLRTSIGNAL_HH
