// umlrtcontroller.hh

/*******************************************************************************
* Copyright (c) 2014-2015 Zeligsoft (2009) Limited  and others.
* All rights reserved. This program and the accompanying materials
* are made available under the terms of the Eclipse Public License v1.0
* which accompanies this distribution, and is available at
* http://www.eclipse.org/legal/epl-v10.html
*******************************************************************************/

#ifndef UMLRTCONTROLLER_HH
#define UMLRTCONTROLLER_HH

// UMLRTController is the main controller-class.

#include "umlrtbasicthread.hh"
#include "umlrtslot.hh"
#include "umlrtcapsuleid.hh"
#include "umlrtprioritymessagequeue.hh"
#include "umlrttimerqueue.hh"
#include "umlrtcontrollercommand.hh"

struct UMLRTCommsPort;
class UMLRTMessagePool;
class UMLRTSignal;
class UMLRTSignalElementPool;
struct UMLRTTimer;
class UMLRTTimerPool;

class UMLRTController : UMLRTBasicThread
{
public:
    // Errors from the current thread can be obtained from the controller.
    // The error code is set by a failed RTS API.
    // The error is not reset by a successful RTS call.
    // Each API defines its own way of indicating whether the call failed an error code was set.
    // WARNING: These must align with strings in UMRTLCONTROLLER_ERROR_TO_STRING.
    typedef enum {
        E_OK = 0,
        E_DEFER_ALLOC,          // defer failed - unable to allocate message
        E_DEPORT_INVLD,         // attempt to deport an invalid capsule id
        E_DEPORT_NOINST,        // attempt to deport but capsule not found in part
        E_DEPORT_NONPLUG,       // attempt to deport from non-plugin slot
        E_DESTR_INVALID,        // attempt to destroy an invalid capsule id
        E_DESTR_NONOPT,         // attempt to destroy a non-optional slot
        E_IMPORT_COMPAT,        // import failed because the capsule ports could not be bound (compatibility)
        E_IMPORT_INV_IDX,       // import index invalid
        E_IMPORT_NO_FREE_SLOT,  // import no free slot
        E_IMPORT_NOINST,        // import failed due to invalid capsule instance specified
        E_IMPORT_NONPLUG,       // import into a non-plugin slot
        E_IMPORT_OCCUPIED,      // import failed because the slot was already occupied
        E_IMPORT_PARTREPL,      // import src part is replicated - if part specified, must be non-replicated.
        E_INC_COMPAT,           // port compatibility check failed
        E_INC_CTLR_NOT_FND,     // Incarnate specified a bad logical thread name.
        E_INC_DATATYPE,         // User specified a data pointer but no type.
        E_INC_INV_IDX,          // invalid index on incarnate
        E_INC_NO_FREE_SLOT,     // no free capsule slot on incarnate
        E_INC_NOT_OPT,          // attempting to incarnate in a non-optional slot
        E_INC_PARTS_ERROR,      // slot had parts already allocated - internal error
        E_INC_SLOT_OCC,         // capsule slot is occupied
        E_SAPDREG_NOT_SAP,      // attempt to deregister a port which is not a SAP.
        E_SAPDREG_SAP_LOCKED,   // attempt to deregister a locked service port.
        E_SAPREG_NOT_SAP,       // attempt to register a port which is not a SAP.
        E_SPPDREG_NOT_SPP,      // attempt to deregister a port which is not a SAP.
        E_SPPDREG_SPP_LOCKED,   // attempt to deregister a locked service port.
        E_SPPREG_PREVIOUS,      // attempt to register an SPP which already registered to a different port.
        E_SPPREG_NOT_SPP,       // attempt to register a port which is not a SPP.
        E_SEND_FROM_DSTR,       // send attempted from a slot designated for destruction
        E_SEND_INV_SIG,         // invalid send signal
        E_SEND_NO_CAP_INST,     // destination port has no capsule instance running
        E_SEND_NO_MSG_AVL,      // deliver no message available
        E_SEND_NO_PORT_INST,    // source port index greater than source port replication
        E_SEND_PRIORITY,        // signal priority invalid
        E_SEND_PRT_NOT_CON,     // send port not connected - port was bound but unconnected
        E_SEND_TO_DSTR,         // send attempted to a slot designated for destruction
        E_SEND_UNBOUND,         // signal send attempted on unbound port
        E_TIMER_GET,            // failed to get timer
        E_TIMER_NOT_ALLOC,      // timer not marked as allocated
        E_TIMER_CANC_INV,       // cancel invalid timer
        E_MAX                   // indicates maximum error code

    } Error;

#define UMLRTCONTROLLER_ERROR_CODE_TO_STRING  {\
    "no error", \
    "defer failed - unable to allocate message", \
    "deport failed - invalid capsule id", \
    "deport failed - capsule instance not found in part", \
    "deport failed - non-plugin slot", \
    "destroy failed - invalid capsule id", \
    "destroy failed - non-optional slot", \
    "import failed - port binding failed", \
    "import failed - invalid index", \
    "import failed - no free slot", \
    "import failed - capsule id invalid", \
    "import failed - non-plugin slot", \
    "import failed - slot occupied", \
    "import failed - replicated part", \
    "incarnate failed - port binding failed", \
    "incarnate failed - bad logical thread name", \
    "incarnate failed - data specified but no type specified", \
    "incarnate failed - invalid index", \
    "incarnate failed - no free capsule slot", \
    "incarnate failed - non-optional slot", \
    "incarnate failed - internal software error - slot parts already defined", \
    "incarnate failed - capsule slot already occuppied", \
    "SAP deregister failed - port not an SAP", \
    "SAP deregister failed - port registration is locked", \
    "SAP register failed - port not an SAP", \
    "SPP deregister failed - port not an SPP", \
    "SPP deregister failed - port registration is locked", \
    "SPP register failed - service already registered on a different port", \
    "SPP register failed - port not an SPP", \
    "send failed - send from a slot condemned for destruction", \
    "send failed - attempt to send invalid signal", \
    "send failed - destination port has no running capsule instance", \
    "send failed - failed to allocate msg resource", \
    "send failed - source port index exceeds available port far end instances", \
    "send failed - signal send priority is invalid", \
    "send failed - port was not connected", \
    "send failed - send to a slot condemned for destruction", \
    "send failed - signal send attempted on port not associated with a slot", \
    "timer failed - failed to allocate timer", \
    "timer failed - internal software error - timer not marked as allocated", \
    "timer failed - cannot cancel invalid timer", \
}

    UMLRTController ( const char * name_, size_t numSlot, UMLRTSlot slots_[] );
    UMLRTController ( const char * name_ );
    virtual ~UMLRTController ( ) { }

    // Abort the controller.
    void abort ( ) { enqueueAbort(); }

    // Cancel a timer (remove the timer from the controller's timer queue). Return true for success.
    bool cancelTimer ( const UMLRTTimerId id );

    // Purge messages and timers, etc. destined for a condemned slot
    void deallocateSlotResources ( UMLRTSlot * slot );

    // Callback to purge messages for a condemned slot.
    static bool deallocateMsgMatchCompare( UMLRTMessage * msg, UMLRTSlot * slot );

    // Callback when an individual message was purged - puts it back on the pool.
    static void deallocateMsgMatchNotify ( UMLRTMessage * msg, UMLRTSlot * slot );

    // Callback to purge timers for a condemned slot.
    static bool deallocateTimerMatchCompare ( UMLRTTimer * timer, UMLRTSlot * slot );

    // Callback when a timer is being purged from timer queue.
    static void deallocateTimerMatchNotify ( UMLRTTimer * timer, UMLRTSlot * slot );

    // Output the model structure for debugging. Is a synchronous call that blocks until the output is complete. Use enqueueDebugOutputModel to avoid blocking.
    void debugOutputModel ( const char * userMsg = NULL );

    // Deliver a signal to the destination port. Returns true if no error.
    bool deliver ( const UMLRTCommsPort * destPort, const UMLRTSignal & signal, size_t srcPortIndex );

    // Enqueue an abort command.
    void enqueueAbort ( );

    // Enqueue an abort command.
    void enqueueAbortAllControllers ( );

    // Enqueue a command - must send the notify to the target controller.
    void enqueueCommand ( const UMLRTControllerCommand & command );

    // Have a controller output the model for debugging. Does not block the requester.
    void enqueueDebugOutputModel ( const char * userMsg = NULL );

    // Enqueue a controller command.
    void enqueueDeport (  UMLRTSlot * slot );

    // Enqueue a controller command.
    void enqueueDestroy (  UMLRTSlot * slot, bool isTopSlot );

    // Request an import of a capsule into a slot.
    void enqueueImport ( UMLRTSlot * slot, UMLRTCapsule * capsule );

    // Request the assignment of an incarnated capsule to the controller.
    void enqueueIncarnate ( UMLRTCapsule * capsule, const void * userData, const UMLRTObject_class * type );

    // Returns true if the controller should abort.
    bool executeCommand ( UMLRTMessage * msg );

    // Get the last error.
    Error getError ( ) const { return lastError; }

    // Get the system-wide message pool.
    static UMLRTMessagePool * getMessagePool ( );

    const char * getName ( ) const { return name; }

    // Get the system-wide signal pool.
    static UMLRTSignalElementPool * getSignalElementPool ( );

   // Get the system-wide timer pool.
    static UMLRTTimerPool * getTimerPool ( );

    // Initialize the free signal, message and timer pools.
    static void initializePools ( UMLRTSignalElementPool * signalElementPool_, UMLRTMessagePool * messagePool_, UMLRTTimerPool * timerPool_ );

    // Tell caller whether they are running in this controller's context.
    bool isMyThread ( );

    // Wait for the controller thread to die.
    void join ( );

    // Output an error containing a user-defined message and the 'strerror()' string.
    void perror ( const char * fmt, ...) const;

    // Enqueue a recalled message to the capsuleQ.
    void recall ( UMLRTMessage * msg, bool front );

    // Create an intermediate buffer to hold serialized data and serialize the data into it.
    static void serializeIncarnateData ( const void * userData, const UMLRTObject_class * type, size_t * sizeSerializedData, void * * serializedData );

    // Set the error code.
    void setError ( Error error );

   // Start the controller thread.
    void spawn ( );

    // Start a timer (enqueue the timer on to the controller's timer queue).
    void startTimer ( const UMLRTTimer * timer );

    // Return the string associated with the last error code.
    const char * strerror ( ) const;

