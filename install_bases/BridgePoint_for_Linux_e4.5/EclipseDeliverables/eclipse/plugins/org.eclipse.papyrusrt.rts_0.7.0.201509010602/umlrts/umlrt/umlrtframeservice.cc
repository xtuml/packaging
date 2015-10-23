// umlrtframeservice.cc

/*******************************************************************************
* Copyright (c) 2015 Zeligsoft (2009) Limited  and others.
* All rights reserved. This program and the accompanying materials
* are made available under the terms of the Eclipse Public License v1.0
* which accompanies this distribution, and is available at
* http://www.eclipse.org/legal/epl-v10.html
*******************************************************************************/

#include "umlrtapi.hh"
#include "umlrtcapsule.hh"
#include "umlrtcapsulepart.hh"
#include "umlrtcapsulerole.hh"
#include "umlrtcommsport.hh"
#include "umlrtcommsportfarend.hh"
#include "umlrtframeservice.hh"
#include "umlrtprotocol.hh"
#include "umlrtslot.hh"
#include "basedebugtype.hh"
#include "basedebug.hh"
#include "basefatal.hh"
#include <sstream>

// Use a global lock on the RTS for now for modifying ports and access far-end ports during message delivery.
/*static*/ UMLRTMutex UMLRTFrameService::rtsGlobalLock;

/*static*/ UMLRTRtsInterfaceUmlrt UMLRTFrameService::rtsifUmlrt;

/*static*/ void UMLRTFrameService::bindSubcapsulePort ( bool isBorder, UMLRTCapsule * subcapsule, int portIndex, int farEndIndex )
{
    // Called from generated capsule code - port is not 'unbound'.
    if (subcapsule != NULL)
    {
        BDEBUG(BD_CONNECT, "bind subcapsule %s portIndex %d fei %d\n", subcapsule->name(), portIndex, farEndIndex);

        subcapsule->bindPort(isBorder, portIndex, farEndIndex);
    }
}

