// umlrtsignalelementpool.hh

/*******************************************************************************
 * Copyright (c) 2014-2015 Zeligsoft (2009) Limited  and others.
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * which accompanies this distribution, and is available at
 * http://www.eclipse.org/legal/epl-v10.html
 *******************************************************************************/

#ifndef UMLRTSIGNALPOOL_HH
#define UMLRTSIGNALPOOL_HH

#include "umlrtpool.hh"
#include "umlrtsignalelement.hh"
#include "umlrtuserconfig.hh"

// UMLRTSignalElementPool is a pool of free signal elements.

// The initial version has a single application-wide free-pool used by all
// threads for all messages.

class UMLRTSignalElementPool: public UMLRTPool
{
public:
    UMLRTSignalElementPool(UMLRTSignalElement signalElements[], size_t arraySize,
            size_t incrementSize = USER_CONFIG_SIGNAL_ELEMENT_POOL_INCR);

    UMLRTSignalElementPool(size_t incrementSize = USER_CONFIG_SIGNAL_ELEMENT_POOL_INCR);

private:
    virtual void grow();
};

#endif // UMLRTSIGNALPOOL_HH
