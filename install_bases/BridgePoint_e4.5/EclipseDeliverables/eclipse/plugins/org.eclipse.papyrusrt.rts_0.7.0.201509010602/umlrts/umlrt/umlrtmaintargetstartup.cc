// umlrtmaintargetstartup.cc

/*******************************************************************************
* Copyright (c) 2014-2015 Zeligsoft (2009) Limited  and others.
* All rights reserved. This program and the accompanying materials
* are made available under the terms of the Eclipse Public License v1.0
* which accompanies this distribution, and is available at
* http://www.eclipse.org/legal/epl-v10.html
*******************************************************************************/

#include "umlrtmain.hh"

// See umlrtmain.hh for documentation.

// targetStartup - a method that can be overridden by the user to initialize prior to controllers starting.
//
// return 'false' to exit the application with a failure indication.
// return 'true' to cause application to start controllers.
/*static*/ bool UMLRTMain::targetStartup()
{
     return true; // By default, we return 'ok' and application will spawn the controllers.
}