/*static*/ const UMLRTCommsPort * * UMLRTFrameService::bindPorts ( UMLRTSlot * slot, UMLRTCapsule * capsule, const UMLRTCapsuleClass * requestedClass,
        const UMLRTCapsuleClass * slotClass, const UMLRTCommsPort * * borderPorts, bool bind, bool import )
{
    // Assumes global RTS lock acquired.

    // For incarnate, the newBorderPorts will become the top capsule's border ports (caspule instance created in this context but initialized by controller.)
    // For import, the returned border port list is discarded. It's run twice - once for checking in the context of the importer (no binding occurs)
    // and once in the context of the slot's controller (where input border port list is actually updated.)

    const UMLRTCommsPort * * newBorderPorts = new const UMLRTCommsPort * [requestedClass->numPortRolesBorder];

    // The slot will get the importPortMap to assist with deport.
    int * slotToBorderMap = new int[slotClass->numPortRolesBorder]; // Will hold index into newBorderPorts for each slot port.

    // Remember the number of bound instances on each slot port for determining whether we need a proxy port or need to make connections.
    size_t * slotBoundCount = new size_t[slotClass->numPortRolesBorder]; // Will hold the number of bound instances on each slot port.

    // Each of the bound slot ports have to be bound to a capsule border port.
    size_t slotPortBoundCount = 0; // Number of ports on the slot that have a bind - need at least this many capsule border ports.
    for (size_t slot_i = 0; slot_i < slotClass->numPortRolesBorder; ++slot_i)
    {
        // Set all slotToBorderMap entries to 'capsule border port not mapped to this slot'.
        slotToBorderMap[slot_i] = -1;

        // Remember how many port instances were bound on the slot port.
        if ((slotBoundCount[slot_i] = bindPortsCountBound( &slot->ports[slot_i])) > 0)
        {
            // Must have at least one capsule border port to accommodate every bound slot port. Checked below.
            ++slotPortBoundCount;
        }
    }
    // Initialize 'newBorderPorts' - the new border ports for the capsule if binding succeeds.
    for (size_t i = 0; i < requestedClass->numPortRolesBorder; ++i)
    {
        if ((borderPorts != NULL) && borderPorts[i] == NULL)
        {
            FATAL("slot %s capsule %s border port was NULL - should at least be the unbound port.",
                    slot->name, capsule->name());
        }
        if ((borderPorts != NULL) && !borderPorts[i]->unbound)
        {
            // Existing capsule border port is bound either to a slot port or is a proxy port bound to one or more slots.
            newBorderPorts[i] = borderPorts[i];
        }
        else
        {
            // Capsule border port is unbound (or we are incarnating and didn't have a set of pre-existing caspule border ports.)
            newBorderPorts[i] = NULL;
        }
    }
    // Port compatibility check.
    // The ports are assumed to be ordered.

    size_t border_i = 0; // Port index on requesting class. We're searching the capsule's border ports for unbound ports to match bound slot ports.

    // Go through each port on the slot. Quit if we fail to find a match for any bound slot port. Do the actual binding after we've proven the capsule fits.
    bool compatible = true; // Assume compatibility until we fail.

    // Before we start checking to see whether the capsule's border ports fit into the slot, we perform the trivial check
    // of ensuring the capsule has at least as many border ports available as their are slot ports with bindings.
    // The for-loop before is not entered if the requested class has no border ports, and we have to check this case.
    // The trivial case of incarnating or importing into a slot with no ports will not enter the for-loop and allow compatibility.
    if (requestedClass->numPortRolesBorder < slotPortBoundCount)
    {
        // Capsule doesn't have enough border ports to satisfy the slot.
        compatible = false;
    }
    for (size_t slot_i = 0; (slot_i < slotClass->numPortRolesBorder) && compatible; ++slot_i)
    {
        bool match = false;

        if (slotBoundCount[slot_i] == 0)
        {
            // The slot port has no bound instances so we don't require the capsule to have an associated border port.
            // The slot port is deemed to have a matching port on the capsule.
            // We need not advance the capsule border port index 'border_i' explicitly here since name-matching will
            // automatically advance 'border_i' to the correct capsule border port if a subsequent slot port is found
            // to have a binding and needs to be matched.
            match = true;
        }
        else if (border_i >= requestedClass->numPortRolesBorder)
        {
            // The slot port has at least one bound instance but we've already checked all available capsule
            // border ports - so this slot port can't be matched to a capsule border port.
            compatible = false;
        }
        // Loop through the remaining requesting capsule border ports looking for a match to slot port slot_i.
        while (compatible && !match)
        {
            // At this point, border_i is guaranteed to point to an existing capsule border port.

            // If port-name match.
            if (!strcmp(slotClass->portRolesBorder[slot_i].name, requestedClass->portRolesBorder[border_i].name))
            {
                // Get the number of bound instances on the capsules's border port.
                size_t borderBoundCount = bindPortsCountBound(newBorderPorts[border_i]);

                if (strcmp(slotClass->portRolesBorder[slot_i].protocol, requestedClass->portRolesBorder[border_i].protocol))
                {
                    // protocol mismatch.
                    compatible = false;
                    BDEBUG(BD_BINDFAIL, "BIND FAIL:%d %d:slot %s (class %s) port %s (protocol %s) has MISMATCHED PROTOCOL with capsule (class %s) (protocol %s).\n",
                            slot_i, border_i, slot->name, slot->capsuleClass, slotClass->portRolesBorder[slot_i].name, slotClass->portRolesBorder[slot_i].protocol, requestedClass->name, requestedClass->portRolesBorder[border_i].protocol);
                }
                else if (slotClass->portRolesBorder[slot_i].conjugated != requestedClass->portRolesBorder[border_i].conjugated)
                {
                    // conjugation mismatch.
                    compatible = false;
                    BDEBUG(BD_BINDFAIL, "BIND FAIL:%d %d:slot %s port %s (protocol %s) has MISMATCHED CONJUGATION with capsule (class %s)\n",
                            slot_i, border_i, slot->name, slotClass->portRolesBorder[slot_i].name, slotClass->portRolesBorder[slot_i].protocol, requestedClass->name);
                }
                else if ((requestedClass->portRolesBorder[border_i].numFarEnd - borderBoundCount) < slotBoundCount[slot_i])
                {
                    // capsule border port unbound instances insufficient for binding
                    compatible = false;
                    BDEBUG(BD_BINDFAIL, "BIND FAIL:%d %d:slot %s port %s (protocol %s) BOUND PORTS [%lu] EXCEEDS CAPSULE'S UNBOUND (class %s) port repl unbound instances [%lu].\n",
                            slot_i, border_i, slot->name, slotClass->portRolesBorder[slot_i].name, slotClass->portRolesBorder[slot_i].protocol, slotBoundCount[slot_i], requestedClass->name,
                            requestedClass->portRolesBorder[border_i].numFarEnd - borderBoundCount);
                }
                else
                {
                    // The slot port has to be bound to this port because it has bindings.

                    BDEBUG(BD_BINDDEBUG, "BIND DEBUG:%d %d:slot %s port %s (protocol %s) repl %lu MATCHED to capsule border port (bindings reqd).\n",
                            slot_i, border_i, slot->name, slotClass->portRolesBorder[slot_i].name, slotClass->portRolesBorder[slot_i].protocol, slotClass->portRolesBorder[slot_i].numFarEnd);

                    // We can accommodate this port bind. Remember the details, but don't bind just yet.
                    match = true;
                    slotToBorderMap[slot_i] = border_i; // Remember which capsule border port index was mapped to slot slot_i.
                    if (newBorderPorts[border_i] == NULL)
                    {
                        // The capsule border port was not already mapped elsewhere, so just map this slot's port to this capsule border port for now.
                        newBorderPorts[border_i] = &slot->ports[slot_i];
                    }
                }
            }
            // At this point, the border port on the capsule is no longer useful.
            // The slot port 'slot_i' needs a match.  We have already matched it or we need to keep looking.
            // If the border port name matched the slot port, match is true or compatible is false.
            // If the border port name didn't match the slot port, we are not concerned with this capsule border port and
            // can skip to the next capsule border port.
            // Advance to the next border port.
            if ((++border_i >= requestedClass->numPortRolesBorder) && !match)
            {
                // If we exhausted the capsule border ports and still haven't found a match for the bound slot port 'slot_i', then
                // we know we're incompatible.
                compatible = false;
            }
        }
        if (!match)
        {
            // Slot port slot_i didn't find a home in the capsule border port list, so fail port binding.
            compatible = false;
            BDEBUG(BD_BINDFAIL, "BIND FAIL:slot %s port %s (protocol %s) repl %lu FAILED TO BIND.\n",
                    slot->name, slotClass->portRolesBorder[slot_i].name, slotClass->portRolesBorder[slot_i].protocol, slotClass->portRolesBorder[slot_i].numFarEnd);
        }
    }
    if (compatible && bind)
    {
        // Succeeded with the bind-check. Redefine the slot's map from slot-port-to-capsule-border-port.
        if (slot->slotToBorderMap != NULL)
        {
            FATAL("Slot %s already had a slotToBorderMap during port binding.", slot->name);
        }
        if (bind)
        {
            // For both incarnate and import, we're binding and define the slotToBorderMap here for deporting or destroy.
            slot->slotToBorderMap = slotToBorderMap;
        }
        // For import, we do port binding. For incarnate, we may create proxy ports.

        // First, replace any matched slot ports in the new capsule border port list with proxies where they are required.
        // We create proxy ports when the capsule border port repl > slot port repl, or the slot port has unbound instances.
        for (size_t slot_i = 0; (slot_i < slotClass->numPortRolesBorder); ++slot_i)
        {
            // If this slot port was bound to a capsule border port in the check above AND the capsule didn't have a previous binding on the port.
            if ((slotToBorderMap[slot_i] != -1) && (newBorderPorts[slotToBorderMap[slot_i]] == &slot->ports[slot_i]))
            {
                // If we get to here, the capsule border port that matched the slot port was not previously bound.
                // We've assigned the slot port as a 'possible' capsule border port.

                // Here we check whether the slot port can stay as the border port. If not, we create a proxy port for the capsule border port.

                // If the slot port replication count is less than the capsule border port can support
                // or the slot had unbound instances...
                if (   (slot->ports[slot_i].numFarEnd < requestedClass->portRolesBorder[slotToBorderMap[slot_i]].numFarEnd)
                    || (slot->ports[slot_i].numFarEnd > slotBoundCount[slot_i]))
                {
                    // ...create a proxy port with a new far-end list to accommodate the capsule border port's larger replication count.
                    // Duplicate the bound connections that were already on the slot port. The slot port's far-end still points the slot port, which is good.

                    // If we're importing, we notify the capsule instance of the bound instances while we copy them to the proxy port.
                    // For incarnation, the capsule class instantiate function will handle the port binding.
                    newBorderPorts[slotToBorderMap[slot_i]] = bindPortsCreateProxyPort(slot, capsule, borderPorts, &slot->ports[slot_i], requestedClass, slotToBorderMap[slot_i], bind, import);
                    BDEBUG(BD_BINDDEBUG, "BIND DEBUG:%d %d:slot %s port %s (protocol %s) repl %lu CREATE PROXY PORT repl %lu\n",
                            slot_i, slotToBorderMap[slot_i], slot->name, slotClass->portRolesBorder[slot_i].name, slotClass->portRolesBorder[slot_i].protocol, slotClass->portRolesBorder[slot_i].numFarEnd,
                            requestedClass->portRolesBorder[slotToBorderMap[slot_i]].numFarEnd);
                }
                else
                {
                    // The slot port is fully bound and the capsule border port was unbound.
                    // The slot port becomes the capsule border port because all slot ports are bound.
                    newBorderPorts[slotToBorderMap[slot_i]] = &slot->ports[slot_i];
                    if (import && bind)
                    {
                        // If this is import, we update the capsule border port right here.
                        const UMLRTCommsPort * existingBorderPort = capsule->getBorderPorts()[slotToBorderMap[slot_i]];
                        if (!existingBorderPort->unbound)
                        {
                            FATAL("import binding of capsule %s into slot %s assigning slot port %s was not 'unbound'",
                                    capsule->name(), slot->name, existingBorderPort->role()->name );
                        }
                        capsule->getBorderPorts()[slotToBorderMap[slot_i]] = newBorderPorts[slotToBorderMap[slot_i]];
                    }
                    BDEBUG(BD_BINDDEBUG, "BIND DEBUG:%d %d:slot %s port %s (protocol %s) repl %lu MATCH BECOMES capsule border port\n",
                            slot_i, slotToBorderMap[slot_i], slot->name, slotClass->portRolesBorder[slot_i].name, slotClass->portRolesBorder[slot_i].protocol, slotClass->portRolesBorder[slot_i].numFarEnd,
                            requestedClass->portRolesBorder[slotToBorderMap[slot_i]].numFarEnd);

                    // If this is import, we notify the capsule instance here.
                    if (import && bind)
                    {
                        for (size_t i = 0; i < requestedClass->portRolesBorder[slotToBorderMap[slot_i]].numFarEnd; ++i)
                        {
                            capsule->bindPort(true/*isBorder*/, slotToBorderMap[slot_i], i);
                        }
                        // Inform the capsule of the entire port binding.
                        BDEBUG(BD_BIND, "capsule %s (class %s) bind entire %s port %d during import\n", slot->name, slot->capsuleClass->name,
                                (borderPorts[slotToBorderMap[slot_i]] == &slot->ports[slot_i]) ? "slot" : "proxy", slotToBorderMap[slot_i]);
                    }
                }
            }
            else if (slotToBorderMap[slot_i] != -1)
            {
                // Will never be true for incarnate here, because incarnate didn't have capsule border ports to begin with.
                // This border port (that is NOT this slot's border port) can accommodate this slot port because it has enough free instances.
                // Previously unbound port instances of the capsule border port are used to accommodate this slot port's far-ends.
                // Bind the bound slot instances to the far-ends of unbound capsule border port instances.
                bindPortsAddFarEnds(borderPorts[slotToBorderMap[slot_i]], slotToBorderMap[slot_i], &slot->ports[slot_i], capsule, import);
                BDEBUG(BD_BINDDEBUG, "BIND DEBUG:%d %d:slot %s port %s (protocol %s) repl %lu bindings added to previously existing border port\n",
                        slot_i, slotToBorderMap[slot_i], slot->name, slotClass->portRolesBorder[slot_i].name, slotClass->portRolesBorder[slot_i].protocol, slotClass->portRolesBorder[slot_i].numFarEnd);
            }
            BDEBUG(BD_BINDDEBUG, "BIND OK:%d: slot %s port %s (protocol %s) (repl %d) %s capsule (class %s) border port%s.\n",
                    slot_i,
                    slot->name,
                    slotClass->portRolesBorder[slot_i].name,
                    slotClass->portRolesBorder[slot_i].protocol,
                    slotClass->portRolesBorder[slot_i].numFarEnd,
                    (slotToBorderMap[slot_i] != -1) ?
                            ((newBorderPorts[slotToBorderMap[slot_i]] == &slot->ports[slot_i]) ? "BECOMES the" : "PROXY USED for the") : "NO BINDS REQUIRED and the",
                    requestedClass->name,
                    (slotToBorderMap[slot_i] != -1) ? "" : " was NOT BOUND");
        }
        // We've done all slot port binding to capsule border port. Import binding has called the capsule instance to notify of the bindings.
        // At this point, any capsule border port that remains unbound by the algorithm above is still 'completely unbound' and we can
        // create the 'unbound port' for this one. This applies to incarnated capsule border ports only.
        for (size_t i = 0; (i < requestedClass->numPortRolesBorder) && !import; ++i)
        {
            if (newBorderPorts[i] == NULL)
            {
                // The newBorderPort remained NULL during binding. Applies to incarnate only.
                // For import, the border port should already be the unbound proxy port.
                newBorderPorts[i] = createPort(slot, requestedClass, i, true/*border*/, true/*proxy*/, true/*isUnbound*/);
            }
        }
    }
    if (!compatible)
    {
        // Binding failed - delete resources. (If binding succeeded, the slot has the 'slotToBorderMap' and the newBorderPorts
        // is returned to the caller (to be passed to the initial capsule instantiation during incarnation and discarded (but indicates
        // success) for import in the context of the importer. Binding is re-done for import in the context of the capsule's controller.
        delete[] slotToBorderMap;
        slotToBorderMap = NULL;
        delete[] newBorderPorts;
        newBorderPorts = NULL;
    }
    delete[] slotBoundCount;

    return newBorderPorts;
}

/*static*/ void UMLRTFrameService::bindPortsAddFarEnds ( const UMLRTCommsPort * capsuleBorderPort, int border_i, const UMLRTCommsPort * slotport, UMLRTCapsule * capsule, bool import )
{
    // Assumes global RTS lock already acquired.

    // We're adding the slot port bound far-ends to free far-end instances on a capsule proxy port.
    // For every slot port instance, find a free home on the capsule border port.
    for (size_t i = 0; i < slotport->numFarEnd; ++i)
    {
        size_t border_fei = 0; // Start at the first 'far-end instance' (fei) of the capsule border port.

        if (slotport->farEnds[i].port != NULL)
        {
            // slot port far end is connected, search forward through the capsule border port looking for an unbound instance.
            bool bound = false;
            while ((border_fei < capsuleBorderPort->numFarEnd) && !bound)
            {
                if (capsuleBorderPort->farEnds[border_fei].port == NULL)
                {
                    // Connect the slot port's far-end to the (formerly) unbound far end on the capsule's border port.
                    // NOTE: The slot port far-end is not modified.
                    capsuleBorderPort->farEnds[border_fei].port = slotport->farEnds[i].port;
                    capsuleBorderPort->farEnds[border_fei].farEndIndex = slotport->farEnds[i].farEndIndex;
                    bound = true;

                    BDEBUG(BD_BIND, "capsule %s (class %s) bind proxy port instance %d %d during import\n",
                            capsuleBorderPort->slot->name, capsuleBorderPort->slot->capsuleClass->name, border_i, border_fei);

                    if (import)
                    {
                        // For import, we notify the capsule instance of the bind.
                        capsule->bindPort(true/*isBorder*/, border_i, border_fei);
                    }
                }
                border_fei++;
            }
            if (border_fei >= capsuleBorderPort->numFarEnd && !bound)
            {
                // Should never happen. We'd already verified that the capsuleBorderPort can accommodate the slot bindings.
                FATAL("failed to connect all slot port's far-ends to the capsule border port during import.");
            }
        }
    }
}

