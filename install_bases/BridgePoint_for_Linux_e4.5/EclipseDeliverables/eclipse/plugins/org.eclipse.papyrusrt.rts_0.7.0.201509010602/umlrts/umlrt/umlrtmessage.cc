// umlrtmessage.cc

/*******************************************************************************
* Copyright (c) 2015 Zeligsoft (2009) Limited  and others.
* All rights reserved. This program and the accompanying materials
* are made available under the terms of the Eclipse Public License v1.0
* which accompanies this distribution, and is available at
* http://www.eclipse.org/legal/epl-v10.html
*******************************************************************************/

#include "umlrtapi.hh"
#include "umlrtcontroller.hh"
#include "umlrtmessage.hh"
#include "umlrtqueue.hh"

// See umlrtmessagequeue.hh for documentation.

bool UMLRTMessage::defer ( ) const
{
    bool ok = false;

    UMLRTMessage * msg = umlrt::MessageGetFromPool();
    if (msg == NULL)
    {
        destPort->slot->controller->setError(UMLRTController::E_DEFER_ALLOC);
    }
    else
    {
        msg->srcPortIndex = srcPortIndex;
        msg->sapIndex0 = sapIndex0;
        msg->destPort = destPort;
        msg->destSlot = destSlot;
        msg->signal = signal; // This copy causes the reference count on signal to increment.
        msg->isCommand = isCommand;

        // Append to defer queue.
        destPort->deferQueue->enqueue(msg);
        ok = true;
    }
    return ok;
}