private:
    // Name for debugging.
    const char * const name;

    // Messages from other threads.
    UMLRTPriorityMessageQueue    incomingQueue;

    // Messages destined for contained capsules. This thread is the only
    // thread enqueuing and dequeuing from this queue.
    UMLRTPriorityMessageQueue    capsuleQueue;

    // Queue of running timers. Starts out empty.
    UMLRTTimerQueue timerQueue;

    // Queue of deferred messages.
    UMLRTMessageQueue deferredMessages;

    // Initial list of slots. Not all these are necessarily assigned to this controller.
    // By the time this controller runs, some slots may have dynamic instances defined, in which case
    // they need to be initialized via the dynamic capsule descriptor.
    size_t numSlots;
    UMLRTSlot * slots;

    // The application wide free-message pool.
    static UMLRTMessagePool * messagePool;

    // The application-wide free signal element pool.
    static UMLRTSignalElementPool * signalElementPool;

    // The application-wide free timer pool.
    static UMLRTTimerPool * timerPool;

    // Output the capsule nesting.
    void debugOutputSlotContainment ( const UMLRTSlot * slot, size_t nesting );

    // Output class inheritance.
    void debugOutputClassInheritance ( const UMLRTCapsuleClass * capsuleClass );

    // Output a port.
    void debugOutputModelPort ( const UMLRTCommsPort * ports, size_t index );

    // Output contents of a port's defer queue.
    void debugOutputModelPortDeferQueue ( const UMLRTCommsPort * port );
    static void debugOutputModelPortDeferQueueWalk ( const UMLRTMessage * msg, void *userData );

    // Output a ports - an array of ports
    void debugOutputModelPortsArray ( size_t numPorts, const UMLRTCommsPort * ports );

    // Output a port list - an array of port pointers.
    void debugOutputModelPortsList ( size_t numPorts, const UMLRTCommsPort * * ports );

    // Output the slots.
    void debugOutputSlots ( const UMLRTSlot * slot );

    // Main loop
    virtual void * run ( void * args );

    // Wait on either an external message or a timeout.
    void wait ( );

    // Last error - set by a failed RTS API which returns an error-indication back to the user.
    Error lastError;
};

#endif // UMLRTCONTROLLER_HH
