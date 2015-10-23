// umlrtcontroller.cc

/*******************************************************************************
* Copyright (c) 2014-2015 Zeligsoft (2009) Limited  and others.
* All rights reserved. This program and the accompanying materials
* are made available under the terms of the Eclipse Public License v1.0
* which accompanies this distribution, and is available at
* http://www.eclipse.org/legal/epl-v10.html
*******************************************************************************/

// UMLRTController is the main controller-class.

#include "basedebug.hh"
#include "basefatal.hh"
#include "umlrtapi.hh"
#include "umlrtcontroller.hh"
#include "umlrtcapsule.hh"
#include "umlrtslot.hh"
#include "umlrtcapsuletocontrollermap.hh"
#include "umlrtcapsulerole.hh"
#include "umlrtcapsulepart.hh"
#include "umlrtcommsport.hh"
#include "umlrtcommsportfarend.hh"
#include "umlrtcommsportrole.hh"
#include "umlrtcontrollercommand.hh"
#include "umlrtframeservice.hh"
#include "umlrtinmessage.hh"
#include "umlrtobjectclass.hh"
#include "umlrtprotocol.hh"
#include "umlrttimer.hh"
#include "umlrttimespec.hh"
#include "umlrtqueue.hh"
#include <stdlib.h>
#include <stdio.h>
#include "osselect.hh"
#include <string.h>
#include <stdarg.h>

// The application-wide free message pool.
/*static*/ UMLRTMessagePool * UMLRTController::messagePool = NULL;

// The application-wide free signal pool.
/*static*/ UMLRTSignalElementPool * UMLRTController::signalElementPool  = NULL;

// The application-wide free timer pool.
/*static*/ UMLRTTimerPool * UMLRTController::timerPool  = NULL;

// Error codes to string
static const char * errorToString[] = UMLRTCONTROLLER_ERROR_CODE_TO_STRING;


UMLRTController::UMLRTController (const char * name_, size_t numSlots_, UMLRTSlot slots_[] )
    : UMLRTBasicThread(name_), name(name_), incomingQueue(name), capsuleQueue(name), numSlots(numSlots_), slots(slots_), lastError(E_OK)
{
    // Register the controller with the capsule-to-controller map.
    UMLRTCapsuleToControllerMap::addController(name, this);
}

UMLRTController::UMLRTController ( const char * name_ )
    : UMLRTBasicThread(name_), name(name_), incomingQueue(name), capsuleQueue(name), numSlots(0), slots(NULL), lastError(E_OK)
{
    // Register the controller with the capsule-to-controller map.
    UMLRTCapsuleToControllerMap::addController(name, this);
}

bool UMLRTController::cancelTimer ( const UMLRTTimerId id )
{
    if (id.isValid())
    {
        const UMLRTTimer * timer = id.getTimer();

        char buf[UMLRTTimespec::TIMESPEC_TOSTRING_SZ];
        BDEBUG(BD_TIMER, "cancel timer(%p) destPort(%s) isInterval(%d) priority(%d) payloadSize(%d) due(%s)\n",
                timer, timer->destPort->role()->name, timer->isInterval, timer->signal.getPriority(), timer->signal.getPayloadSize(),
                timer->isInterval ? timer->due.toStringRelative(buf, sizeof(buf)) : timer->due.toString(buf, sizeof(buf)));
    }
    return timerQueue.cancel(id);
}

/*static*/ bool UMLRTController::deallocateMsgMatchCompare ( UMLRTMessage * msg, UMLRTSlot * slot )
{
    return !msg->isCommand && msg->destSlot == slot;
}

/*static*/ void UMLRTController::deallocateMsgMatchNotify ( UMLRTMessage * msg, UMLRTSlot * slot )
{
    BDEBUG(BD_DESTROY, "Purging message destined for slot %s\n", slot->name);

    umlrt::MessagePutToPool(msg);
}

// Callback to purge timers for a condemned slot.
/*static*/ bool UMLRTController::deallocateTimerMatchCompare ( UMLRTTimer * timer, UMLRTSlot * slot )
{
    return timer->destSlot == slot;
}

// Callback when a timer is being deleted.
/*static*/ void UMLRTController::deallocateTimerMatchNotify ( UMLRTTimer * timer, UMLRTSlot * slot )
{
    BDEBUG(BD_DESTROY, "Purging timer destined for slot %s\n", slot->name);

    umlrt::TimerPutToPool(timer);
}

