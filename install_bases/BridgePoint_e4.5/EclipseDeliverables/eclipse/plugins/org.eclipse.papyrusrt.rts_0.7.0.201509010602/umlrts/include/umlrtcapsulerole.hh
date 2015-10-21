// umlrtcapsulerole.hh

/*******************************************************************************
* Copyright (c) 2014-2015 Zeligsoft (2009) Limited  and others.
* All rights reserved. This program and the accompanying materials
* are made available under the terms of the Eclipse Public License v1.0
* which accompanies this distribution, and is available at
* http://www.eclipse.org/legal/epl-v10.html
*******************************************************************************/

#include <stdlib.h>

#ifndef UMLRTCAPSULEROLE_HH
#define UMLRTCAPSULEROLE_HH

struct UMLRTCapsuleClass;

// The CapsuleRole captures model structure. It names a single sub-capsule within a capsule type (Class).

struct UMLRTCapsuleRole
{
    const char * const name; // The instance name relative to containing capsule type - not relative to the application.

    const UMLRTCapsuleClass * const capsuleClass;

    const size_t multiplicityLower; // Lower-bound on multiplicity
    const size_t multiplicityUpper; // Upper-bound on multiplicity

    unsigned int optional : 1;
    unsigned int plugin : 1;
    // optional  plugin
    //    0         0  - The part is 'static'. The part's slot, instance, ports, parts-list and sub-capsules are all created when
    //                   its parent capsule is created.
    //    1         0  - The part is an 'optional' slot. The slot and ports are created when its parent capsule is
    //                   created, however the instance, the parts-list and sub-capsules are only created during an incarnate into the slot.
    //    0         1  - The part is a 'plugin' slot. The part's slot, ports, parts-list and sub-capsules are all created, but
    //                   the instance is left undefined until a import operation is performed.
    //    1         1  - undefined (not valid)
};

#endif // UMLRTCAPSULEROLE_HH
