// umlrtsignalelement.cc

/*******************************************************************************
* Copyright (c) 2014-2015 Zeligsoft (2009) Limited  and others.
* All rights reserved. This program and the accompanying materials
* are made available under the terms of the Eclipse Public License v1.0
* which accompanies this distribution, and is available at
* http://www.eclipse.org/legal/epl-v10.html
*******************************************************************************/

#include <stdlib.h>
#include "basedebug.hh"
#include "basedebugtype.hh"
#include "basefatal.hh"
#include "umlrtapi.hh"
#include "umlrtslot.hh"
#include "umlrtcommsportrole.hh"
#include "umlrtguard.hh"
#include "umlrtobjectclass.hh"
#include "umlrtsignalelement.hh"
#include "umlrtuserconfig.hh"

// See umlrtsignalelement.hh for documentation.

UMLRTSignalElement::UMLRTSignalElement ( ) : id(0), srcPort(0), name(0), defaultPayload(0), payload(0), priority(PRIORITY_NORMAL), appPayloadSize(0),
            maxPayloadSize(USER_CONFIG_SIGNAL_DEFAULT_PAYLOAD_SIZE), nonDefaultPayload(false), allocated(false), refCount(0)
{
    // DEFAULT PAYLOAD SIZE IS A CONSTANT HERE, BUT HAS TO BE OBTAINED AT RUN-TIME.
    if (!(defaultPayload = payload = (uint8_t*)malloc(USER_CONFIG_SIGNAL_DEFAULT_PAYLOAD_SIZE)))
    {
        FATAL("(%p) payload malloc(%d)",this, USER_CONFIG_SIGNAL_DEFAULT_PAYLOAD_SIZE);
    }
    if (USER_CONFIG_SIGNAL_DEFAULT_PAYLOAD_SIZE > sizeof(int))
    {
        (*(int*)payload) = 0; // Initialize default rtdata location. We could zero the entire payload, but we don't bother.
    }
    BDEBUG(BD_SIGNALINIT, "(%p) signal element payload(%p)\n", this, payload);
}

// Initialize a signal from the pool and make sure the payload is large enough.

void UMLRTSignalElement::initialize ( const char * name_, size_t payloadSize_, Priority priority_ )
{
    BDEBUG(BD_SIGNAL, "[%p] initialize no id, srcPort or payloadSize\n", this);

    srcPort = NULL; // No srcPort by default.
    if (name_ == NULL)
    {
        FATAL("(%p) initialize name is NULL", this);
    }
    if (name != NULL)
    {
        // Free up previous name (if defined).
        free((void*)name);
    }
    name = strdup(name_);

    id = -1; // The signal id of '-1' used for 'uninitialized id value'.
    priority = priority_;

    if (refCount)
    {
        FATAL("(%p) signal element alloc refcount non-zero(%d)", this, refCount);
    }
    // If the user is requesting more space than is available in the
    // default payload buffer, allocate it here.

    // THIS CONSTANT MAY HAVE TO BE OBTAINED AT RUN-TIME FROM THE
    // GENERATED USER-CONFIGURATION PARAMETERS, IF THE VALUES FROM umlrtapi.hh ARE NOT USED.
    if (payloadSize_ > USER_CONFIG_SIGNAL_DEFAULT_PAYLOAD_SIZE)
    {
        if (!(payload = (uint8_t *)malloc(payloadSize_)))
        {
            FATAL("(%p) non-default payload malloc(%d)", this, payloadSize_ );
        }
        nonDefaultPayload = true;
        maxPayloadSize = payloadSize_;
    }
    appPayloadSize = payloadSize_;

    // Initialize encoding.
    encodeInfo = payload;

    incrementRefCount();
}
// Initialize a signal from the pool and make sure the payload is large enough.

void UMLRTSignalElement::initialize ( const char * name, Id id_, const UMLRTCommsPort * srcPort_, size_t payloadSize_, Priority priority_ )
{
    if (!srcPort_)
    {
        SWERR("(%p) signal allocate src port == 0.", this);
    }

    initialize(name, payloadSize_, priority_); // Default initialization.

    id = id_;
    srcPort = srcPort_;
    priority = priority_;
    
    BDEBUG(BD_SIGNAL, "[%p] initialize id(%d) %s(%s) appPayloadSize(%d) maxPayloadSize(%d)\n",
            this, id,
            (srcPort_ != NULL) ? srcPort->slot->name : "(NULL src port)",
            (srcPort_ != NULL) ? srcPort->role()->name : "(NULL src port)",
            appPayloadSize,
            maxPayloadSize);
}