void UMLRTController::deallocateSlotResources ( UMLRTSlot * slot )
{
    if (slot == NULL)
    {
        FATAL("attempting to deallocate the NULL slot.");
    }
    BDEBUG(BD_DESTROY, "Remove messages and timers for slot %s\n", slot->name);

    incomingQueue.remove( (UMLRTQueue::match_compare_t)deallocateMsgMatchCompare, (UMLRTQueue::match_notify_t)deallocateMsgMatchNotify, slot );
    capsuleQueue.remove( (UMLRTQueue::match_compare_t)deallocateMsgMatchCompare, (UMLRTQueue::match_notify_t)deallocateMsgMatchNotify, slot );
    timerQueue.remove( (UMLRTQueue::match_compare_t)deallocateTimerMatchCompare, (UMLRTQueue::match_notify_t)deallocateTimerMatchNotify, slot );
}

// Deliver a signal to the destination port.
bool UMLRTController::deliver ( const UMLRTCommsPort * destPort, const UMLRTSignal &signal, size_t srcPortIndex )
{
    // Assumes global RTS lock acquired.

    UMLRTMessage * msg = umlrt::MessageGetFromPool();
    bool ok = false;

    if (!msg)
    {
        signal.getSrcPort()->slot->controller->setError(E_SEND_NO_MSG_AVL);
    }
    else if (destPort == NULL)
    {
        FATAL("Message from slot %s (port %s) has destination port NULL.", signal.getSrcPort()->slot->name, signal.getSrcPort()->role()->name);
    }
    else
    {
        // Look up sapIndex0 so receiver knows which index in their replicated port the message was received on.
        msg->sapIndex0 = signal.getSrcPort()->farEnds[srcPortIndex].farEndIndex;
        msg->signal = signal;
        msg->destPort = destPort;
        msg->destSlot = destPort->slot;
        msg->srcPortIndex = srcPortIndex;
        msg->isCommand = false;

        // Source port may not exist.
        BDEBUG(BD_SIGNALALLOC, "%s: deliver signal-qid[%d] id(%d) -> %s(%s[%d]) payloadSize(%d)\n",
                name,
                msg->signal.getQid(),
                msg->signal.getId(),
                msg->destPort->slot->name, msg->destPort->role()->name, msg->sapIndex0,
                msg->signal.getPayloadSize());

        if (isMyThread())
        {
            // If this is me delivering the message, I can deliver directly to my capsule queues.
            capsuleQueue.enqueue(msg);
        }
        else
        {
            // Otherwise, I deliver to the remote capsule's incoming queue.
            incomingQueue.enqueue(msg);
        }
        ok = true;
    }
    return ok;
}

void UMLRTController::enqueueAbort ( )
{
    UMLRTControllerCommand command;

    // Explicitly reset unused command contents.
    command.capsule = NULL;
    command.isTopSlot = false;
    command.serializedData = NULL;
    command.sizeSerializedData = 0;
    command.slot = NULL;
    command.userMsg = NULL;

    // Format command and enqueue it.
    command.command = UMLRTControllerCommand::ABORT;
    enqueueCommand(command);
}

void UMLRTController::enqueueAbortAllControllers ( )
{
    UMLRTCapsuleToControllerMap::enqueueAbortAllControllers();
}

void UMLRTController::enqueueCommand ( const UMLRTControllerCommand & command )
{
    UMLRTMessage *msg = umlrt::MessageGetFromPool();

    msg->signal.initialize("ControllerCommand", sizeof(UMLRTControllerCommand), UMLRTSignalElement::PRIORITY_SYSTEM);
    uint8_t * payload;

    BDEBUG(BD_SIGNALALLOC, "%s: deliver signal-qid[%d] as command %d\n",
            name,
            msg->signal.getQid(),
            command.command);

    if ((payload = msg->signal.getPayload()) == NULL)
    {
        FATAL("initialized signal had no payload.");
    }
    memcpy( payload, &command, sizeof(command));
    msg->isCommand = true;

    if (isMyThread())
    {
        // If this is me delivering the message, I can deliver directly to my capsule queues.
        capsuleQueue.enqueue(msg);
    }
    else
    {
        // Otherwise, I deliver to the remote capsule's incoming queue.
        incomingQueue.enqueue(msg);
    }
}

void UMLRTController::enqueueDebugOutputModel ( const char * userMsg )
{
    UMLRTControllerCommand command;

    // Explicitly reset unused command contents.
    command.capsule = NULL;
    command.isTopSlot = false;
    command.serializedData = NULL;
    command.sizeSerializedData = 0;
    command.slot = NULL;

    // Format command and enqueue it.
    command.command = UMLRTControllerCommand::DEBUG_OUTPUT_MODEL;
    command.userMsg = (userMsg == NULL) ? NULL : strdup(userMsg);

    enqueueCommand(command);
}