/*static*/ size_t UMLRTFrameService::bindPortsCountBound ( const UMLRTCommsPort * port )
{
    // Assumes global RTS lock already acquired.

    size_t count = 0; // The 'unbound port' has no far-ends.

    if (port != NULL)
    {
        for (size_t i = 0; i < port->numFarEnd; ++i)
        {
            if (port->farEnds[i].port != NULL)
            {
                ++count;
            }
        }
    }
    return count;
}

/*static*/ const UMLRTCommsPort * UMLRTFrameService::bindPortsCreateProxyPort ( UMLRTSlot * slot, UMLRTCapsule * capsule, const UMLRTCommsPort * * borderPorts, const UMLRTCommsPort * slotPort, const UMLRTCapsuleClass * requestedClass, int border_i, bool bind, bool import )
{
    // Assumes global RTS lock already acquired.

    // Create the border port unconnected.
    const UMLRTCommsPort * newBorderPort = createPort( slot, requestedClass, border_i, true/*border*/, true/*proxy*/, false/*isUnbound*/);

    if (import && bind)
    {
        if (!borderPorts[border_i]->unbound)
        {
            FATAL("import binding of capsule %s into slot %s creating proxy port %s was not 'unbound'",
                    capsule->name(), slot->name, borderPorts[border_i]->role()->name );
        }
        // If this is import, we update the actual border port of the capsule right here - and the far ends get defined below.
        // The existing border port was the unbound port and we deallocate it here.
        destroyPort(borderPorts[border_i], true/*deletePort*/);
        borderPorts[border_i] = newBorderPort;
    }
    // Copy connections from the slot port. The border port is larger - the 'extra' port instances remain unconnected.
    for (size_t i = 0; i < slotPort->numFarEnd; ++i)
    {
        newBorderPort->farEnds[i].farEndIndex = slotPort->farEnds[i].farEndIndex;
        if (((newBorderPort->farEnds[i].port = slotPort->farEnds[i].port) != NULL) && import && bind)
        {
            // We've just bound a far-end of a slot port to a new capsule border proxy port - notify the capsule instance when we're importing.
            capsule->bindPort(true/*isBorder*/, border_i, i);
        }
    }
    return newBorderPort;
}

/*static*/ bool UMLRTFrameService::bindServicePort ( const UMLRTCommsPort * sapPort, const UMLRTCommsPort * sppPort )
{
    bool ok = false;

    BDEBUG(BD_SAP, "SAP [%s %s] bind to SPP [%s %s]\n", (sapPort == NULL) ? "NULL" : sapPort->slot->name, (sapPort == NULL) ? "NULL" : sapPort->role()->name,
                    (sppPort == NULL) ? "NULL" : sppPort->slot->name, (sppPort == NULL) ? "NULL" : sppPort->role()->name);

    if (sapPort && sppPort)
    {
        if (sapPort->numFarEnd != 1)
        {
            FATAL("SAP slot %s port %s has > 1 far-ends (has %d)", sapPort->slot->name, sapPort->role()->name, sapPort->numFarEnd);
        }
        else if (sapPort->farEnds[0].port != NULL)
        {
            FATAL("SAP slot %s port %s already connected to slot %s port %s", sapPort->slot->name, sapPort->role()->name,
                    sapPort->farEnds[0].port->slot->name, sapPort->farEnds[0].port->role()->name);
        }
        else if (strcmp(sapPort->role()->protocol, sppPort->role()->protocol))
        {
            // TODO - should inform user - Bug 246
            BDEBUG(BD_SWERR, "binding SAP[%s %s] to SPP[%s %s] mismatched protocol sap(%s) spp(%s)\n",  sapPort->slot->name, sapPort->role()->name,
                    sppPort->slot->name, sppPort->role()->name, sapPort->role()->protocol, sppPort->role()->protocol);
        }
        else if (sapPort->role()->conjugated == sppPort->role()->conjugated)
        {
            // TODO - should inform user - Bug 246
            BDEBUG(BD_SWERR, "binding SAP[%s %s] to SPP[%s %s] conjugation not complementary\n",  sapPort->slot->name, sapPort->role()->name,
                    sppPort->slot->name, sppPort->role()->name);
        }
        else
        {
            for (size_t i = 0; (i < sppPort->numFarEnd) && !ok; ++i)
            {
                if (sppPort->farEnds[i].port == NULL)
                {
                    connectPorts(sapPort, 0, sppPort, i);
                    sendBoundUnbound(sapPort, 0, sppPort, i, true/*isBind*/);
                    sendBoundUnbound(sppPort, i, sapPort, 0, true/*isBind*/);
                    ok = true;
                }
            }
        }
    }
    return ok;
}

/*static*/ void UMLRTFrameService::condemnParts ( UMLRTSlot * slot )
{
    slot->condemned = true;
    for (size_t i = 0; i < slot->numParts; ++i)
    {
        for (size_t j = 0; j < slot->parts[i].numSlot; ++j)
        {
            // set 'condemned' on sub-structure.
            condemnParts(slot->parts[i].slots[j]);
        }
    }
}

/*static*/ void UMLRTFrameService::connectPorts ( const UMLRTCommsPort * p1, size_t index1, const UMLRTCommsPort * p2, size_t index2 )
{
    // Assumes global RTS lock already acquired.

    BDEBUG(BD_CONNECT, "connectPorts: ");
    if (p1 == NULL)
    {
        BDEBUG(BD_CONNECT, "P1 NULL");
    }
    else if (p1->unbound)
    {
        BDEBUG(BD_CONNECT, "P1 UNBOUND");
    }
    else
    {
        BDEBUG(BD_CONNECT, "P1 slot(%s) port(%s[%lu]) id(%d) (%s) repl sz(%d)\n",
                p1->slot->name,
                p1->role()->name,
                index1,
                p1->role()->id,
                p1->role()->protocol,
                p1->role()->numFarEnd);

        // if p1 has a far end, connect p1's far-end to p2.
        if (p1->farEnds[index1].port)
        {
            size_t original_index1 = index1;
            index1 = p1->farEnds[index1].farEndIndex;
            p1 = p1->farEnds[original_index1].port;
            BDEBUG(BD_CONNECT, "              *** connecting P1->farEnd: slot(%s) port(%s[%lu]) id(%d) (%s) repl sz(%d)\n",
                    p1->slot->name,
                    p1->role()->name,
                    index1,
                    p1->role()->id,
                    p1->role()->protocol,
                    p1->role()->numFarEnd);
        }
        if (p2 == NULL)
        {
            BDEBUG(BD_CONNECT, "              P2 NULL\n");
        }
        else if (p2->unbound)
        {
            BDEBUG(BD_CONNECT, "              P2 UNBOUND\n");
        }
        else
        {
            BDEBUG(BD_CONNECT, "              P2 slot(%s) port(%s[%lu]) id(%d) (%s) repl sz(%d)\n",
                    p2->slot->name,
                    p2->role()->name,
                    index2,
                    p2->role()->id,
                    p2->role()->protocol,
                    p2->role()->numFarEnd);
        }
        p1->farEnds[index1].port = p2->unbound ? NULL : p2;
        p1->farEnds[index1].farEndIndex = p2->unbound ? 0 : index2;
        if ((p2 != NULL) && (!p2->unbound))
        {
            if (index2 > p2->numFarEnd)
            {
                FATAL("slot %s port %s p1 %s[%d] p2 %s[%d] p2 index exceeds p2->numFarEnd(%d)",
                        p1->slot->name, p1->role()->name, index1, p2->role()->name, index2, p2->numFarEnd);
            }
            p2->farEnds[index2].port = p1;
            p2->farEnds[index2].farEndIndex = index1;
        }
    }
}

/*static*/ void UMLRTFrameService::connectRelayPort ( const UMLRTCommsPort * relayPort, size_t relayIndex, const UMLRTCommsPort * destPort, size_t destIndex )
{
    // Assumes global RTS lock already acquired.

    BDEBUG(BD_CONNECT, "connectRelayPort: ");
    if (relayPort == NULL)
    {
        BDEBUG(BD_CONNECT, "P1 NULL");
    }
    else if (relayPort->unbound)
    {
        BDEBUG(BD_CONNECT, "P1 UNBOUND");
    }
    else
    {
        relayPort->relay = true;

        BDEBUG(BD_CONNECT, "P1 slot(%s) port(%s[%lu]) id(%d) (%s) repl sz(%d)\n",
                relayPort->slot->name,
                relayPort->role()->name,
                relayIndex,
                relayPort->role()->id,
                relayPort->role()->protocol,
                relayPort->role()->numFarEnd);

        size_t original_index1 = relayIndex;
        relayIndex = relayPort->farEnds[relayIndex].farEndIndex;
        relayPort = relayPort->farEnds[original_index1].port;
        if (relayPort == NULL)
        {
            BDEBUG(BD_CONNECT, "              *** connecting P1->farEnd: NULL - will leave p2 'as is'\n");
        }
        else
        {
            BDEBUG(BD_CONNECT, "              *** connecting P1->farEnd: slot(%s) port(%s[%lu]) id(%d) (%s) repl sz(%d)\n",
                relayPort->slot->name,
                relayPort->role()->name,
                relayIndex,
                relayPort->role()->id,
                relayPort->role()->protocol,
                relayPort->role()->numFarEnd);
        }
        if ((relayPort != NULL) && (destPort != NULL))
        {
            BDEBUG(BD_CONNECT, "              P2 slot(%s) port(%s[%lu]) id(%d) (%s) repl sz(%d)\n",
                    destPort->slot->name,
                    destPort->role()->name,
                    destIndex,
                    destPort->role()->id,
                    destPort->role()->protocol,
                    destPort->role()->numFarEnd);
            destPort->farEnds[destIndex].port = relayPort;
            destPort->farEnds[destIndex].farEndIndex = relayIndex;
            relayPort->farEnds[relayIndex].port = destPort;
            relayPort->farEnds[relayIndex].farEndIndex = destIndex;
        }
    }
}

