// umlrtqueueelement.hh

/*******************************************************************************
* Copyright (c) 2014-2015 Zeligsoft (2009) Limited  and others.
* All rights reserved. This program and the accompanying materials
* are made available under the terms of the Eclipse Public License v1.0
* which accompanies this distribution, and is available at
* http://www.eclipse.org/legal/epl-v10.html
*******************************************************************************/

#ifndef UMLRTQUEUEELEMENT_HH
#define UMLRTQUEUEELEMENT_HH

// UMLRTQueueElement - base type for elements added to queues.

struct UMLRTQueueElement
{
    UMLRTQueueElement ( ) : next(0), qid(-1) { }
    mutable const UMLRTQueueElement * next;
    mutable int qid;
};

#endif // UMLRTQUEUEELEMENT_HH
