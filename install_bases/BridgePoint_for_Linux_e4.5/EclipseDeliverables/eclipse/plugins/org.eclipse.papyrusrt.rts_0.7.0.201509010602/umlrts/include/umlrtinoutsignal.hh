// umlrtinoutsignal.hh

/*******************************************************************************
* Copyright (c) 2015 Zeligsoft (2009) Limited  and others.
* All rights reserved. This program and the accompanying materials
* are made available under the terms of the Eclipse Public License v1.0
* which accompanies this distribution, and is available at
* http://www.eclipse.org/legal/epl-v10.html
*******************************************************************************/

#ifndef UMLRTINOUTSIGNAL_HH
#define UMLRTINOUTSIGNAL_HH

#include "umlrtinsignal.hh"
#include "umlrtoutsignal.hh"

class UMLRTInOutSignal : public UMLRTInSignal, public UMLRTOutSignal
{
public:
    UMLRTInOutSignal();
    UMLRTInOutSignal(const UMLRTInOutSignal &signal);
    UMLRTInOutSignal& operator=(const UMLRTInOutSignal &signal);
    ~UMLRTInOutSignal();

};

#endif // UMLRTINOUTSIGNAL_HH
