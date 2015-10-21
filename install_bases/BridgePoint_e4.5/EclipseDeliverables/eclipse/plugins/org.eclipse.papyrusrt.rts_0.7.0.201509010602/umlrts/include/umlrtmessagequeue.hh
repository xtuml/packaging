// umlrtmessagequeue.hh

/*******************************************************************************
* Copyright (c) 2014-2015 Zeligsoft (2009) Limited  and others.
* All rights reserved. This program and the accompanying materials
* are made available under the terms of the Eclipse Public License v1.0
* which accompanies this distribution, and is available at
* http://www.eclipse.org/legal/epl-v10.html
*******************************************************************************/

#ifndef UMLRTMESSAGEQUEUE_HH
#define UMLRTMESSAGEQUEUE_HH

#include "umlrtqueue.hh"

// UMLRTMessageQueue - a message queue.

// One of these exists per msg-priority for delivery of messages to capsules.

// One of these exists for the controller 'incoming queue' - messages
// received from other threads.

class UMLRTMessageQueue : public UMLRTQueue
{
public:
    // Constructed empty.
    UMLRTMessageQueue();
};


#endif // UMLRTMESSAGEQUEUE_HH
