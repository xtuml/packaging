// umlrtmaintargetshutdown.cc

/*******************************************************************************
* Copyright (c) 2014-2015 Zeligsoft (2009) Limited  and others.
* All rights reserved. This program and the accompanying materials
* are made available under the terms of the Eclipse Public License v1.0
* which accompanies this distribution, and is available at
* http://www.eclipse.org/legal/epl-v10.html
*******************************************************************************/

#include <stdlib.h>
#include "umlrtmain.hh"

// See umlrtmain.hh for documentation.

// targetShutdown - a method that can be overridden by the user to shutdown the execution platform.
//
// The input parameter is the return value of the mainLoop().
//
// Returns the 'exit status' of the application. This default version does nothing and returns EXIT_SUCCESS.
/*static*/ int UMLRTMain::targetShutdown( bool mainLoopReturnValue )
{
     return EXIT_SUCCESS; // By default, we return 'ok' and application will spawn the controllers.
}