void UMLRTController::enqueueDeport ( UMLRTSlot * slot )
{
    UMLRTControllerCommand command;

    // Explicitly reset unused command contents.
    command.capsule = NULL;
    command.isTopSlot = false;
    command.serializedData = NULL;
    command.sizeSerializedData = 0;
    command.userMsg = NULL;

    // Format command and enqueue it.
    command.command = UMLRTControllerCommand::DEPORT;
    command.slot = slot;

    enqueueCommand(command);
}

void UMLRTController::enqueueDestroy ( UMLRTSlot * slot, bool isTopSlot )
{
    UMLRTControllerCommand command;

    // Explicitly reset unused command contents.
    command.capsule = NULL;
    command.serializedData = NULL;
    command.sizeSerializedData = 0;
    command.userMsg = NULL;

    // Format command and enqueue it.
    command.command = UMLRTControllerCommand::DESTROY;
    command.slot = slot;
    command.isTopSlot = isTopSlot;

    enqueueCommand(command);
}

void UMLRTController::enqueueImport ( UMLRTSlot * slot, UMLRTCapsule * capsule )
{
    UMLRTControllerCommand command;

    // Explicitly reset unused command contents.
    command.isTopSlot = false;
    command.serializedData = NULL;
    command.sizeSerializedData = 0;
    command.userMsg = NULL;

    // Format command and enqueue it.
    command.command = UMLRTControllerCommand::IMPORT;
    command.slot = slot;
    command.capsule = capsule;

    enqueueCommand(command);
}

void UMLRTController::enqueueIncarnate ( UMLRTCapsule * capsule, const void * userData, const UMLRTObject_class * type )
{
    UMLRTControllerCommand command;

    // May be called from within the context of another controller thread.

    // Explicitly reset unused command contents.
    command.isTopSlot = false;
    command.slot = NULL;
    command.userMsg = NULL;

    // Format command and enqueue it.
    command.command = UMLRTControllerCommand::INCARNATE;
    command.capsule = capsule;

    serializeIncarnateData( userData, type, &command.sizeSerializedData, &command.serializedData);
    // Controller frees serialized data buffer back to the heap.

    command.sizeSerializedData = 0;
    command.serializedData = NULL;

    // Notification to destination controller occurs as a result of this enqueue.
    enqueueCommand(command);
}

// Return true if abort was received.
bool UMLRTController::executeCommand ( UMLRTMessage * msg )
{
    bool abort = false;
    UMLRTControllerCommand * command = (UMLRTControllerCommand *)msg->signal.getPayload();

    if (command != NULL)
    {
        switch (command->command)
        {
        case UMLRTControllerCommand::ABORT:
            BDEBUG(BD_COMMAND, "%s: ABORT command received\n", getName());
            abort = true;
            break;

        case UMLRTControllerCommand::DEBUG_OUTPUT_MODEL:
            BDEBUG(BD_COMMAND, "%s: DEBUG_OUTPUT_MODEL command received\n", getName());
            debugOutputModel(command->userMsg);
            if (command->userMsg != NULL)
            {
                free((void*)command->userMsg);
            }
            break;

        case UMLRTControllerCommand::DEPORT:
            BDEBUG(BD_COMMAND, "%s: DEPORT from slot %s command received\n", getName(), command->slot->name);
            UMLRTFrameService::controllerDeport(command->slot, false/*synchronous*/, false/*lockAcquired*/);
            break;

        case UMLRTControllerCommand::DESTROY:
            BDEBUG(BD_COMMAND, "%s: DESTROY from slot %s (is %stop slot) command received\n", getName(), command->slot->name, command->isTopSlot ? "" : "NOT ");
            UMLRTFrameService::controllerDestroy(command->slot, command->isTopSlot, false/*synchronous*/, false/*lockAcquired*/);
            break;

        case UMLRTControllerCommand::IMPORT:
            BDEBUG(BD_COMMAND, "%s: IMPORT capsule %s to slot %s command received\n", getName(), command->capsule->name(), command->slot->name);
            UMLRTFrameService::controllerImport(command->slot, command->capsule, false/*synchronous*/, false/*lockAcquired*/);
            break;

        case UMLRTControllerCommand::INCARNATE:
            BDEBUG(BD_COMMAND, "%s: INCARNATE capsule %s (size data %d) command received\n", getName(), command->capsule->name(), command->sizeSerializedData);
            UMLRTFrameService::controllerIncarnate(command->capsule, command->sizeSerializedData, command->serializedData);
            break;

        case UMLRTControllerCommand::UNDEFINED:
        default:
            FATAL("%s:unknown controller (%d) command received", getName(), command->command);
        }
    }
    return abort;
}