void UMLRTSignalElement::initialize ( const char * name, Id id_, const UMLRTCommsPort * srcPort_ )
{
    if (!srcPort_)
    {
        SWERR("(%p) signal allocate src port == 0.", this);
    }

    initialize(name, 0/*payloadSize*/); // Default initialization.

    id = id_;
    srcPort = srcPort_;

    BDEBUG(BD_SIGNAL, "[%p] initialize id(%d) %s(%s) appPayloadSize(%d) maxPayloadSize(%d)\n",
            this, id,
            (srcPort_ != NULL) ? srcPort->slot->name : "(NULL src port)",
            (srcPort_ != NULL) ? srcPort->role()->name : "(NULL src port)",
            appPayloadSize,
            maxPayloadSize);
}

// Free a signal element (return to the pool).

/*static*/ void UMLRTSignalElement::dealloc ( UMLRTSignalElement * element )
{
    if (!element)
    {
        FATAL("element NULL");
    }
    BDEBUG(BD_SIGNAL, "[%p] deallocate id(%d) srcPort(%s) appPayloadSize(%d) maxPayloadSize(%d)\n",
            element, element->id, element->srcPort ? element->srcPort->role()->name : "NONE", element->appPayloadSize, element->maxPayloadSize);

    if (element->refCount)
    {
        FATAL("(%p) signal element dealloc refcount non-zero(%d)", element, element->refCount);
    }

    if (element->nonDefaultPayload)
    {
        // Return non-default payload.
        free(element->payload);

        // Restore default payload.
        element->payload = element->defaultPayload;
        element->maxPayloadSize = USER_CONFIG_SIGNAL_DEFAULT_PAYLOAD_SIZE;
        element->appPayloadSize = 0; // Should be unused while deallocated.
    }
    umlrt::SignalElementPutToPool(element);
}

void UMLRTSignalElement::incrementRefCount ( ) const
{
    BDEBUG(BD_SIGNALREF, "(%p) signal element inc ref count(%d)\n", this, refCount);

    UMLRTGuard g(refCountMutex);

    ++refCount;
}

void UMLRTSignalElement::decrementRefCount ( ) const
{
    BDEBUG(BD_SIGNALREF, "(%p) signal element dec ref count(%d)\n", this, refCount);

    UMLRTGuard g(refCountMutex);
    if (refCount <= 0)
    {
        FATAL("(%p) signal element dec ref count that is already zero id(%d)", this, id);
    }
    else if (!(--refCount))
    {
        BDEBUG(BD_SIGNALREF, "(%p) signal element dec ref count == 0 dealloc\n", this);

        UMLRTSignalElement::dealloc(const_cast<UMLRTSignalElement *>(this));
    }
}

// TODO debugging - to be removed.
extern uint8_t * globalSrc;
extern uint8_t * globalDst;

// See documentation in UMLRTSignal.
void UMLRTSignalElement::encode ( const UMLRTObject_class * desc, const void * data, int arraySize )
{
    // encodeInfo is intitialized to be 'payload' when the signal is initialized.
    // It is moved forward during successive #encode calls.

	globalSrc = (uint8_t *)data; // TODO: Remove this later. It's for debugging.
    globalDst = (uint8_t *)encodeInfo; // TODO: Remove this later. It's for debugging.

    for (int i = 0; i < arraySize; ++i)
    {
        encodeInfo = desc->copy(desc, data, encodeInfo);
        data = ((uint8_t*)data + desc->sizeDecoded);
    }
}

// See documentation in UMLRTSignal.
void UMLRTSignalElement::encode ( const UMLRTObject_class * desc, int ptrIndirection, const void * data, int arraySize )
{
    FATAL("encode ptr indirection not implemented.");
}

void UMLRTSignalElement::decode ( const void * * decodeInfo, const UMLRTObject_class * desc, void * data, int arraySize )
{
    // decodeInfo is initialized to be NULL by a decoder - it is owned by the decoder and not by
    // the signal (contrary to the encodeInfo, which can be owned by the signal, since there is always
    // only ever one encoder).

    if ((*decodeInfo) == NULL)
    {
        *decodeInfo = payload;
    }
	globalDst = (uint8_t *)data; // TODO: Remove this later. It's for debugging.
    globalSrc = (uint8_t *)(*decodeInfo); // TODO: Remove this later. It's for debugging.

    for (int i = 0; i < arraySize; ++i)
    {
        data = desc->copy(desc, *decodeInfo, data);
        *decodeInfo = (const void *)((uint8_t*)(*decodeInfo) + desc->sizeDecoded);
    }
}

// See documentation in UMLRTSignal.
void UMLRTSignalElement::decode ( const void * * decodeInfo, const UMLRTObject_class * desc, int ptrIndirection, void * data, int arraySize )
{
    FATAL("decode ptr indirection not implemented.");
}

void UMLRTSignalElement::setAllocated ( bool allocated_ )
{
    if (allocated_)
    {
        if (allocated)
        {
            FATAL("signal-qid[%d] was already allocated", qid);
        }
        allocated = true;
    }
    else
    {
        if (!allocated)
        {
            FATAL("signal-qid[%d] was already deallocated", qid);
        }
        allocated = false;
    }
}

