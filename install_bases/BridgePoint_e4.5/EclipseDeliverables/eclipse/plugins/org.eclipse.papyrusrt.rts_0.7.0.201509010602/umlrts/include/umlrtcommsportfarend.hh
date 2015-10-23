// umlrtcommsportfarend.hh

/*******************************************************************************
* Copyright (c) 2014-2015 Zeligsoft (2009) Limited  and others.
* All rights reserved. This program and the accompanying materials
* are made available under the terms of the Eclipse Public License v1.0
* which accompanies this distribution, and is available at
* http://www.eclipse.org/legal/epl-v10.html
*******************************************************************************/

#ifndef UMLRTCOMMSPORTFAREND_HH
#define UMLRTCOMMSPORTFAREND_HH

struct UMLRTCommsPort;

// The 'far-end list' in a UMLRTCommsPort contains a list of these, one per 'connection'.

// A replicated port "A" with multiple connections defined will have multiple instances (each with a different 'index' value)
// defined within the far-end lists of those remote ports that port "A" is connected to.

struct UMLRTCommsPortFarEnd
{
    size_t farEndIndex; // 'port' far-end's replicated instance (i.e. index). This is an index of the remote port - not an index of this 'port'.
    const UMLRTCommsPort * port;
};

#endif // UMLRTCOMMSPORTFAREND_HH