/*static*/ void UMLRTFrameService::controllerDeport ( UMLRTSlot * slot, bool synchronous, bool lockAcquired )
{
    // Get lock if we don't have it.
    if (!lockAcquired)
    {
        rtsLock();
    }
    if (slot == NULL)
    {
        FATAL("Attempt deport from NULL slot.");
    }
    else if ((slot->capsule == NULL) && !synchronous)
    {
        // Must have received the deport request 'late'. Just ignore it.
        BDEBUG(BD_IMPORT, "Received 'late' deport request for slot %s. Ignoring it.\n", slot->name);
    }
    else if (slot->slotToBorderMap == NULL)
    {
        FATAL("Attempt to deport from slot %s but slotToBorderMap was NULL.", slot->name);
    }
    else  if (!slot->role()->plugin)
    {
        FATAL("Attempt to deport from slot %s but slot was not a plugin slot.", slot->name);
    }
    else
    {
        const UMLRTCommsPort * * borderPorts = slot->capsule->getBorderPorts();

        controllerDeportUnbind(slot, borderPorts);

        delete[] slot->slotToBorderMap;
        slot->slotToBorderMap = NULL;

        slot->capsuleClass = slot->role()->capsuleClass; // restore the original capsuleClass
        slot->capsule = NULL; // Removes record of this instance, but the instance lives on in its original optional slot.
    }
    // Get lock if we don't have it.
    if (!lockAcquired)
    {
        rtsUnlock();
    }
}

void UMLRTFrameService::controllerDeportUnbind ( UMLRTSlot * slot, const UMLRTCommsPort * * borderPorts )
{
    // Assumes global RTS lock already acquired.

    // Examine the capsule's class slotToBorderMap to see if any of the capsules border ports were mapped to this slot.
    for (size_t slot_i = 0; slot_i < slot->capsule->getSlot()->role()->capsuleClass->numPortRolesBorder; ++slot_i)
    {
        int border_i = slot->slotToBorderMap[slot_i];

        if (border_i != -1)
        {
            // This capsule border port slot had bindings on this slot port - notify the capsule about the unbinds from this slot.
            if (borderPorts[border_i] == &slot->ports[slot_i])
            {
                // Capsule has a border port mapped directly to the slot port.
                BDEBUG(BD_BIND, "capsule %s (class %s) unbind entire slot port %d during deport\n", slot->name, slot->capsuleClass->name, border_i);

                for (size_t j = 0; j < slot->ports[slot_i].numFarEnd; ++j)
                {
                    if (slot->ports[slot_i].farEnds[j].port != NULL)
                    {
                        // Notify the capsule.
                        slot->capsule->unbindPort(true/*isBorder*/, slot_i, j);

                        // Connect the slot port far-ends back to the slot.
                        connectPorts(&slot->ports[slot_i], j, &slot->ports[slot_i], j);
                    }
                }
            }
            else
            {
                // This is a proxy port with individual instances that would have been bound during import binding.
                if (!borderPorts[border_i]->proxy)
                {
                    FATAL("Unbinding capsule %s from slot %s and mapped slot port[%d] != border port[%d] and port was not a proxy?",
                            slot->capsule->getSlot()->name, slot->name, slot_i, border_i );
                }
                // slotToBorderMap[border_i] should be -1 for unbound ports.
                if (borderPorts[border_i]->unbound)
                {
                    FATAL("Unbinding capsule %s from slot %s and mapped slot port[%d] != border port[%d] and port was unbound ?",
                            slot->capsule->getSlot()->name, slot->name, slot_i, border_i );
                }
                for (size_t j = 0; j < slot->ports[slot_i].numFarEnd; ++j)
                {
                    if (slot->ports[slot_i].farEnds[j].port != NULL)
                    {
                        int instance_i;

                        if ((instance_i = findFarEndInstance(slot->ports[slot_i].farEnds[j].port, slot->ports[slot_i].farEnds[j].farEndIndex, borderPorts[border_i])) < 0)
                        {
                            FATAL("slot %s deport of capsule %s port %s bound to far end %s not found on border proxy port id(%d)",
                                    slot->name, slot->capsule->name(), slot->ports[slot_i].role()->name, slot->ports[slot_i].farEnds[j].port->slot->name, border_i);
                        }
                        else
                        {
                            // Callback into the capsule instance for unbinding the border port.
                            BDEBUG(BD_BIND, "capsule %s (class %s) unbind proxy port instance %d %d during deport\n", slot->name, slot->capsuleClass->name, border_i, instance_i);

                            // Notify capsule.
                            slot->capsule->unbindPort(true/*isBorder*/, border_i, instance_i);

                            // Reconnect the slot port instance to itself - it's far-end gets connected to the slot.
                            connectPorts( &slot->ports[slot_i], j, &slot->ports[slot_i], j);
                        }
                    }
                }
            }
        }
    }
}

/*static*/ void UMLRTFrameService::controllerDestroy ( UMLRTSlot * slot, bool isTopSlot, bool synchronous, bool lockAcquired )
{
    // Get RTS lock if we don't have it.
    if (!lockAcquired)
    {
        rtsLock();
    }
    BDEBUG(BD_DESTROY, "Controller %s destroying slot %s. Capsule instance %s. The slot itself %s.\n",
            slot->controller->getName(),
            slot->name,
            (slot->capsule != NULL) ? "will be deported or deleted" : "not present",
            isTopSlot ? "will persist" : "will be destroyed");

    if (slot->role() == NULL)
    {
        FATAL("Attempting to destroy a slot with no role. Destroying Top?");
    }
    if (slot->generated && !isTopSlot)
    {
        FATAL("Attempt to destroy a generated slot name(%d) class(%s) role(%s)",
                slot->name, slot->capsuleClass->name, slot->role()->name);
    }
    if (isTopSlot)
    {
        // This is the top slot in a destruction. Special processing is required.
        // The destruction of sub-slots of the top slot do not branch into this code.

        // Mark all sub-slots as 'condemned' to cease signal activity on these.
        condemnParts(slot);

        if (slot->capsule != NULL)
        {
            // Send rtUnbound for top capsule ports.
            sendBoundUnboundForCapsule(slot->capsule, false/*isBind*/);

            // Reconnect the slot port far-ends with the slot ports.
            for (size_t i = 0; i < slot->capsule->getClass()->numPortRolesBorder; ++i)
            {
                for (size_t j = 0; j < slot->ports[i].numFarEnd; ++j)
                {
                    // Purge any deferred messages owned by capsule being destroyed.
                    slot->ports[i].purge();
                    slot->ports[i].relay = false; // Empty slots have no relay ports regardless of slot capsule class.
                    connectPorts( &slot->ports[i], j, &slot->ports[i], j );
                }
            }
        }
        // Recurse into sub-structure, either deporting or sending DEPORT Controller Commands.
        deportParts(slot);

        // Recurse into sub-structure, either destroying or sending DESTROY Controller Commands.
        for (size_t i = 0; i < slot->numParts; ++i)
        {
            for (size_t j = 0; j < slot->parts[i].numSlot; ++j)
            {
                requestControllerDestroy(slot->parts[i].slots[j], false/*isTopSlot*/, true/*lockAcquired*/);
            }
        }
    }
    if (slot->capsule != NULL)
    {
        // If the slot had a running instance, delete it.

        // Get a copy of the ports to destroy them here.
        const UMLRTCommsPort * * borderPorts = slot->capsule->getBorderPorts();
        const UMLRTCommsPort * * internalPorts = slot->capsule->getInternalPorts();
        const UMLRTCapsuleClass * capsuleClass = slot->capsule->getClass();

        delete slot->capsule;
        destroyPortList(borderPorts, capsuleClass->numPortRolesBorder, true/*proxiesOnly*/);
        destroyPortList(internalPorts, capsuleClass->numPortRolesInternal, false/*proxiesOnly*/);

        slot->capsule = NULL;
        slot->capsuleClass = slot->role()->capsuleClass; // restore the original capsuleClass

        // Destroy parts data-structures. The destroy requests have already been enqueued for sub-slots, but need to clean up this slot's part structures.
        destroyPartsBookkeeping(slot->numParts, slot->parts);
        slot->parts = NULL;
        slot->numParts = 0;
    }
    else if ((slot->numParts != 0) || (slot->parts != NULL))
    {
        // Sanity check - slot didn't have a running instance, so there should be no bookkeeping.
        FATAL("slot (%s) had no running instance, but had numParts(%d) != 0 or parts != NULL", slot->name, slot->numParts);
    }
    // If the slot ports were bound at run-time, there will be a map.
    if (slot->slotToBorderMap != NULL)
    {
        delete[] slot->slotToBorderMap;
        slot->slotToBorderMap = NULL;
    }
    // Delete messages bound for the slot and timers bound for the slot.
    slot->controller->deallocateSlotResources( slot );

    if (!isTopSlot)
    {
        // Is a sub-slot in a condemned sub-capsule structure - delete the slot itself.
        destroyPortArray(slot->ports, slot->numPorts);
        free((void*)slot->name);
        delete slot;
    }
    else
    {
        slot->condemned = false; // Top capsule slot no longer 'condemned' - it has no instance, but is not condemned.
    }
    // Unlock RTS if we locked it here.
    if (!lockAcquired)
    {
        rtsUnlock();
    }
}