// Initialize the free pools.
/*static*/ void UMLRTController::initializePools ( UMLRTSignalElementPool * signalElementPool_, UMLRTMessagePool * messagePool_,
        UMLRTTimerPool * timerPool_ )
{
    signalElementPool = signalElementPool_;
    messagePool = messagePool_;
    timerPool = timerPool_;
}

// Wait for the controller thread to die.
void UMLRTController::join ( )
{
    UMLRTBasicThread::join();
}

// Get the system-wide signal pool.
/*static*/ UMLRTSignalElementPool * UMLRTController::getSignalElementPool ( )
{
    return signalElementPool;
}

// Get the system-wide message pool.
/*static*/ UMLRTMessagePool * UMLRTController::getMessagePool ( )
{
    return messagePool;
}

// Get the system-wide timer pool.
/*static*/ UMLRTTimerPool * UMLRTController::getTimerPool ( )
{
    return timerPool;
}

bool UMLRTController::isMyThread ( )
{
    return UMLRTBasicThread::isMyThread();
}

// Output an error containing a user-defined message and the 'strerror()' string.
void UMLRTController::perror ( const char * fmt, ... ) const
{
    va_list ap;
    va_start(ap, fmt);
    vprintf(fmt, ap);
    printf(": %s\n", strerror());
    va_end(ap);
}

// Deliver a signal to the destination port.
void UMLRTController::recall ( UMLRTMessage * msg, bool front )
{
    capsuleQueue.enqueue(msg, front);
}

// Main loop
void * UMLRTController::run ( void * args )
{
    printf("Controller \"%s\" running.\n", getName());

    if (slots == NULL)
    {
        numSlots = UMLRTCapsuleToControllerMap::getDefaultSlotList( &slots );
    }
    for (size_t i = 0; (i < numSlots) && (slots != NULL); ++i)
    {
        if (slots[i].controller == this)
        {
            if ((slots[i].capsule != NULL) && (!slots[i].condemned))
            {
                if (   (slots[i].role() == NULL)
                    || ((slots[i].role()->optional == 0) && (slots[i].role()->plugin == 0)))
                {
                    BDEBUG(BD_INSTANTIATE, "%s: initialize capsule %s (class %s).\n", getName(), slots[i].name, slots[i].capsuleClass->name );

                    UMLRTFrameService::initializeCapsule(slots[i].capsule, 0, NULL);
                }
            }
        }
    }

    bool abort = false;
    while (!abort)
    {
        // Queue messages associated with all timed-out timers.
        capsuleQueue.queueTimerMessages(&timerQueue);

        // Transfer all incoming messages to capsule queues.
        capsuleQueue.moveAll(incomingQueue);

        // Inject all available messages, highest priority msgs first.
        UMLRTInMessage * msg;
        while (!abort && (msg = capsuleQueue.dequeueHighestPriority()) != NULL)
        {
            if (msg->isCommand)
            {
                abort = executeCommand(msg);
            }
            else if (msg->destSlot->capsule == NULL)
            {
                FATAL("%s: signal id(%d) to slot %s (no capsule instance) should not occur\n",
                        getName(), msg->signal.getId(), msg->destSlot->name);
            }
            else
            {
                if (msg->destSlot->condemned)
                {
                    // Drop messages to a condemned slot.
                    BDEBUG(BD_INJECT, "%s: dropping signal-qid[%d] id(%d)(%s) to slot %s (slot condemned)\n",
                            getName(), msg->signal.getQid(), msg->getSignalId(), msg->getSignalName(), msg->destSlot->name);
                }
                else
                {
                    // Source port may no longer exist.
                    BDEBUG(BD_INJECT, "%s: inject signal-qid[%d] into %s(role %s, class %s) {%s[%d]} id %d(%s)\n",
                            getName(), msg->signal.getQid(), msg->destSlot->capsule->name(), msg->destSlot->capsule->getName(),
                            msg->destSlot->capsule->getTypeName(), msg->destPort->role()->name, msg->sapIndex0, msg->signal.getId(), msg->getSignalName());

                    base::debugLogData( BD_SIGNALDATA, msg->signal.getPayload(), msg->signal.getPayloadSize());

                    // Set capsule message for this inject.
                    msg->destPort->slot->capsule->msg = msg;

                    // Inject the signal into the capsule.
                    msg->destPort->slot->capsule->inject((UMLRTInMessage)*msg);
                }
            }
            // Put the message back in the pool (handles signal allocation also).
            umlrt::MessagePutToPool(msg);
        }
        // Wait on the incoming queue or a timeout.
        if (!abort)
        {
            wait();
        }
    }
    // Bug 468521 - must destroy owned capsules + slots here.

    // Remove this controller from the controller list.
    UMLRTCapsuleToControllerMap::removeController(getName());

    // Leave this output in here for now.
    printf("Controller %s is aborting.\n", getName());

    return(NULL);
}

