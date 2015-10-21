// umlrtinsignal.hh

/*******************************************************************************
* Copyright (c) 2015 Zeligsoft (2009) Limited  and others.
* All rights reserved. This program and the accompanying materials
* are made available under the terms of the Eclipse Public License v1.0
* which accompanies this distribution, and is available at
* http://www.eclipse.org/legal/epl-v10.html
*******************************************************************************/

#ifndef UMLRTINSIGNAL_HH
#define UMLRTINSIGNAL_HH

#include "umlrtsignal.hh"

class UMLRTInSignal : public UMLRTSignal
{
public:
    UMLRTInSignal();
    UMLRTInSignal(const UMLRTInSignal &signal);
    UMLRTInSignal& operator=(const UMLRTInSignal &signal);
    ~UMLRTInSignal();

    // Multiple receivers may be decoding the same signal, so the signal does not
    // maintain the decoding-tracking information internally. The caller to the
    // decode functions must initialize a 'void* decodeInfo' parameter (with NULL)
    // for the FIRST call to the #decode methods, after which is it modified
    // during decoding (i.e. is an input/output parameter to #decode methods).

    void decode( const void * * decodeInfo, const UMLRTObject_class * desc, void * data, int arraySize = 1 ) const;
    void decode( const void * * decodeInfo, const UMLRTObject_class * desc, int ptrIndirection, void * data, int arraySize = 1 ) const;

   // Delete all deferred signals of this type on all port instances. Returns the number of deleted messages.
    int purge();

    // Delete all deferred signals of this type on a specific port instances. Returns the number of deleted messages.
    int purgeAt( int index );

    // Recall one deferred signal of this type on all port instances. Returns the number of recalled messages (0 or 1).
    int recall( bool front = false );

    // Recall all deferred signals of this type on a all port instances. Returns the number of recalled messages (0 or 1).
    int recallAll( bool front = false );

    // Recall all signals of this type from a single port instance. Returns the number of recalled messages.
    int recallAllAt( int index, bool front = false );

    // Recall a signal of this type on a port instance. Returns the number of recalled messages (0 or 1).
    int recallAt( int index, bool front = false );

};

#endif // UMLRTINSIGNAL_HH
