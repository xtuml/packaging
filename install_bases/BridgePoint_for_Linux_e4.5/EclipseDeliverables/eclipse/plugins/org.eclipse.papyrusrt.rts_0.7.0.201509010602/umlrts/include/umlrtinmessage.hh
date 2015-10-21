// umlrtinmessage.hh

/*******************************************************************************
* Copyright (c) 2015 Zeligsoft (2009) Limited  and others.
* All rights reserved. This program and the accompanying materials
* are made available under the terms of the Eclipse Public License v1.0
* which accompanies this distribution, and is available at
* http://www.eclipse.org/legal/epl-v10.html
*******************************************************************************/

#ifndef UMLRTINMESSAGE_HH
#define UMLRTINMESSAGE_HH

#include <stdint.h>
#include "umlrtmessage.hh"

class UMLRTInMessage : public UMLRTMessage
{
public:

    UMLRTInMessage ( ) : UMLRTMessage() {};

    int getSignalId ( ) const { return signal.getId(); }

    void decodeInit ( const UMLRTObject_class * desc ) const;
    void decode ( void * data, int arraySize = 1 ) const;
    void decode ( int ptrIndirection, void * data, int arraySize = 1 ) const;
    int sizeDecoded ( ) const { return (desc == NULL) ? 0 : desc->sizeDecoded; }
    void destroy( void * data, int arraySize = 1 ) const;
};

#endif // UMLRTMESSAGE_HH
