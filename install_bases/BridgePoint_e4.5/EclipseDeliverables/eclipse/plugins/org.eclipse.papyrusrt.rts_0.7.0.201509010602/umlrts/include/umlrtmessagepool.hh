// umlrtmessagepool.hh

/*******************************************************************************
 * Copyright (c) 2014-2015 Zeligsoft (2009) Limited  and others.
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * which accompanies this distribution, and is available at
 * http://www.eclipse.org/legal/epl-v10.html
 *******************************************************************************/

#ifndef UMLRTMESSAGEPOOL_HH
#define UMLRTMESSAGEPOOL_HH

#include "umlrtpool.hh"
#include "umlrtmessage.hh"
#include "umlrtuserconfig.hh"

// UMLRTMessageElementPool is a pool of free messages.

// The initial version has a single application-wide free-pool used by all
// threads for all messages.

class UMLRTMessagePool: public UMLRTPool
{
public:
    UMLRTMessagePool(UMLRTMessage messages[], size_t arraySize, size_t incrementSize =
            USER_CONFIG_MESSAGE_POOL_INCR);

    UMLRTMessagePool(size_t incrementSize = USER_CONFIG_MESSAGE_POOL_INCR);

private:
    virtual void grow();
};

#endif // UMLRTMESSAGEPOOL_HH
