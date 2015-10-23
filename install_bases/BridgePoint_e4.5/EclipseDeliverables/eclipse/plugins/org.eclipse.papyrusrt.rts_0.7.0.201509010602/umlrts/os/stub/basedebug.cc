// basedebug.cpp - Base Debug (BD) run-time debugging.

/*******************************************************************************
 * Copyright (c) 2015 Zeligsoft (2009) Limited  and others.
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * which accompanies this distribution, and is available at
 * http://www.eclipse.org/legal/epl-v10.html
 *******************************************************************************/

#include "basedebug.hh"

namespace base
{

// BD debug printf - called from BDEBUG macros.
void debugPrintf( int bdtype, const char * file, int line, const char * method, const char * fmt, ...)
{
}

// See UMLRTMain.hh
long int debugGetEnabledTypes()
{
    return 0;
}

// See UMLRTMain.hh
void debugEnableSet(bool enabled)
{
}

// See UMLRTMain.hh
void debugEnableColorSet(bool enabled)
{
}

// See UMLRTMain.hh
bool debugEnableTypeMaskNameSet(const char * name, bool enable)
{
    return true;
}

// See UMLRTMain.hh
bool debugEnableTypeMaskSpecSet(char * typeMaskSpec, bool enable)
{
    return true;
}

// See UMLRTMain.hh
void debugEnableTypeNameDisplaySet(bool enabled)
{
}

// See UMLRTMain.hh
void debugEnableTimeDisplaySet(bool enabled)
{
}

// See UMLRTMain.hh
void debugEnableThreadDisplaySet(bool enabled)
{
}

// See UMLRTMain.hh
void debugEnableFilenameDisplaySet(bool enabled)
{
}

// See UMLRTMain.hh
void debugEnableLineNumberDisplaySet(bool enabled)
{
}

// See UMLRTMain.hh
void debugEnableMethodDisplaySet(bool enabled)
{
}

// See UMLRTMain.hh
void debugEnableUserMsgDisplaySet(bool enabled)
{
}

// See UMLRTMain.hh
bool debugTypeEnabled(long int type)
{
    return false;
}

// See UMLRTMain.hh
void debugLogData(long int type, uint8_t * data, size_t size)
{
}

// See UMLRTMain.hh
void debugOptionSummary()
{
}

// See UMLRTMain.hh
void debugTypeSummary()
{
}

// See basedebug.hh
void debugColourReset()
{
}

void debugColourBrightBlue()
{
}
void debugColourBrightRed()
{
}
void debugColourDimBlue()
{
}

// See basedebug.hh for documentation.
void swerr(const char * file, int line, const char * method, const char * fmt, ...)
{
}
// See basedebug.hh for documentation.
void swerrErrno(const char * file, int line, const char * method, int errno_, const char * api)
{
}
}
