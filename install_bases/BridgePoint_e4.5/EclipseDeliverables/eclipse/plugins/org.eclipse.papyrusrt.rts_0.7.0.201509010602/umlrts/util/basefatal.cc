// basefatal.cpp - Platform independent fatal error handling.

/*******************************************************************************
* Copyright (c) 2014-2015 Zeligsoft (2009) Limited and others.
* All rights reserved. This program and the accompanying materials
* are made available under the terms of the Eclipse Public License v1.0
* which accompanies this distribution, and is available at
* http://www.eclipse.org/legal/epl-v10.html
*******************************************************************************/

// See basefatal.hh for interface details.

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "basefatal.hh"

namespace base
{
    // A single place to exit from all 'fatal' calls to dump
    // stack trace-back and/or set breakpoint.

    #ifdef __GNUC__     /*code for GNU C compiler */
        static void fatalExit( int status ) __attribute((noreturn));
    #elif _MSC_VER      /*code specific to MSVC compiler*/
        __declspec(noreturn) static void fatalExit( int status );
    #else
        #error "Compiler not supported"
    #endif

    static void fatalExit( int status )
    {
        exit(status);
    }

    // See basefatal.hh for documentation.
    void fatal( const char * file, int line, const char * method, const char * fmt, ...)
    {
        va_list ap;
        va_start(ap, fmt);
        printf("FATAL ERROR:%s:%d:%s:", file, line, method);
        vprintf(fmt, ap);
        printf("\n");
        va_end(ap);
        fatalExit(EXIT_FAILURE);
    }

    // See basefatal.hh for documentation.
    void fatalErrno( const char * file, int line, const char * method, int errno_, const char * api)
    {
        printf("FATAL ERROR:%s:%d:%s:%s:errno(%d):%s\n", file, line, method, api, errno_, strerror(errno_));
        fatalExit(EXIT_FAILURE);
    }
}
