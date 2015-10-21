// umlrtmainloop.cc

/*******************************************************************************
* Copyright (c) 2014-2015 Zeligsoft (2009) Limited  and others.
* All rights reserved. This program and the accompanying materials
* are made available under the terms of the Eclipse Public License v1.0
* which accompanies this distribution, and is available at
* http://www.eclipse.org/legal/epl-v10.html
*******************************************************************************/

#include "umlrtmain.hh"

// See umlrtmain.hh for documentation.

// mainLoop - a method that can be overridden by the user to implement a main thread.
//
// return 'false' to skip waiting for controllers to run-to-completion and call targetShutdown() immediately.
// return 'true' to cause application to wait for controllers to complete before calling targetShutdown().
/*static*/ bool UMLRTMain::mainLoop()
{
     return true; // By default, we wait for controllers to run-to-completion.
}
