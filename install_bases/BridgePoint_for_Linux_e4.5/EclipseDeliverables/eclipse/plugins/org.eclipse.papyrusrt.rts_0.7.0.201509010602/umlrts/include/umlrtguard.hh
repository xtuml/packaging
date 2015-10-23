// umlrtguard.hh

/*******************************************************************************
* Copyright (c) 2014-2015 Zeligsoft (2009) Limited  and others.
* All rights reserved. This program and the accompanying materials
* are made available under the terms of the Eclipse Public License v1.0
* which accompanies this distribution, and is available at
* http://www.eclipse.org/legal/epl-v10.html
*******************************************************************************/

#include "umlrtmutex.hh"

// UMLRTGuard takes a mutex in constructor, gives in destructor.

class UMLRTGuard
{
public:
    UMLRTGuard( UMLRTMutex & m ) : mutex( m ) { mutex.take(); }
    ~UMLRTGuard() { mutex.give(); }

private:
    UMLRTMutex & mutex;
};