/*static*/ void UMLRTController::serializeIncarnateData ( const void * userData, const UMLRTObject_class * type, size_t * sizeSerializedDataP, void * * serializedDataP )
{
    (*sizeSerializedDataP) = 0;
    (*serializedDataP) = NULL;

    if (userData != NULL)
    {
        if (type == NULL)
        {
            FATAL("Type descriptor missing. Has been previously checked.");
        }
        // Define data pointer and size.
        if (((*serializedDataP) = malloc((*sizeSerializedDataP) = type->getSize(type))) == NULL)
        {
            FATAL("could not allocate memory for serialized data.");
        }
        // Encode the data.
        type->encode(type, userData, (*serializedDataP), 0);
    }
    // The controller frees the serialized data buffer after it's been copied to the initialize message.
}

// Set the error code.
void UMLRTController::setError ( Error error )
{
    lastError = error;
    if ((error != E_OK) && (base::debugGetEnabledTypes() & (1 << BD_ERROR)) != 0)
    {
        BDEBUG(BD_ERROR, "Controller %s setError(%d):'%s'\n", getName(), lastError, strerror());
    }
}

// Start the controller thread.
void UMLRTController::spawn ( )
{
    // No arguments for this thread.
    start(NULL);
}

// See umlrtcontroller.hh.
const char * UMLRTController::strerror ( ) const
{
    Error error = getError();
    if ((error < 0) || (error >= E_MAX))
    {
        SWERR("error code(%d) out of range max(%d)", error, E_MAX);
        return "unknown error code";
    }
    else
    {
        return errorToString[error];
    }
}

void UMLRTController::startTimer ( const UMLRTTimer * timer )
{
    char buf[UMLRTTimespec::TIMESPEC_TOSTRING_SZ];
    BDEBUG(BD_TIMER, "start timer(%p) destPort(%s) isInterval(%d) priority(%d) payloadSize(%d) due(%s)\n",
            timer, timer->destPort->role()->name, timer->isInterval, timer->signal.getPriority(), timer->signal.getPayloadSize(),
            timer->isInterval ? timer->due.toStringRelative(buf, sizeof(buf)) : timer->due.toString(buf, sizeof(buf)));

    timerQueue.enqueue(timer);
}

