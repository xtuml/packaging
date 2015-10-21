// umlrtobjectclassgeneric.hh

/*******************************************************************************
* Copyright (c) 2015 Zeligsoft (2009) Limited  and others.
* All rights reserved. This program and the accompanying materials
* are made available under the terms of the Eclipse Public License v1.0
* which accompanies this distribution, and is available at
* http://www.eclipse.org/legal/epl-v10.html
*******************************************************************************/

#ifndef UMLRTOBJECTCLASSGENERIC_HH
#define UMLRTOBJECTCLASSGENERIC_HH

#include <new>

// Generic functions for object instance serialization.

template <typename T>
void * UMLRTObjectInitialize ( const UMLRTObject_class * desc, void * data )
{
    new( data ) T;
    return ( (T *)data ) + 1;
}

template <typename T>
void * UMLRTObjectCopy ( const UMLRTObject_class * desc, const void * src, void * dst )
{
    if( ! src )
    {
        return UMLRTObjectInitialize<T>(desc, dst);
    }
    new( dst ) T( *(const T *)src );
    return ( (T *)dst ) + 1;
}

template <typename T>
void * UMLRTObjectDestroy (  const UMLRTObject_class * desc, void * data )
{
    ( (T *)data )->~T();
    return ( (T *)data ) + 1;
}

#endif // UMLRTOBJECTCLASSGENERIC_HH