// Import a capsule - run from the controller who owns the capsule instance being imported.
/*static*/ bool UMLRTFrameService::controllerImport ( UMLRTSlot * slot, UMLRTCapsule * capsule, bool synchronous, bool lockAcquired )
{
    bool ok = false;

    if (slot == NULL)
    {
        FATAL("attempt to import into NULL slot");
    }
    if (capsule == NULL)
    {
        FATAL("attempt to import NULL capsule into slot %s", slot->name);
    }
    BDEBUG(BD_IMPORT, "controller import capsule %s into slot %s.\n", capsule->name(), slot->name);

    if (slot->capsule)
    {
        if (synchronous)
        {
            // We're running this code from the initial import request thread - need to set the requestor's controller error code.
            slot->controller->setError(UMLRTController::E_IMPORT_OCCUPIED);
        }
        else
        {
            // May occur if this was an IMPORT Controller Command and some previous command caused the slot to be occupied.
            // SWERR for now. Whether this can actually happen is not known at this time.
            BDEBUG(BD_SWERR, "slot(%s) import of capsule(%s) slot is occupied by capsule %s", slot->name, capsule->name(), slot->capsule->name());
        }
    }
    else
    {
        if (!lockAcquired)
        {
            rtsLock();
        }
        const UMLRTCommsPort * * borderPorts = NULL; // Returned ports are not used - just used as an indication of the binding success.

        if ((borderPorts = bindPorts(slot, capsule, capsule->getSlot()->capsuleClass, slot->role()->capsuleClass, capsule->getBorderPorts(), true/*bind*/, true/*import*/)) == NULL)
        {
            // Failed compatibility check.
            if (synchronous)
            {
                // Need to set the requestor's error code.
                slot->controller->setError(UMLRTController::E_IMPORT_COMPAT);
            }
            else
            {
                // May occur if this was an IMPORT Controller Command and some previous previous activity made the slot incompatible?
                // SWERR for now.
                BDEBUG(BD_SWERR, "Controller bind of ports during import failed into slot %s - capsule's original slot %s.", slot->name, capsule->name());
            }
        }
        else
        {
            // Import succeeded.
            delete[] borderPorts; // Do not need this copy for import - the capsule's border ports are updated.

            // Update the capsule instance here and it's capsule class. They are restored during deport. This controller owns the slot now.
            slot->controller = capsule->getSlot()->controller;
            slot->capsule = capsule;
            slot->capsuleClass = slot->capsule->getSlot()->capsuleClass;
            ok = true;
        }
        if (!lockAcquired)
        {
            rtsUnlock();
        }
    }
    return ok;
}

// Recurse into sub-structure requesting a destroy of the slot from the associated controller.
/*static*/ void UMLRTFrameService::controllerIncarnate ( UMLRTCapsule * capsule, size_t sizeSerializedData, void * serializedData )
{
    // Can't acquire the RTS lock for this, since the capsule initialize signal can result in signal sends from the capsule.
    if (capsule == NULL)
    {
        FATAL("attempt to initialize NULL capsule");
    }
    initializeCapsule(capsule, sizeSerializedData, serializedData);
}

/*static*/ const UMLRTCommsPort * * UMLRTFrameService::createInternalPorts ( UMLRTSlot * slot, const UMLRTCapsuleClass * capsuleClass )
{
    const UMLRTCommsPort * * ports = new const UMLRTCommsPort * [capsuleClass->numPortRolesInternal];
    for (size_t i = 0; i < capsuleClass->numPortRolesInternal; ++i)
    {
        ports[i] = createPort(slot, capsuleClass, i, false/*border*/, false/*importProxy*/, false/*isUnbound*/);
    }
    return ports;
}

/*static*/ char * UMLRTFrameService::createName ( const char * prefix, const char * suffix, size_t index, bool appendIndex )
{
    char ind[23]; // 18,446,744,073,709,551,615 is largest possible index in a 64-bit architecture.
    ind[0] = '\0';
    if (appendIndex)
    {
        std::ostringstream oss;
        oss << "[" << index << "]";
        strncpy(ind, oss.str().c_str(), sizeof(ind));
    }
    char * name = (char*)malloc(strlen(prefix) + strlen(suffix) + strlen(ind) + 2);

    if (!name)
    {
        FATAL("could not allocate part name prefix(%s) suffix(%s) index(%d) appendIndex(%d)", prefix, suffix, index, appendIndex);
    }
    strcpy(name, prefix);
    strcat(name, ".");
    strcat(name, suffix);
    if (appendIndex)
    {
        strcat(name, ind);
    }
    return name;
}

/*static*/ UMLRTCapsulePart * UMLRTFrameService::createParts ( const char * containerName, const UMLRTCapsuleClass * containerClass )
{
    UMLRTCapsulePart * parts = NULL;

    if (containerClass->numSubcapsuleRoles)
    {
        parts = new UMLRTCapsulePart[containerClass->numSubcapsuleRoles];

        for (size_t i = 0; i < containerClass->numSubcapsuleRoles; ++i)
        {
            const UMLRTCapsuleRole * role = &containerClass->subcapsuleRoles[i];
            parts[i].containerClass = containerClass;
            parts[i].numSlot = role->multiplicityUpper;
            parts[i].slots = new UMLRTSlot * [role->multiplicityUpper];
            parts[i].roleIndex = i;

            for (size_t j = 0; j < role->multiplicityUpper; ++j)
            {
                // define the slot for this part
                char * name = createName(containerName, role->name, j, (role->multiplicityUpper > 1)/*appendIndex*/);

                // Create the sub-structure
                const UMLRTCapsulePart * subcapsuleParts = createParts( name, role->capsuleClass );

                UMLRTSlot templateSlot = {
                        name,
                        j, // capsuleIndex
                        role->capsuleClass,
                        containerClass,
                        i,
                        NULL, // capsule,
                        NULL, // controller
                        role->capsuleClass->numSubcapsuleRoles, // numParts
                        subcapsuleParts,
                        role->capsuleClass->numPortRolesBorder,
                        NULL, // ports
                        NULL, // slotToBorderMap
                        0, // generated
                        0, // condemned
                };
                parts[i].slots[j] = new UMLRTSlot(templateSlot);
                parts[i].slots[j]->ports = createPortArray(parts[i].slots[j], role->capsuleClass);
            }
        }
    }
    return parts;
}

/*static*/ const UMLRTCommsPort * UMLRTFrameService::createPort ( UMLRTSlot * slot, const UMLRTCapsuleClass * capsuleClass, int roleIndex,
        bool border, bool importProxy, bool isUnbound )
{
    UMLRTCommsPort templatePort;
    definePort(&templatePort, slot, capsuleClass, roleIndex, border, importProxy, isUnbound);
    return new const UMLRTCommsPort(templatePort);
}

/*static*/ const UMLRTCommsPort * UMLRTFrameService::createPortArray ( UMLRTSlot * slot, const UMLRTCapsuleClass * capsuleClass )
{
    UMLRTCommsPort * ports = new UMLRTCommsPort[capsuleClass->numPortRolesBorder];

    for (size_t i = 0; i < capsuleClass->numPortRolesBorder; ++i)
    {
        definePort( &ports[i], slot, capsuleClass, i, true/*border*/, false/*importProxy*/, false/*isUnbound*/);
    }
    return ports;
}

/*static*/ const UMLRTCommsPort * * UMLRTFrameService::createBorderPorts ( UMLRTSlot * slot, size_t numPorts )
{
    const UMLRTCommsPort * * borderPorts = new const UMLRTCommsPort * [numPorts];

    for (size_t i = 0; i < numPorts; ++i)
    {
        borderPorts[i] = &slot->ports[i];
    }
    return borderPorts;
}

/*static*/ void UMLRTFrameService::definePort ( UMLRTCommsPort * port, UMLRTSlot * slot, const UMLRTCapsuleClass * capsuleClass, int roleIndex, bool border, bool importProxy, bool isUnbound )
{
    const UMLRTCommsPortRole * portRole = border ? &capsuleClass->portRolesBorder[roleIndex] : &capsuleClass->portRolesInternal[roleIndex];

    port->containerClass = capsuleClass;
    port->roleIndex = roleIndex;
    port->slot = slot;
    port->numFarEnd = isUnbound ? 0 : portRole->numFarEnd;

    port->automatic = portRole->automatic;
    port->border = border;
    port->generated = false;
    port->locked = portRole->locked;
    port->notification = portRole->notification;
    port->proxy = importProxy;
    port->relay = false; // Set true during port binding, if appropriate.
    port->sap = portRole->sap;
    port->spp = portRole->spp;
    port->unbound = isUnbound;
    port->wired = portRole->wired;

    if (portRole->registeredName != NULL)
    {
        port->registeredName = strdup(portRole->registeredName);
    }
    else
    {
        port->registeredName = NULL;
    }
    if (port->numFarEnd == 0)
    {
        port->farEnds = NULL;
        port->deferQueue = NULL;
    }
    else
    {
        port->farEnds = new UMLRTCommsPortFarEnd[portRole->numFarEnd];
        port->deferQueue = new UMLRTMessageQueue();
    }
    // Leave all ports disconnected for now. We connect them after the structure is built.
    for (size_t j = 0; j < port->numFarEnd; ++j)
    {
        port->farEnds[j].farEndIndex = 0;
        port->farEnds[j].port = NULL;
    }
}