// Wait on an external message or a timeout.
void UMLRTController::wait ( )
{
    // Linux-specific implementation for the time-being.

    // If there is a timer running, this holds the remaining time as the timeout of the select.
    struct timeval remainTimeval;

    // We default to 'wait forever', unless a timer is running.
    struct timeval * selectTimeval = NULL;

    bool wait = true; // Set this false if a timer is due or an incoming message appeared.

    // Get the time remaining on the first timer in the queue (if one exists).
    if (!timerQueue.isEmpty())
    {
        UMLRTTimespec remainTimespec = timerQueue.timeRemaining();

        if (remainTimespec.isZeroOrNegative())
        {
            // The timer is due - don't wait.
            wait = false;
            BDEBUG(BD_TIMER, "%s:timer is due\n", name);
        }
        else
        {
            // A timer is waiting but is not yet due. Set up the timeout. Will be non-zero.
            remainTimeval.tv_sec = remainTimespec.tv_sec;
            remainTimeval.tv_usec = remainTimespec.tv_nsec / 1000;

            char tmbuf[UMLRTTimespec::TIMESPEC_TOSTRING_SZ];
            BDEBUG(BD_TIMER, "%s: timer is not due - remain(%s)\n", name, remainTimespec.toStringRelative(tmbuf, sizeof(tmbuf)));

            // Select will not wait forever.
            selectTimeval = &remainTimeval;
        }
    }
    if (!incomingQueue.isEmpty())
    {
        BDEBUG(BD_CONTROLLER, "%s: incoming q non-empty\n", name);
        wait = false;
    }
    if (wait)
    {
        // selectTimeval remains NULL if no timers are running. In that case, select will wait
        // forever until a message is delivered or a new timer is added to the timer-queue.

        // Get the queue notification file descriptors.
        int msgNotifyFd = incomingQueue.getNotifyFd();
        int timerFd = timerQueue.getNotifyFd();

        fd_set fds;

        FD_ZERO(&fds);
        FD_SET(msgNotifyFd, &fds);
        FD_SET(timerFd, &fds);

        // select wants to know the highest-numbered fd + 1.
        int nfds = msgNotifyFd + 1;
        if (timerFd > msgNotifyFd)
        {
            nfds = timerFd + 1;
        }

        // TODO - Bug 238 - DEBUG - remove this later.
        if (!selectTimeval)
        {
            // DEBUG - the intent is to wait forever until notification, since no timer is pending.
            // However, we wake the process up every 10 seconds during debugging.
            // No harm in doing that - the controller returns here if it finds nothing to do.
            remainTimeval.tv_sec = 10;
            remainTimeval.tv_usec = 0;
            selectTimeval = &remainTimeval;
        }
        // end DEBUG - remove this later

        BDEBUG(BD_CONTROLLER, "%s: call select msgfd(%d) timeout[%d,%d]\n",
                name,
                msgNotifyFd,
                selectTimeval ? selectTimeval->tv_sec : -1,
                selectTimeval ? selectTimeval->tv_usec : -1);


        if ((select(nfds, &fds, NULL, NULL, selectTimeval)) < 0)
        {
            FATAL_ERRNO("select");
        }
        if (FD_ISSET(msgNotifyFd, &fds))
        {
            // Clear message notify-pending.
            incomingQueue.clearNotifyFd();
        }
        if (FD_ISSET(timerFd, &fds))
        {
            // Clear message notify-pending.
            timerQueue.clearNotifyFd();
        }
    }
}

/*static*/ void UMLRTController::debugOutputModelPortDeferQueueWalk ( const UMLRTMessage * msg, void *userData )
{
    int rtdata = *(int *)msg->signal.getPayload();
    size_t size = msg->signal.getPayloadSize();

    BDEBUG(BD_MODEL, "                            msg: id(%d) data [%d] %s\n",
            msg->signal.getId(),
            (size < sizeof(rtdata)) ? 0 : rtdata, (size < sizeof(rtdata)) ? "undef" : "");
}

void UMLRTController::debugOutputModelPortDeferQueue ( const UMLRTCommsPort * port )
{
    if (port != NULL)
    {
        if (port->deferQueue != NULL)
        {
            if (!port->deferQueue->isEmpty())
            {
                BDEBUG(BD_MODEL,
                        "                        defer queue:\n");
                port->deferQueue->walk( (UMLRTQueue::walk_callback_t)debugOutputModelPortDeferQueueWalk, NULL);
            }
        }
    }
}

void UMLRTController::debugOutputModelPort ( const UMLRTCommsPort * port, size_t index )
{
    if (!port)
    {
        BDEBUG(BD_MODEL,
"            port [%u]: (null)\n", index);
    }
    else
    {
        BDEBUG(BD_MODEL,
"            %s[%u] (id %d) %s%s %s%s%s%s%s%s%s%s%s%s%s%s%s\n",
            port->role()->name,
            port->role()->numFarEnd,
            port->role()->id,
            port->role()->protocol,
            port->role()->conjugated ? "~" : "",
            port->proxy ? "(proxy)" : "",
            port->spp ? "(SPP " : "",
            port->spp ? ((port->registeredName == NULL) ? "?" : port->registeredName) : "",
            port->spp ? ")" : "",
            port->sap ? "(SAP " : "",
            port->sap ? ((port->registeredName == NULL) ? "?" : port->registeredName) : "",
            port->sap ? ")" : "",
            port->automatic ? "(auto)" : "",
            port->notification ? "(notify)" : "",
            (port->unbound) ? "('unbound')" : "",
            port->relay ? "(relay)" : "",
            port->wired ? "(wired)" : "",
            port->locked ? "(locked)" : "");

        for (size_t j = 0; j < port->numFarEnd; ++j)
        {
            const char * farEndSlotName = "(none)";
            const char * farEndPortName = "(none)";
            const char * farEndSlotClass = "(no instance)";
            size_t farEndIndex = 0;
            if (port->farEnds[j].port != NULL)
            {
                farEndSlotName = port->farEnds[j].port->slot->name;
                farEndPortName = port->farEnds[j].port->role()->name;
                farEndIndex = port->farEnds[j].farEndIndex;
                if (port->farEnds[j].port->slot->capsule != NULL)
                {
                    farEndSlotClass = port->farEnds[j].port->slot->capsule->getClass()->name;
                }
            }
            BDEBUG(BD_MODEL, "                    farEnd[%u] : -> { slot %s, port %s[%u] %s}\n",
                    j, farEndSlotName, farEndPortName, farEndIndex, farEndSlotClass );
        }
        debugOutputModelPortDeferQueue(port);
    }
}

