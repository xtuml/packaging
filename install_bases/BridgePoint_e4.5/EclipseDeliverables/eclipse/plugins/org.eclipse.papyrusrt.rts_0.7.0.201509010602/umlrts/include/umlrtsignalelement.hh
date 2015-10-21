// umlrtsignalelement.hh

/*******************************************************************************
* Copyright (c) 2014-2015 Zeligsoft (2009) Limited  and others.
* All rights reserved. This program and the accompanying materials
* are made available under the terms of the Eclipse Public License v1.0
* which accompanies this distribution, and is available at
* http://www.eclipse.org/legal/epl-v10.html
*******************************************************************************/

#ifndef UMLRTSIGNALELEMENT_HH
#define UMLRTSIGNALELEMENT_HH

#include <stdint.h>
#include "umlrtcommsport.hh"
#include "umlrtmutex.hh"
#include "umlrtobjectclass.hh"
#include "umlrtqueueelement.hh"

struct UMLRTCommsPort;

// Default-sized payload buffers are created when the signal is first
// used. Thereafter, the default-size payload buffer remains for future use
// (is never deallocated.)

// If the application requires more space than the default size, a temporary
// payload buffer is allocated from the heap. In this case, when the signal
// is 'freed', the temporary buffer is returned to the heap and the default
// payload buffer is restored.

class UMLRTSignalElement : public UMLRTQueueElement
{
public:
    UMLRTSignalElement ( );

    // Message priorities.
    // The code assumes 0 is lowest, and MAX-1 is largest legal value.
    enum Priorities
    {
        PRIORITY_BACKGROUND = 0,
        PRIORITY_LOW,
        PRIORITY_NORMAL,
        PRIORITY_HIGH,
        PRIORITY_PANIC,
        PRIORITY_SYSTEM,
        PRIORITY_SYNCHRONOUS,
        PRIORITY_MAX
    };
    typedef uint8_t Priority;
    typedef int Id;

    // Initialize the signal element and make sure the payload is large enough.
    void initialize ( const char * name, Id id, const UMLRTCommsPort * srcPort, size_t payloadSize, Priority priority = PRIORITY_NORMAL );

    // Basic initialization - no srcPort or signal id.
    void initialize ( const char * name, size_t payloadSize, Priority priority = PRIORITY_NORMAL );

    // Initialize the signal element with just and id and src port. Used for in-signals.
    void initialize ( const char * name, Id id, const UMLRTCommsPort * srcPort );

    // Deallocate a signal element.
    static void dealloc ( UMLRTSignalElement * element );

    // Signal source - used to get destination port during sends.
    const UMLRTCommsPort * getSrcPort ( ) const { return srcPort; }

    // Get the payload buffer.
    uint8_t *getPayload ( ) const { return payload; }

    // Size - what the user thinks is the size and what the actual buffer size is (Max).
    size_t getPayloadSize ( ) const { return appPayloadSize; }
    size_t getMaxPayloadSize ( ) const { return maxPayloadSize; }

    // Reference counting
    void incrementRefCount ( ) const;
    void decrementRefCount ( ) const;

    int getId ( ) const { return id; }

    const char * getName ( ) const { return name; }

    Priority getPriority ( ) const { return priority; }
    void setPriority ( Priority priority_) { priority = priority_; }

    // Encode 'data' buffer into payload.
    void encode ( const UMLRTObject_class * desc, const void * data, int arraySize = 1 );
    void encode ( const UMLRTObject_class * desc, int ptrIndirection, const void * data, int arraySize = 1 );

    // Decode payload into 'data' buffer.
    void decode ( const void * * decodeInfo, const UMLRTObject_class * desc, void * data, int arraySize = 1 );
    void decode ( const void * * decodeInfo, const UMLRTObject_class * desc, int ptrIndirection, void * data, int arraySize = 1 );

    // Pool allocation sanity check.
    void setAllocated ( bool allocated_ );

private:
    // This signal's ID.
    uint32_t id;

    // The src port for this signal.
    const UMLRTCommsPort * srcPort;

    // Signal name defined by signal initialization.
    const char * name;

    // Keep a copy of the default-sized payload buffer.
    uint8_t * defaultPayload;

    // User-data is serialized into payload buffer.
    // This buffer may be temporarily replaced with a larger buffer obtained
    // from the heap.
    uint8_t * payload;

    Priority priority;

    size_t appPayloadSize; // What user declared was the payload size.
    size_t maxPayloadSize; // Actual buffer size.

    // Set this true when a buffer larger than the default-size is allocated
    // for a signal and must be deallocated when the signal is returned to the
    // pool.
    bool nonDefaultPayload;

    // Set true when the signal is allocated (obtained) from the free-pool.
    // Set false when the signal is returned to the free-pool.
    // Used for sanity checks to detect access to uninitialized data.
    bool allocated;

    // Number of UMLRTSignal's referring to this element.
    mutable int refCount;

    // Mutex for reference counting.
    mutable UMLRTMutex refCountMutex;

    // Encoding information can be maintained internally (here) since there is always one encoder.
    // Decoding information must be maintained within the decoding code. See UMLRTSignal #decode documentation.
    void * encodeInfo;
};

#endif // UMLRTSIGNALELEMENT_HH
