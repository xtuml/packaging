// umlrtinmessage.cc

/*******************************************************************************
* Copyright (c) 2015 Zeligsoft (2009) Limited  and others.
* All rights reserved. This program and the accompanying materials
* are made available under the terms of the Eclipse Public License v1.0
* which accompanies this distribution, and is available at
* http://www.eclipse.org/legal/epl-v10.html
*******************************************************************************/

#include "umlrtapi.hh"
#include "umlrtcontroller.hh"
#include "umlrtinmessage.hh"
#include "umlrtinsignal.hh"
#include "umlrtqueue.hh"
#include "basefatal.hh"

// See umlrtinmessage.hh for documentation.

void UMLRTInMessage::decodeInit ( const UMLRTObject_class * desc_ ) const
{
    desc = desc_;
    decodeInfo = NULL;
}

void UMLRTInMessage::decode ( void * data, int arraySize ) const
{
    if (desc != NULL)
    {
        ((UMLRTInSignal &)signal).decode(&decodeInfo, desc, data, arraySize);
    }
}

void UMLRTInMessage::decode ( int ptrIndirection, void * data, int arraySize ) const
{
    FATAL("UMLRTInMessage::decode ptrIndirection not supported.");
}

void UMLRTInMessage::destroy( void * data, int arraySize ) const
{
    void * s = data;
    if (desc != NULL)
    {
        for (int i = 0; i < arraySize; ++i)
        {
            s = desc->destroy(desc, s);
        }
    }
}