/*static*/ void UMLRTFrameService::defineSlotControllers ( UMLRTSlot * slot, const UMLRTCapsulePart * part, const UMLRTCapsuleClass * capsuleClass,
        const char * logThread, UMLRTController * assignedController, UMLRTController *defaultController, int index )
{
    // Use the logical thread name, controller instance, or the default controller to determine the assigned controller.
    slot->controller = getCapsuleController(slot, part, logThread, assignedController, defaultController, index);

    if (slot->controller == NULL)
    {
        FATAL("slot %s controller NULL", slot->name);
    }
    // Recurse into sub-structure
    for (size_t i = 0; i < slot->numParts; ++i)
    {
        for (size_t j = 0; j < slot->parts[i].numSlot; ++j)
        {
            defineSlotControllers(slot->parts->slots[j],
                    &slot->parts[i],
                    slot->parts->slots[j]->capsuleClass,
                    logThread,
                    assignedController,
                    slot->controller,
                    j);
        }
    }
}

/*static*/ void UMLRTFrameService::deportParts ( UMLRTSlot * slot )
{
    // Assumes global RTS lock already acquired.

    requestControllerDeport(slot, true/*lockAcquired*/);

    for (size_t i = 0; i < slot->numParts; ++i)
    {
        for (size_t j = 0; j < slot->parts[i].numSlot; ++j)
        {
            // set 'condemned' on sub-structure.
            deportParts(slot->parts[i].slots[j]);
        }
    }
}

/*static*/ void UMLRTFrameService::destroyPartsBookkeeping ( size_t numParts, const UMLRTCapsulePart * parts )
{
    // Assumes global RTS lock already acquired.

    // Deletes the array of slot pointers, but the not the slots themselves.
    for (size_t i = 0; i < numParts; ++i)
    {
        delete[] parts[i].slots;
    }
    delete[] parts;
}

/*static*/ void UMLRTFrameService::destroyPort ( const UMLRTCommsPort * port, bool deletePort )
{
    // Assumes global RTS lock already acquired.
    if (port != NULL)
    {
        if (port->spp)
        {
            UMLRTProtocol::deregisterSppPort(port);
        }
        else if (port->sap)
        {
            UMLRTProtocol::deregisterSapPort(port);
        }
        if (port->generated)
        {
            FATAL("trying to destroy a static slot %s port %s", port->slot, port->role()->name);
        }
        if (port->numFarEnd > 0)
        {
            delete[] port->farEnds;
        }
        if (port->deferQueue)
        {
            port->purge();
            delete port->deferQueue;
        }
        if (port->registeredName != NULL)
        {
            free(port->registeredName);
        }
        if (deletePort)
        {
            delete port;
        }
    }
}

/*static*/ void UMLRTFrameService::destroyPortArray ( const UMLRTCommsPort * ports, const size_t numPorts )
{
    // Assumes global RTS lock already acquired.

    if (ports != NULL)
    {
        for (size_t i = 0; i < numPorts; ++i)
        {
            destroyPort(&ports[i], false/*deletePort*/); // The ports are part of the array and not individual objects.
        }
        delete[] ports;
    }
}

/*static*/ void UMLRTFrameService::destroyPortList ( const UMLRTCommsPort * * ports, size_t numPorts, bool proxiesOnly )
{
    // Assumes global RTS lock already acquired.

    if (ports != NULL)
    {
        for (size_t i = 0; i < numPorts; ++i)
        {
            if (ports[i])
            {
                // If 'proxiesOnly' is true and the port is not a proxy, then the port is a slot port and should not be destroyed.
                if (!proxiesOnly || ports[i]->proxy)
                {
                    destroyPort(ports[i], true/*deletePort*/);
                }
            }
        }
        delete[] ports;
    }
}

/*static*/ void UMLRTFrameService::disconnectPort ( const UMLRTCommsPort * port, int index )
{
    // Assumes global RTS lock already acquired.

    if (port)
    {
        if (!port->unbound)
        {
            if (port->farEnds[index].port != NULL)
            {
                port->farEnds[index].port = NULL;
                port->farEnds[index].farEndIndex = 0;
            }
        }
    }
}

// Count the number of free far-ends on a port.
/*static*/ size_t UMLRTFrameService::freeFarEndsCount ( const UMLRTCommsPort * port )
{
    size_t count = 0;

    if (port != NULL)
    {
        for (size_t i = 0; i < port->numFarEnd; ++i)
        {
            if (port->farEnds[i].port == NULL)
            {
                ++count;
            }
        }
    }
    return count;
}


/*static*/ int UMLRTFrameService::findFarEndInstance ( const UMLRTCommsPort * farEnd, size_t farEndIndex, const UMLRTCommsPort * port )
{
    // Assumes global RTS lock already acquired.

    int instance_i = -1;

    if (farEnd)
    {
        if (!port->unbound)
        {
            for (size_t i = 0; (i < port->role()->numFarEnd) && (instance_i == -1); ++i)
            {
                if ((farEnd == port->farEnds[i].port) && (farEndIndex == port->farEnds[i].farEndIndex))
                {
                    instance_i = i;
                }
            }
        }
    }
    return instance_i;
}

// Assumes AT MOST one of 'logThread' or 'assignedController' is non-NULL. (Both may be NULL.)
/*static*/ UMLRTController * UMLRTFrameService::getCapsuleController ( const UMLRTSlot * slot, const UMLRTCapsulePart * part, const char * logThread,
                                UMLRTController * assignedController, UMLRTController * defaultController, int slotIndex )
{
    UMLRTController * capsuleController = NULL;

    if ((logThread != NULL) && (assignedController != NULL))
    {
        FATAL("Only one of logThread and controller may be non-NULL when assigning a controller to capsule %s.",
                part->slots[slotIndex]->name);
    }
    else if (defaultController == NULL)
    {
        FATAL("Default controller NULL assigning a controller to capsule %s.",
                part->slots[slotIndex]->name);
    }
    else if ((capsuleController = UMLRTCapsuleToControllerMap::getControllerForCapsule(part->slots[slotIndex]->name, slot->capsuleClass)) == NULL)
    {
        if ((logThread == NULL) && (assignedController == NULL))
        {
            capsuleController = defaultController; // Default to current controller.
        }
        else if (logThread != NULL)
        {
            if ((capsuleController = UMLRTCapsuleToControllerMap::getControllerFromName(logThread)) == NULL)
            {
                capsuleController = defaultController; // Default to current controller.
            }
        }
        else if ((capsuleController = assignedController) == NULL)
        {
            capsuleController = defaultController;  // Default to current controller.
        }
    }
    BDEBUG(BD_CONTROLLERMAP, "getCapsuleController capsule %s (index %d) input controller(%s) logThread(%s) assigned to %s\n",
                    part->slots[slotIndex]->name,
                    slotIndex,
                    assignedController == NULL ? "NULL" : assignedController->getName(),
                    logThread == NULL ? "NULL" : logThread, capsuleController->getName());

    return capsuleController;
}

// Get the next free capsule slot - return -1 if there's none.
/*static*/ int UMLRTFrameService::getNextFreeCapsuleSlot ( const UMLRTCapsulePart * part )
{
    int nextFree = -1;

    // Look for a slot with no controller assigned.

    for (size_t i = 0; (i < part->numSlot) && (nextFree == -1); ++i)
    {
        if (part->slots[i]->capsule == NULL)
        {
            nextFree = i;
        }
    }
    return nextFree;
}

/*static*/ const UMLRTRtsInterface * UMLRTFrameService::getRtsInterface ( )
{
    return &rtsifUmlrt;
}


/*static*/ bool UMLRTFrameService::importCapsule ( const UMLRTCommsPort * srcPort, UMLRTCapsule * capsule, const UMLRTCapsulePart * destPart, int index )
{
    bool ok = false;

    //const UMLRTSlot * srcSlot = capsule->getSlot();

    if (!destPart->role()->plugin)
    {
        // Returned 'ok' will be false.
        srcPort->slot->controller->setError(UMLRTController::E_IMPORT_NONPLUG);
    }
    // Validate the index if it was explicitly set.
    else if ((index < -1) || (index >= (int)destPart->numSlot))
    {
        // Returned 'ok' will be false.
        srcPort->slot->controller->setError(UMLRTController::E_IMPORT_INV_IDX);
    }
    else
    {
        if (index < 0)
        {
            index = getNextFreeCapsuleSlot(destPart);
        }
        if (index < 0)
        {
            // Returned 'ok' will be false.
            srcPort->slot->controller->setError(UMLRTController::E_IMPORT_NO_FREE_SLOT);
        }
        else if (index >= (int)destPart->numSlot)
        {
            FATAL("internal error obtaining next free capsule index(%d) max(%d)", index, destPart->numSlot);
        }
        else
        {
            // Obtain RTS lock.
            rtsLock();

            UMLRTSlot * destSlot = destPart->slots[index];

            const UMLRTCommsPort * * borderPorts = NULL; // Not used to modify capsule instance - only used to check binding.

            bool compatible = true; // Assume we're compatible unless we've proven otherwise.

            if (destSlot->capsule != NULL)
            {
                srcPort->slot->controller->setError(UMLRTController::E_IMPORT_OCCUPIED);
                compatible = false;
            }
            else if (!capsule->getSlot()->controller->isMyThread())
            {
                // We are not running in the context of the controller that owns the capsule, all we'll do is check compatibility.
                // We check the binding, but don't actually bind - that's done in the context of the slot's controller.

                if ((borderPorts = bindPorts(destSlot, capsule, capsule->getSlot()->capsuleClass, destPart->role()->capsuleClass, capsule->getBorderPorts(), false/*bind*/, true/*import*/)) == NULL)
                {
                    // Requested capsule type does not have port-compatibility.
                    srcPort->slot->controller->setError(UMLRTController::E_IMPORT_COMPAT);
                    compatible = false;
                }
            }
            if (compatible)
            {
                // Either we're running in the context of the controller that owns the capsule (and port binding has not been verified,
                // and the error code will be set by the following if it fails), or we're running in another thread, in which case the binding
                // has been checked and is ok.
                ok = requestControllerImport(destSlot, capsule, true/*lockAcquired*/);
            }

            // Unlock RTS
            rtsUnlock();

            // We don't need this copy of the border ports - it was used to check port binding success only.
            if (borderPorts != NULL)
            {
                delete[] borderPorts;
            }
        }
    }
    return ok;
}