void UMLRTController::debugOutputModelPortsArray ( size_t numPorts, const UMLRTCommsPort * ports )
{
    if (numPorts == 0)
    {
        BDEBUG(BD_MODEL,
"                ports        : (none)\n");
    }
    for (size_t i = 0; i < numPorts; ++i)
    {
        debugOutputModelPort(&ports[i], i);
    }
}

void UMLRTController::debugOutputModelPortsList ( size_t numPorts, const UMLRTCommsPort * * ports )
{
    if (numPorts == 0)
    {
        BDEBUG(BD_MODEL,
"                ports        : (none)\n");
    }
    for (size_t i = 0; i < numPorts; ++i)
    {
        debugOutputModelPort(ports[i], i);
    }
}

void UMLRTController::debugOutputClassInheritance ( const UMLRTCapsuleClass * capsuleClass )
{
    const UMLRTCapsuleClass * parent = capsuleClass->super;

    if (parent == NULL)
    {
        BDEBUG(BD_MODEL, "(none)");
    }
    while (parent != NULL)
    {
        BDEBUG(BD_MODEL, "-> %s ", parent->name);
        parent = parent->super;
    }
    BDEBUG(BD_MODEL,"\n");
}

void UMLRTController::debugOutputSlots ( const UMLRTSlot * slot )
{
    BDEBUG(BD_MODEL,
"    %s:\n", slot->name);
    BDEBUG(BD_MODEL,
"        capsule instance        : %-30s (%p)\n", slot->capsule ? slot->capsule->name() : "(none)", slot->capsule);
    if (slot->role() == NULL)
    {
    BDEBUG(BD_MODEL,
"        role                    : (none)\n");
    }
    else
    {
        BDEBUG(BD_MODEL,
"        role                    : %-30s [%d..%d] %s %s\n",
                slot->role()->name,
                slot->role()->multiplicityLower,
                slot->role()->multiplicityUpper,
                slot->role()->optional ? "optional " : "",
                slot->role()->plugin ? "plugin " : "");

    }
    BDEBUG(BD_MODEL,
"        index                   : %d\n", slot->capsuleIndex);
    BDEBUG(BD_MODEL,
"        class                   : %-30s (# sub-capsule roles : %u) (# border ports : %u) (# internal ports : %u)\n",
            slot->capsuleClass->name,
            slot->capsuleClass->numSubcapsuleRoles,
            slot->capsuleClass->numPortRolesBorder,
            slot->capsuleClass->numPortRolesInternal);
    BDEBUG(BD_MODEL,
"        class inheritance       : ");
    debugOutputClassInheritance(slot->capsuleClass);
    BDEBUG(BD_MODEL,
"        controller              : %s\n", slot->controller->getName());
    if (slot->slotToBorderMap == NULL)
    {
        BDEBUG(BD_MODEL,
"        slot to border map      : (none)\n");
    }
    else
    {
        BDEBUG(BD_MODEL,
"        slot to border map      :");
        for (size_t i = 0; i < slot->role()->capsuleClass->numPortRolesBorder; ++i)
        {
            BDEBUG(BD_MODEL, " [sl %lu=cp %d]", i, slot->slotToBorderMap[i]);
        }
        BDEBUG(BD_MODEL,"\n");
    }
    if (slot->numPorts == 0)
    {
        BDEBUG(BD_MODEL,
"        slot ports              : (none)\n");
    }
    else
    {
        BDEBUG(BD_MODEL,
"        slot ports              :\n");
        debugOutputModelPortsArray(slot->numPorts, slot->ports);
    }
    const UMLRTCommsPort * * internalPorts;
    const UMLRTCommsPort * * borderPorts;
    if (slot->capsule == NULL)
    {
        BDEBUG(BD_MODEL,
"        capsule border ports    : (none - no instance running in slot)\n");
    }
    else if ((borderPorts = slot->capsule->getBorderPorts()) == NULL)
    {
        BDEBUG(BD_MODEL,
"        capsule border ports    : (no border ports)\n");
    }
    else if (slot->capsuleClass->numPortRolesBorder == 0)
    {
        BDEBUG(BD_MODEL,
"        capsule border ports    : (class has no border ports)\n");
    }
    else
    {
        BDEBUG(BD_MODEL,
"        capsule border ports    : \n");
        debugOutputModelPortsList(slot->capsuleClass->numPortRolesBorder, borderPorts);
    }
    if (slot->capsule == NULL)
    {
        BDEBUG(BD_MODEL,
"        capsule internal ports  : (none)\n");
    }
    else if ((internalPorts = slot->capsule->getInternalPorts()) == NULL)
    {
        BDEBUG(BD_MODEL,
"        capsule internal ports  : (none)\n");
    }
    else
    {
        BDEBUG(BD_MODEL,
"        capsule internal ports  :\n");
        debugOutputModelPortsList(slot->capsuleClass->numPortRolesInternal, internalPorts);
    }
    // recurse into parts.
    if (slot->capsuleClass->numSubcapsuleRoles == 0)
    {
        BDEBUG(BD_MODEL,
"        # sub-capsule parts     : (none)\n");
    }
    else
    {
        BDEBUG(BD_MODEL,
"        # sub-capsule parts     : %d\n", slot->numParts);

        for (size_t i = 0; i < slot->numParts; ++i)
        {
            BDEBUG(BD_MODEL,
"            role [%u]: %s [%d..%d] %s %s\n",
                    i,
                    slot->parts[i].role()->name,
                    slot->parts[i].role()->multiplicityLower,
                    slot->parts[i].role()->multiplicityUpper,
                    (slot->parts[i].role()->optional) ? "optional " : "",
                            (slot->parts[i].role()->plugin) ? "plugin " : "");
        }
    }
    // Recurse into sub-structure outputing slot info.
    for (size_t i = 0; i < slot->numParts; ++i)
    {
        for (size_t j = 0; j < slot->parts[i].numSlot; ++j)
        {
            debugOutputSlots(slot->parts[i].slots[j]);
        }
    }
}

