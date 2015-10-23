// umlrtoutsignal.hh

/*******************************************************************************
* Copyright (c) 2015 Zeligsoft (2009) Limited  and others.
* All rights reserved. This program and the accompanying materials
* are made available under the terms of the Eclipse Public License v1.0
* which accompanies this distribution, and is available at
* http://www.eclipse.org/legal/epl-v10.html
*******************************************************************************/

#ifndef UMLRTOUTSIGNAL_HH
#define UMLRTOUTSIGNAL_HH

#include "umlrtsignal.hh"

class UMLRTMessage;

class UMLRTOutSignal : public UMLRTSignal
{
public:
    UMLRTOutSignal();
    UMLRTOutSignal(const UMLRTOutSignal &signal);
    UMLRTOutSignal& operator=(const UMLRTOutSignal &signal);
    ~UMLRTOutSignal();

    // The encode functions allow the user to serialize data into the payload.
    // The encoding keeps track of where in the payload the next data-item should be
    // serialized. The 'initialize' function initializes the encoding and each call
    // to the encoding functions moves the encoding-location forward.
    void encode( const UMLRTObject_class * desc, const void * data, int arraySize = 1 );
    void encode( const UMLRTObject_class * desc, int ptrIndirection, const void * data, int arraySize = 1 );

    // Synchronous send out all port instances. Returns the number of replies (0 if fail).
    int invoke( UMLRTMessage * replyMsgs );

    // Synchronous send out a specific port instance. Returns the number of replies (0 or 1).
    int invokeAt( int index, UMLRTMessage * replyMsg );

    // Send the signal to its far end port (or ports, if it is replicated).
    // Return 0 if there is an error.
    bool send( UMLRTSignalElement::Priority priority = UMLRTSignalElement::PRIORITY_NORMAL ) const;

    // Send the signal to an individual port.
    // Return 0 if there is an error.
    bool sendAt( size_t  portIndex, UMLRTSignalElement::Priority priority = UMLRTSignalElement::PRIORITY_NORMAL ) const;

    // Deliver the signal to a destination.
    bool signalDeliver( const UMLRTCommsPort * srcPort, size_t srcPortIndex ) const;

    // Reply to a synchronous message. Return true if success.
    bool reply();

};

#endif // UMLRTINSIGNAL_HH