/*static*/ const UMLRTCapsuleId UMLRTFrameService::incarnateCapsule ( const UMLRTCommsPort * srcPort, const UMLRTCapsulePart * part, const UMLRTCapsuleClass * capsuleClass,
                                            const void * userData, const UMLRTObject_class * type, const char * logThread, UMLRTController * assignedController, int index )
{
    UMLRTCapsuleId id(NULL);

    // Default to part's capsuleClass.
    if (capsuleClass == NULL)
    {
        capsuleClass = part->role()->capsuleClass;
    }
    // Verify type supplied if data supplied.
    if ((userData) && (!type))
    {
        srcPort->slot->controller->setError(UMLRTController::E_INC_DATATYPE);
    }
    // Validate the index if it was explicitly set.
    else if ((index < -1) || (index >= (int)part->numSlot))
    {
        // Returned id will be the 'invalid capsule id'.
        srcPort->slot->controller->setError(UMLRTController::E_INC_INV_IDX);
    }
    else if (!part->role()->optional)
    {
        // Attempting to incarnate into a non-optional slot.
        srcPort->slot->controller->setError(UMLRTController::E_INC_NOT_OPT);
    }
    else
    {
        if (index < 0)
        {
            index = getNextFreeCapsuleSlot(part);
        }
        if (index < 0)
        {
            // Returned id will be the 'invalid capsule id'.
            srcPort->slot->controller->setError(UMLRTController::E_INC_NO_FREE_SLOT);
        }
        else if (index >= (int)part->numSlot)
        {
            FATAL("internal error obtaining next free capsule index(%d) max(%d)", index, part->numSlot);
        }
        else
        {
            // Obtain global RTS lock
            rtsLock();

            UMLRTSlot * slot = part->slots[index];

            const UMLRTCommsPort * * borderPorts = NULL; // Defines the border port mapping.

            bool compatible = false;

            if (slot == NULL)
            {
                FATAL("slot %s has NULL part slot[%d]", srcPort->slot->name, index);
            }
            if (slot->capsule != NULL)
            {
                srcPort->slot->controller->setError(UMLRTController::E_INC_SLOT_OCC);
            }
            else if (slot->parts != NULL)
            {
                srcPort->slot->controller->setError(UMLRTController::E_INC_PARTS_ERROR);
            }
            else if (slot->capsuleClass == capsuleClass)
            {
                compatible = true; // The border ports are the slot ports.
                borderPorts = createBorderPorts(slot, capsuleClass->numPortRolesBorder);
            }
            else if ((borderPorts  = bindPorts(slot, NULL, capsuleClass, part->role()->capsuleClass, NULL /*borderPorts*/, true/*bind*/, false/*import*/)) != NULL)
            {
                // Bound OK - and the capsule's borderPorts are defined.
                compatible = true;
            }
            else
            {
                srcPort->slot->controller->setError(UMLRTController::E_INC_COMPAT);
            }
            if (!compatible)
            {
                // Error already set - occupied, parts error, requested capsule type does not have port-compatibility.
                if (borderPorts)
                {
                    delete[] borderPorts;
                }
            }
            else
            {
               // Over-write with occupier's type.
                slot->capsuleClass = capsuleClass;

                // Recurse into sub-structure creating parts.
                slot->parts = createParts(slot->name, capsuleClass);
                slot->numParts = capsuleClass->numSubcapsuleRoles;

                // Recurse into sub-structure assigning the controllers to the capsule and parts.
                defineSlotControllers(slot, part, capsuleClass, logThread, assignedController, srcPort->slot->controller, index);

                BDEBUG(BD_INSTANTIATE, "instantiate capsule class %s into slot %s\n", slot->capsuleClass->name, slot->name);

                slot->capsuleClass->instantiate(&rtsifUmlrt, slot, borderPorts);

                if (slot->capsule)
                {
                    // Recurse into sub-structure requesting the associated controllers to send the capsule initialize messages.
                    requestControllerIncarnate(slot, userData, type);
                }
                // Send rtBound to slot far-ends that are not relay ports.
                incarnateSendBoundSlotFarEnd(slot);
                id = UMLRTCapsuleId(slot->capsule);
            }
            rtsUnlock();
        }
    }
    return id;
}

/*static*/ void UMLRTFrameService::incarnateSendBoundSlotFarEnd ( UMLRTSlot * slot )
{
    // Assumes global RTS lock already acquired.
    const UMLRTCommsPort * * borderPorts = slot->capsule->getBorderPorts();

    // Examine the capsule's class slotToBorderMap to see if any of the capsules border ports were mapped to this slot.
    for (size_t slot_i = 0; (borderPorts != NULL) && (slot_i < slot->role()->capsuleClass->numPortRolesBorder); ++slot_i)
    {
        int border_i = slot_i;

        if (slot->slotToBorderMap != NULL)
        {
            // If this slot port was mapped to border port via the slotToBorderMap...
            border_i = slot->slotToBorderMap[slot_i];
        }
        if (border_i != -1)
        {
            // This capsule border port slot had bindings on this slot port - notify those far-ends of the rtBound.
            if (borderPorts[border_i] == &slot->ports[slot_i])
            {
                // Capsule has a border port mapped directly to the slot port.
                for (size_t j = 0; !borderPorts[border_i]->relay && (j < slot->ports[slot_i].numFarEnd); ++j)
                {
                    // If port is non-relay, notify the far-end of rtBound.
                    if ((slot->ports[slot_i].farEnds[j].port != NULL) && !slot->ports[slot_i].relay)
                    {
                        // Send rtBound to far-end.
                        sendBoundUnbound( slot->ports[slot_i].farEnds[j].port, slot->ports[slot_i].farEnds[j].farEndIndex,
                                &slot->ports[slot_i], slot_i, true/*isBind*/);
                    }
                }
            }
            // slotToBorderMap[border_i] should be -1 for unbound ports.
            else if (borderPorts[border_i]->unbound)
            {
                FATAL("Sending rtBound to slot far end. slot %s and mapped slot port[%d] != border port[%d] and port was unbound ?",
                        slot->name, slot_i, border_i );
            }
            else
            {
                for (size_t j = 0; !borderPorts[border_i]->relay && (j < slot->ports[slot_i].numFarEnd); ++j)
                {
                    // Non relay port.
                    if (slot->ports[slot_i].farEnds[j].port != NULL)
                    {
                        int instance_i;
                        // This slot port instance has a far-end.
                        if ((instance_i = findFarEndInstance(slot->ports[slot_i].farEnds[j].port, slot->ports[slot_i].farEnds[j].farEndIndex, borderPorts[border_i])) < 0)
                        {
                            FATAL("slot %s port %s bound to far end %s not found on border proxy port id(%d)",
                                    slot->name, slot->ports[slot_i].role()->name, slot->ports[slot_i].farEnds[j].port->slot->name, border_i);
                        }
                        else
                        {
                            // Send rtUnbound to slot port far-end - the local port (as seen from the far-end) remains the slot port instance.
                            sendBoundUnbound( slot->ports[slot_i].farEnds[j].port, slot->ports[slot_i].farEnds[j].farEndIndex, &slot->ports[slot_i], slot_i, true/*isBind*/);
                        }
                    }
                }
            }
        }
    }
}

/*static*/ void UMLRTFrameService::initializeCapsule ( UMLRTCapsule * capsule, size_t sizeSerializedData, void * serializedData )
{
    // Assumes rts lock not acquired. capsule->initialize() may send messages.

    BDEBUG(BD_INSTANTIATE, "initialize capsule %s (role %s, class %s)\n", capsule->name(), capsule->getName(), capsule->getTypeName());

    UMLRTInMessage initializeMsg;

    initializeMsg.signal.initialize("initialize", sizeSerializedData);

    // Put the data in the signal.
    memcpy(initializeMsg.signal.getPayload(), serializedData, sizeSerializedData);

    // Deallocate serialized data - it was allocated immediately prior to the call to this method and is deallcoated here.
    if (serializedData != NULL)
    {
        free(serializedData);
    }
    // Register automatic SAPs and SPPs.
    initializeCapsulePorts(capsule);

    // Send rtBound/rtUnbound for capsule ports - far-ends not handled here.
    sendBoundUnboundForCapsule(capsule, true/*isBind*/);

    // Initialize the capsule, sending the initial message.
    capsule->initialize(initializeMsg);
}

/*static*/ void UMLRTFrameService::initializeCapsulePorts ( UMLRTCapsule * capsule )
{
    // Assume RTS lock not acquired, but acquires it for this.
    rtsLock();

    initializePortList(capsule->getBorderPorts(), capsule->getClass()->numPortRolesBorder);
    initializePortList(capsule->getInternalPorts(), capsule->getClass()->numPortRolesInternal);

    rtsUnlock();
}

/*static*/ void UMLRTFrameService::initializePort ( const UMLRTCommsPort * port )
{
    // Create defer queue for appropriate ports.
    if (port != NULL)
    {
        if ((port->deferQueue == NULL) && !port->unbound && (port->wired || port->sap || port->spp))
        {
            port->deferQueue = new UMLRTMessageQueue();
        }
        // Handle automatic service port registration.
        if (port->sap && (port->registeredName != NULL) && (port->automatic))
        {
            UMLRTProtocol::registerSapPort(port, port->registeredName);
        }
        else if (port->spp && (port->registeredName != NULL) && (port->automatic))
        {
            UMLRTProtocol::registerSppPort(port, port->registeredName);
        }
    }
}

