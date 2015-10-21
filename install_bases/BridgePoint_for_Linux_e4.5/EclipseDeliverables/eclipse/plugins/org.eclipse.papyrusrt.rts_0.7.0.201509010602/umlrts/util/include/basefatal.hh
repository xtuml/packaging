// basefatal.hh - Platform independent Base fatal - disaster error handling.

/*******************************************************************************
 * Copyright (c) 2015 Zeligsoft (2009) Limited  and others.
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * which accompanies this distribution, and is available at
 * http://www.eclipse.org/legal/epl-v10.html
 *******************************************************************************/

#ifndef BASEFATAL_HH
#define BASEFATAL_HH

#include <errno.h>

namespace base
{
#ifdef __GNUC__ 	/*code for GNU C compiler */
    // Print a error message and die.
    void fatal(const char * file, int line, const char * method, const char * fmt, ...)
        __attribute((noreturn));

    // Die when you have an 'errno'.
    void fatalErrno(const char * file, int line, const char * method, int errno_, const char * api)
        __attribute((noreturn));

    // General suicide.
    #define FATAL( FMT... ) \
        do { ::base::fatal( __FILE__, __LINE__, __PRETTY_FUNCTION__, FMT ); } \
        while( 0 )

    // Suicide when errno holds error.
    #define FATAL_ERRNO( API ) \
        do { ::base::fatalErrno( __FILE__, __LINE__, __PRETTY_FUNCTION__, errno, API ); } \
        while( 0 )

#elif _MSC_VER 		/*code specific to MSVC compiler*/
    // Print a error message and die.
    __declspec(noreturn) void fatal( const char * file, int line, const char * method, const char * fmt, ...);

    // Die when you have an 'errno'.
    __declspec(noreturn) void fatalErrno( const char * file, int line, const char * method, int errno_, const char * api);

    // General suicide.
    #define FATAL( FMT, ... ) \
        do { ::base::fatal( __FILE__, __LINE__, __FUNCSIG__, FMT, __VA_ARGS__ ); } \
        while( 0 )

    // Suicide when errno holds error.
    #define FATAL_ERRNO( API ) \
        do { ::base::fatalErrno( __FILE__, __LINE__, __FUNCSIG__, errno, API ); } \
        while( 0 )

#endif

}

#endif // BASEFATAL_HH
