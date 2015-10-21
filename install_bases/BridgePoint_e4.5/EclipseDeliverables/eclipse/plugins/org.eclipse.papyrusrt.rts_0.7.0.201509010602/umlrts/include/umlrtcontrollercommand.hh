// umlrtcontollercommand.hh

/*******************************************************************************
 * Copyright (c) 2015 Zeligsoft (2009) Limited  and others.
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * which accompanies this distribution, and is available at
 * http://www.eclipse.org/legal/epl-v10.html
 *******************************************************************************/

#ifndef UMLRTCONTROLLERCOMMAND_HH
#define UMLRTCONTROLLERCOMMAND_HH

#include "umlrtqueueelement.hh"
#include <stdlib.h>

class UMLRTCapsule;
struct UMLRTSlot;

struct UMLRTControllerCommand : public UMLRTQueueElement
{
    typedef enum {
        UNDEFINED, // Not a command.
        ABORT, // Abort the controller.
        DEBUG_OUTPUT_MODEL, // Have controller thread output the model structure for debugging frame service.
        DEPORT, // Deport a capsule from a slot.
        DESTROY, // Destroy a slot.
        IMPORT, // Import a capsule into a slot.
        INCARNATE, // Incarnate a capsule into a slot i.e. initialize the dynamic capsule.
    } Command;

    UMLRTControllerCommand ( ) :
            command(UNDEFINED), isTopSlot(false), serializedData(NULL), sizeSerializedData(0), slot(NULL), userMsg(NULL), allocated(false) {}

    Command command; // All commands.

    UMLRTCapsule * capsule; // IMPORT, INCARNATE
    bool isTopSlot; // DESTROY
    void * serializedData;    // INCARNATE
    size_t sizeSerializedData; // INCARNATE
    UMLRTSlot * slot; // DEPORT, DESTROY, IMPORT
    const char * userMsg; // DEBUG_OUTPUT_MODEL
    bool allocated; // For sanity checking on controller command allocation.
/*
Command parameters:

ABORT
 - command

DEBUG_OUTPUT_MODEL
 - command
 - userMsg (NULL permitted)

DEPORT
 - command
 - slot

DESTROY
 - command
 - slot
 - isTopSlot

IMPORT
 - command
 - capsule
 - slot

INCARNATE
 - command
 - capsule
 - sizeSerializedData
 - serializedData
*/

};

#endif // UMLRTCONTROLLERCOMMAND_HH
