// umlrtinsignal.cc

/*******************************************************************************
* Copyright (c) 2015 Zeligsoft (2009) Limited  and others.
* All rights reserved. This program and the accompanying materials
* are made available under the terms of the Eclipse Public License v1.0
* which accompanies this distribution, and is available at
* http://www.eclipse.org/legal/epl-v10.html
*******************************************************************************/

#include "umlrtinsignal.hh"
#include "basefatal.hh"

UMLRTInSignal::UMLRTInSignal()
{

}

UMLRTInSignal::UMLRTInSignal(const UMLRTInSignal &signal) : UMLRTSignal(signal)
{

}

UMLRTInSignal& UMLRTInSignal::operator=(const UMLRTInSignal &signal)
{
    UMLRTSignal::operator = (signal);
    return *this;
}

UMLRTInSignal::~UMLRTInSignal()
{

}

// See documentation in UMLRTSignal.
void UMLRTInSignal::decode( const void * * decodeInfo, const UMLRTObject_class * desc, void * data, int arraySize ) const
{
    if (!element)
    {
        FATAL("decode attempted on invalid signal.");
    }
    element->decode(decodeInfo, desc, data, arraySize);
}

// See documentation in UMLRTSignal.
void UMLRTInSignal::decode( const void * * decodeInfo, const UMLRTObject_class * desc, int ptrIndirection, void * data, int arraySize ) const
{
    FATAL("encode ptr indirection not supported");
}

int UMLRTInSignal::purge()
{
    int count = 0;
    if (element == NULL)
    {
        FATAL("purge element is NULL");
    }
    const UMLRTCommsPort * srcPort = element->getSrcPort();
    if (srcPort != NULL)
    {
        count = srcPort->purge(-1/*index*/, element->getId());
    }
    return count;
}

int UMLRTInSignal::purgeAt( int index )
{
    int count = 0;
    if (element == NULL)
    {
        FATAL("purge element is NULL");
    }
    const UMLRTCommsPort * srcPort = element->getSrcPort();
    if ((srcPort != NULL) && (index >= 0) && (index < (int)srcPort->numFarEnd))
    {
        count = srcPort->purge(index, element->getId());
    }
    return count;
}

int UMLRTInSignal::recall( bool front )
{
    int count = 0;
    if (element == NULL)
    {
        FATAL("purge element is NULL");
    }
    const UMLRTCommsPort * srcPort = element->getSrcPort();
    if (srcPort != NULL)
    {
        count = srcPort->recall(-1/*index*/, front, true/*one*/, element->getId());
    }
    return count;
}

int UMLRTInSignal::recallAll( bool front )
{
    int count = 0;
    if (element == NULL)
    {
        FATAL("purge element is NULL");
    }
    const UMLRTCommsPort * srcPort = element->getSrcPort();
    if (srcPort != NULL)
    {
        count = srcPort->recall(-1/*index*/, front, false/*one*/, element->getId());
    }
    return count;
}

int UMLRTInSignal::recallAllAt( int index, bool front )
{
    int count = 0;
    if (element == NULL)
    {
        FATAL("purge element is NULL");
    }
    const UMLRTCommsPort * srcPort = element->getSrcPort();
    if ((srcPort != NULL) && (index >= 0) && (index < (int)srcPort->numFarEnd))
    {
        count = srcPort->recall(index, front, false/*one*/, element->getId());
    }
    return count;
}

int UMLRTInSignal::recallAt( int index, bool front )
{
    int count = 0;
    if (element == NULL)
    {
        FATAL("purge element is NULL");
    }
    const UMLRTCommsPort * srcPort = element->getSrcPort();
    if ((srcPort != NULL) && (index >= 0) && (index < (int)srcPort->numFarEnd))
    {
         count = srcPort->recall(index, front, true/*one*/, element->getId());
    }
    return count;
}
