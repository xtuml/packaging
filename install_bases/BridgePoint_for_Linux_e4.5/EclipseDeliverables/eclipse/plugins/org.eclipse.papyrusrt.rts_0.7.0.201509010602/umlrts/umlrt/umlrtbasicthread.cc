// umlrtbasicthread.cc

/*******************************************************************************
* Copyright (c) 2014-2015 Zeligsoft (2009) Limited  and others.
* All rights reserved. This program and the accompanying materials
* are made available under the terms of the Eclipse Public License v1.0
* which accompanies this distribution, and is available at
* http://www.eclipse.org/legal/epl-v10.html
*******************************************************************************/

#include <string.h>
#include "umlrtbasicthread.hh"

// See umlrtbasicthread.hh for documentation.

/*static*/ void * UMLRTBasicThread::static_entrypoint( void * arg )
{
    threadargs_t *threadargs = (threadargs_t*)arg;

    return threadargs->inst->run( threadargs->args );
}


UMLRTBasicThread::UMLRTBasicThread( const char * name_ ) : tid(0)
{
    memset(name, 0, sizeof(name));
    strncpy(name, name_, sizeof(name) - 1);
}

// Return my name

const char * UMLRTBasicThread::getName() const
{
    return name;
}

 