void UMLRTController::debugOutputSlotContainment ( const UMLRTSlot * slot, size_t nesting )
{
    for (size_t i = 0; i < nesting; ++i)
    {
        BDEBUG(BD_MODEL, "    ");
    }
    BDEBUG(BD_MODEL, "{ %s, %s, %p, %s }\n",
        slot->name,
        (slot->capsule == NULL) ? "(none)" : slot->capsule->getName(),
         slot->capsule,
        slot->capsuleClass->name);
    for (size_t i = 0; i < slot->numParts; ++i)
    {
        for (size_t j = 0; j < slot->parts[i].numSlot; ++j)
        {
            debugOutputSlotContainment(slot->parts[i].slots[j], nesting + 1);
        }
    }
}

void UMLRTController::debugOutputModel ( const char * userMsg )
{
    // Acquire global RTS lock for this.
    UMLRTFrameService::rtsLock();

    char timebuf[UMLRTTimespec::TIMESPEC_TOSTRING_SZ];
    UMLRTTimespec tm;

    UMLRTTimespec::getClock(&tm);
    BDEBUG(BD_MODEL, "Model structure at time %s: %s\n", tm.toString(timebuf, sizeof(timebuf)), userMsg == NULL ? "" : userMsg);

    UMLRTCapsuleToControllerMap::debugOutputControllerList();

    UMLRTCapsuleToControllerMap::debugOutputCapsuleList();

    UMLRTProtocol::debugOutputServiceRegistration();

    const UMLRTCapsule * top = UMLRTCapsuleToControllerMap::getCapsuleFromName("Top");

    if (top == NULL)
    {
        BDEBUG(BD_MODEL, "ERROR: no 'Top' capsule found - no slot containment output.\n");
    }
    else
    {
        const UMLRTSlot * slot = top->getSlot();
        BDEBUG(BD_MODEL, "Slot containment: { <slot>, <capsule name>, <capsule instance address>, <capsule class> } \n");
        debugOutputSlotContainment( slot, 1 );
    }
    if (top == NULL)
    {
        BDEBUG(BD_MODEL, "ERROR: no 'Top' capsule found - no slot list output.\n");
    }
    else
    {
        const UMLRTSlot * slot = top->getSlot();
        BDEBUG(BD_MODEL, "Slot list:\n");
        debugOutputSlots( slot );
    }
    UMLRTFrameService::rtsUnlock();
}
