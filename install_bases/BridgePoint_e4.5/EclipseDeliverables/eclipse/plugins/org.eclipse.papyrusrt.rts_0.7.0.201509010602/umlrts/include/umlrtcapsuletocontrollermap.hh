// umlrtcapsuletocontrollermap.hh

/*******************************************************************************
* Copyright (c) 2014-2015 Zeligsoft (2009) Limited  and others.
* All rights reserved. This program and the accompanying materials
* are made available under the terms of the Eclipse Public License v1.0
* which accompanies this distribution, and is available at
* http://www.eclipse.org/legal/epl-v10.html
*******************************************************************************/

#include "umlrthashmap.hh"
#include <stdlib.h>

#ifndef UMLRTCAPSULETOCONTROLLERMAP_HH
#define UMLRTCAPSULETOCONTROLLERMAP_HH

class UMLRTCapsule;
class UMLRTController;
struct UMLRTSlot;
struct UMLRTCapsuleClass;

class UMLRTCapsuleToControllerMap
{
public:

    // Assign a controller to a capsule (and optionally capsule class).
    static void addCapsuleToControllerSpec ( char * capsuleName, char * controllerName, char * className );

    // Add a capsule to the list.
    static void addCapsule ( const char * capsulename, UMLRTCapsule * capsule );

    // Add a controller by name.
    static bool addController ( const char * controllerName, UMLRTController * controller );

    // Enqueue abort commands to all controllers.
    static void enqueueAbortAllControllers ( );

    // Return the controller for a given name. Returns NULL if the controller is not found.
    static UMLRTController * getControllerFromName ( const char * controllerName );

    // Return the controller assigned to this capsule. Returns NULL if no controller is assigned or doesn't exist.
    static UMLRTController * getControllerForCapsule ( const char * capsuleName, const UMLRTCapsuleClass * capsuleClass );

    // Return the controller name assigned to a capsule and its class.
    static const char * getControllerNameForCapsule ( const char * capsuleName, const UMLRTCapsuleClass * capsuleClass );

    // Get the default slot list and return the number of slots in the list.
    static int getDefaultSlotList ( UMLRTSlot * * slots_p );

    // Return the first controller. Returns NULL if no controller defined.
    static UMLRTController * getFirstController ( );

    // Create a map of capsule-name to controller assignment.
    static bool readCapsuleControllerMap ( const char * controllerFile );

    // Pass in the global slot list for processing after the capsule-to-controller map is processed.
    static void setDefaultSlotList ( UMLRTSlot * slots, size_t size );

    // Get a capsule by name.
    static const UMLRTCapsule * getCapsuleFromName ( const char * capsuleName );

    // Remove a capsule instance from the list.
    static void removeCapsule ( const char * capsulename, UMLRTCapsule * capsule );

    // Remove a controller instance from the list.
    static void removeController ( const char * controllerName );

    // Debug output controller list.
    static void debugOutputControllerList ( );

    // Debug output the the capsule, controller and capsule-to-controller maps.
    static void debugOutputCapsuleList ( );

    // Debug output of capsule-to-controller map.
    static void debugOutputCaspuleToControllerMap ( );

private:
    // Parse an individual capsule-to-controller line.
    static bool parseCapsuleControllerLine ( char * line );

    static UMLRTHashMap * getControllerNameMap ( );
    static UMLRTHashMap * getCapsuleToControllerListMap ( );
    static UMLRTHashMap * getCapsuleNameMap ( );

    static UMLRTHashMap * controllerNameMap;
    static UMLRTHashMap * capsuleToControllerListMap;
    static UMLRTHashMap * capsuleNameMap;

    static int numDefaultSlotList;
    static UMLRTSlot * defaultSlotList;
};

#endif // UMLRTCAPSULEROLETOCONTROLLERMAP_HH
