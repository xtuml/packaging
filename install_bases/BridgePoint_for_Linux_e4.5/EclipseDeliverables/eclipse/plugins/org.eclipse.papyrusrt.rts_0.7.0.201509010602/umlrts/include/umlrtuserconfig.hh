// umlrtuserconfig.hh - User configuration definitions.

/*******************************************************************************
 * Copyright (c) 2014-2015 Zeligsoft (2009) Limited  and others.
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * which accompanies this distribution, and is available at
 * http://www.eclipse.org/legal/epl-v10.html
 *******************************************************************************/

#ifndef UMLRTUSERCONFIG_H
#define UMLRTUSERCONFIG_H

// --- user-defined configuration parameters -----------------------------------

// The user defines a number of configuration parameters, such as the default
// signal payload size and the number of free message buffers.

// The RTS library will need these parameters for its internal operation at
// run-time.

// For the prototype, we just define them here as macro constants.

// Bug 1 tracks the changes required here.

#define USER_CONFIG_SIGNAL_DEFAULT_PAYLOAD_SIZE     1024

// Pool sizes
#define USER_CONFIG_MESSAGE_POOL_SIZE               1024
#define USER_CONFIG_SIGNAL_ELEMENT_POOL_SIZE        1024
#define USER_CONFIG_TIMER_POOL_SIZE                 1024

// default number of elements to increase pool size
#define USER_CONFIG_MESSAGE_POOL_INCR               50
#define USER_CONFIG_SIGNAL_ELEMENT_POOL_INCR        50
#define USER_CONFIG_TIMER_POOL_INCR                 50

#endif // UMLRTUSERCONFIG_H