/*static*/ void UMLRTFrameService::initializePortList ( const UMLRTCommsPort * * ports, size_t numPorts )
{
    for (size_t i = 0; (ports != NULL) && (i < numPorts); ++i)
    {
        initializePort(ports[i]);
    }
}

/*static*/ void UMLRTFrameService::instantiate ( UMLRTSlot * slot, const UMLRTCapsuleClass * capsuleClass )
{
    // Utility method for generated code to instantiate a static sub-capsule whose border ports are the slot ports.

    // Assumes RTS lock acquired.
    capsuleClass->instantiate(&rtsifUmlrt, slot, createBorderPorts(slot, capsuleClass->numPortRolesBorder));
}

/*static*/ const UMLRTCommsPort * UMLRTFrameService::isBound ( const UMLRTCommsPort * port, int index, int * farEndIndexP )
{
    // Assumes local slot is occupied by a capsule instance.

    const UMLRTCommsPort * farEndPort;

    if (!port->unbound && ((farEndPort = port->farEnds[index].port) != NULL))
    {
        *farEndIndexP = port->farEnds[index].farEndIndex;

        if (!farEndPort->slot->capsule)
        {
            // Far-end slot is not occupied.
            farEndPort = NULL;
            *farEndIndexP = 0;
        }
    }
    return farEndPort;
}

// Request a deport.
/*static*/ void UMLRTFrameService::requestControllerDeport ( UMLRTSlot * slot, bool lockAcquired )
{
    if (slot->role()->plugin && (slot->capsule != NULL))
    {
        if (slot->controller->isMyThread())
        {
            // We can perform the deport now because we're running in the context of the capsule's controller.
            controllerDeport(slot, true/*synchronous*/, lockAcquired);
        }
        else
        {
            // We send a DEPORT Controller Command to have the capsule's controller execute the deport.
            slot->controller->enqueueDeport(slot);
        }
    }
}

/*static*/ void UMLRTFrameService::requestControllerDestroy ( UMLRTSlot * slot, bool isTopSlot, bool lockAcquired )
{
    if (!isTopSlot)
    {
        // This is NOT the top-capsule in the destruction.
        // Go ahead and recurse into sub-structure performing destroy requests (i.e capsules along with the slots themselves).
        for (size_t i = 0; i < slot->numParts; ++i)
        {
             for (size_t j = 0; j < slot->parts[i].numSlot; ++j)
             {
                 requestControllerDestroy(slot->parts[i].slots[j], false/*isTopSlot*/, lockAcquired);
             }
        }
    }
    if (slot->controller->isMyThread())
    {
        // We can perform the destroy now because we're running in the context of the destroyed capsule's controller.
        controllerDestroy(slot, isTopSlot, true/*synchronous*/, lockAcquired);
    }
    else
    {
        // We send a DESTROY Controller Command to have the capsule's controller execute the destroy.
        slot->controller->enqueueDestroy(slot, isTopSlot);
    }
}

/*static*/ bool UMLRTFrameService::requestControllerImport ( UMLRTSlot * slot, UMLRTCapsule * capsule, bool lockAcquired )
{
    bool ok = true;

    if (capsule->getSlot()->controller->isMyThread())
    {
        // The controller who owns the capsule being imported is running - the import can be performed directly now.
        // The capsule instance being imported into the slot must run on the same controller it was originally incarnated on.

        ok = controllerImport(slot, capsule, true/*synchronous*/, lockAcquired);
    }
    else
    {
        // The capsule's controller is not running - must send an IMPORT Controller Command. We've checked port binding once.
        // We can reassign the slot to the capsule's controller and request an import to the controller.
        slot->controller = capsule->getSlot()->controller;

        slot->controller->enqueueImport(slot, capsule);
    }
    return ok;
}

/*static*/ void UMLRTFrameService::requestControllerIncarnate ( UMLRTSlot * slot, const void * userData, const UMLRTObject_class * type )
{
    // We've got the RTS lock - we must queue the initialize signals - even if the running controller is the capsule's controller.
    if (slot->capsule != NULL)
    {
        // Must queue an INCARNATE Controller Command to send the initialize message.
        slot->controller->enqueueIncarnate(slot->capsule, userData, type );
    }
    // Recurse into sub-structure, incarnating sub-slots where appropriate.
    for (size_t i = 0; i < slot->numParts; ++i)
    {
        for (size_t j = 0; j < slot->parts[i].numSlot; ++j)
        {
            requestControllerIncarnate(slot->parts->slots[j], NULL, NULL);
        }
    }
}

/*static*/ void UMLRTFrameService::rtsLock ( )
{
    BDEBUG(BD_LOCK,"rtsLock attempt\n");

    // Outputting the instance model may take a long time - wait a minute and then die.
    if (!rtsGlobalLock.take(60000))
    {
        FATAL("Failed to acquire global RTS lock after waiting a full minute.");
    }
    BDEBUG(BD_LOCK,"rtsLock acquired\n");
}

/*static*/ void UMLRTFrameService::rtsUnlock ( )
{
    BDEBUG(BD_LOCK,"rtsUnlock\n");

    rtsGlobalLock.give();
}

/*static*/ void UMLRTFrameService::sendBoundUnbound ( const UMLRTCommsPort * port, int index, const UMLRTCommsPort * farEndPort, int farEndIndex, bool isBind )
{
    if ((!port->unbound) && (!port->relay) && (port->slot->capsule != NULL) && (port->notification) && (!port->slot->condemned))
    {
        UMLRTSignal signal;
        signal.initialize( isBind ? "rtBound" : "rtUnbound", isBind ? UMLRTSignal::rtBound : UMLRTSignal::rtUnbound, farEndPort);
        port->slot->controller->deliver(port, signal, farEndIndex);

        BDEBUG(BD_BIND,"send %s to slot %s port %s[%d] from slot %s port %s[%d]\n",
                isBind ? "rtBound" : "rtUnbound", port->slot->name, port->role()->name, index,
                farEndPort->slot->name, farEndPort->role()->name, farEndIndex );
    }
}

/*static*/ void UMLRTFrameService::sendBoundUnbound ( const UMLRTCommsPort * * ports, int portId, int farEndIndex, bool isBind )
{
    if (ports != NULL)
    {
        const UMLRTCommsPort * port = ports[portId];
        if (!port->unbound)
        {
            if (farEndIndex < (int)port->numFarEnd)
            {
                const UMLRTCommsPort * farEndPort = port->farEnds[farEndIndex].port;
                if (farEndPort != NULL)
                {
                    if (farEndPort->slot->capsule != NULL)
                    {
                        sendBoundUnbound(port, portId, farEndPort, farEndIndex, isBind);
                        sendBoundUnbound(farEndPort, farEndIndex, port, portId, isBind);
                    }
                }
            }
        }
    }
}

/*static*/ void UMLRTFrameService::sendBoundUnboundForCapsule ( UMLRTCapsule * capsule, bool isBind )
{
    sendBoundUnboundForPortList(capsule->getBorderPorts(), capsule->getClass()->numPortRolesBorder, isBind);
    sendBoundUnboundForPortList(capsule->getInternalPorts(), capsule->getClass()->numPortRolesInternal, isBind);
}

/*static*/ void UMLRTFrameService::sendBoundUnboundForPort ( const UMLRTCommsPort * port, bool isBind )
{
    if (port != NULL)
    {
        for (size_t index = 0; (index < port->numFarEnd) && !port->relay && port->wired; ++index)
        {
            const UMLRTCommsPort * farEndPort;
            int farEndIndex;
            if ((farEndPort = isBound(port, index, &farEndIndex)) != NULL)
            {
                if (isBind)
                {
                    // rtBound - is going to local port.
                    sendBoundUnbound(port, index, farEndPort, farEndIndex, isBind);
                }
                else
                {
                    // rtUnbound - is going to far-end port.
                    sendBoundUnbound(farEndPort, farEndIndex, port, index, isBind);
                }
            }
        }
    }
}

/*static*/ void UMLRTFrameService::sendBoundUnboundForPortList ( const UMLRTCommsPort * * ports, size_t numPorts, bool isBind )
{
    for (size_t i = 0; (ports != NULL) && (i < numPorts); ++i)
    {
        sendBoundUnboundForPort( ports[i], isBind );
    }
}

/*static*/ void UMLRTFrameService::unbindSubcapsulePort ( bool isBorder, UMLRTCapsule * subcapsule, int portIndex, int farEndIndex )
{
    if (subcapsule != NULL)
    {
        BDEBUG(BD_CONNECT, "unbind subcapsule %s portIndex %d fei %d\n", subcapsule->name(), portIndex, farEndIndex);

        subcapsule->unbindPort(isBorder, portIndex, farEndIndex);
    }
}

/*static*/ bool UMLRTFrameService::unbindServicePort ( const UMLRTCommsPort * sapPort, const UMLRTCommsPort * sppPort )
{
    bool ok = false;

    if (sapPort && sppPort)
    {
        if (sapPort->numFarEnd == 1)
        {
            if (sapPort->farEnds[0].port != NULL)
            {
                for (size_t i = 0; (i < sppPort->numFarEnd) && !ok; ++i)
                {
                    if (sppPort->farEnds[i].port == sapPort)
                    {
                        sendBoundUnbound(sapPort, 0, sppPort, i, false/*isBind*/);
                        sendBoundUnbound(sppPort, i, sapPort, 0, false/*isBind*/);
                        disconnectPort(sapPort, 0);
                        disconnectPort(sppPort, i);
                        ok = true;
                    }
                }
            }
        }
    }
    return ok;
}

