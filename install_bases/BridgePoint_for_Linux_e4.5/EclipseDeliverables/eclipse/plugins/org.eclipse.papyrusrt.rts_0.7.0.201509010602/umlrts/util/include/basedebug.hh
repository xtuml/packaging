// basedebug.hh - Platform independent Base Debug (BD) run-time debugging.

/*******************************************************************************
* Copyright (c) 2015 Zeligsoft (2009) Limited  and others.
* All rights reserved. This program and the accompanying materials
* are made available under the terms of the Eclipse Public License v1.0
* which accompanies this distribution, and is available at
* http://www.eclipse.org/legal/epl-v10.html
*******************************************************************************/

#ifndef BASEDEBUG_HH
#define BASEDEBUG_HH

#include <stdint.h>
#include <stdlib.h>

#include "basedebugtype.hh"

// BASE Debug - BD - debug message output.

// BDEBUG is a macro for printing debug messages.
//
// The first parameter is the bdtype - BD_xxxx.

// Individual BD types can be enabled for output by setting the associated
// bit in the rdEnableMessageType (see below).

// By default, all types are disabled. For debugging specific components
// of the library or application, individual BD types can be enabled.

#ifdef __GNUC__     /*code for GNU C compiler */

// Debug message - see BD.
#define BDEBUG(bdtype, FMT... ) \
    do { \
        if ((bdtype > 0) && (bdtype < BD_MAXPLUS1) && (base::debugGetEnabledTypes() & (1 << bdtype))) \
        { \
            base::debugPrintf( bdtype, __FILE__, __LINE__, __PRETTY_FUNCTION__, FMT ); \
        } \
    } while( 0 )

 // General software error.
#define SWERR( FMT... ) \
    do { ::base::swerr( __FILE__, __LINE__, __PRETTY_FUNCTION__, FMT ); } \
    while( 0 )

// Error when errno holds error.
#define SWERR_ERRNO( API ) \
    do { ::base::swerrErrno( __FILE__, __LINE__, __PRETTY_FUNCTION__, errno, API ); } \
    while( 0 )

#elif _MSC_VER      /*code specific to MSVC compiler*/

// Debug message - see BD.
#define BDEBUG(bdtype, FMT, ... ) \
    do { \
        if ((bdtype > 0) && (bdtype < BD_MAXPLUS1) && (base::debugGetEnabledTypes() & (1 << bdtype))) \
        { \
            base::debugPrintf( bdtype, __FILE__, __LINE__, __FUNCSIG__, FMT, __VA_ARGS__ ); \
        } \
    } while( 0 )

// General software error.
#define SWERR( FMT, ... ) \
    do { ::base::swerr( __FILE__, __LINE__, __FUNCSIG__, FMT, __VA_ARGS__ ); } \
    while( 0 )

// Error when errno holds error.
#define SWERR_ERRNO( API ) \
    do { ::base::swerrErrno( __FILE__, __LINE__, __FUNCSIG__, errno, API ); } \
    while( 0 )

#else
#error "Compiler not supported"
#endif

namespace base
{
    // Called from BDEBUG macro to output a debug log message.
    void debugPrintf ( int bdtype, const char * file, int line, const char * method, const char * fmt, ...);

    // Get the enabled types (as a mask)
    long int debugGetEnabledTypes ( );

    // Enable the overall debug log output feature.
    void debugEnableSet ( bool enabled );

    // Enable the terminal color escape sequences.
    void debugEnableColorSet ( bool enabled );

    // Enable/disable a single mask-type by name.
    bool debugEnableTypeMaskNameSet( const char * name, bool enable );

    // Specify a string list of types to enable/disable.
    bool debugEnableTypeMaskSpecSet ( char * typeMaskSpec, bool enable );

    // Enable or disable the type-name part of a debug log output.
    void debugEnableTypeNameDisplaySet ( bool enabled );

    // Enable or disable the timestamp part of a debug log output.
    void debugEnableTimeDisplaySet ( bool enabled );

    // Enable or disable the thread-id part of a debug log output.
    void debugEnableThreadDisplaySet ( bool enabled );

    // Enable or disable the file-name part of a debug log output.
    void debugEnableFilenameDisplaySet ( bool enabled );

    // Enable or disable the file line-number part of a debug log output.
    void debugEnableLineNumberDisplaySet ( bool enabled );

    // Enable or disable the method-name part of a debug log output.
    void debugEnableMethodDisplaySet ( bool enabled );

    // Enable or disable the user-message part of a debug log output.
    void debugEnableUserMsgDisplaySet ( bool enabled );

    // Return TRUE if type enabled.
    bool debugTypeEnabled( long int type );

    // Return TRUE if type enabled.
    void debugLogData( long int type, uint8_t * data, size_t size );

    // Output a summary of debug settings to standard output.
    void debugOptionSummary ( );

    // Output a summary of debug types to standard output.
    void debugTypeSummary ( );

    // Set a colour of output.
    void debugColourDimBlue();
    void debugColourBrightBlue();
    void debugColourBrightRed();

    // Reset the colour.
    void debugColourReset();

    // Print a error message to error log.
    void swerr( const char * file, int line, const char * method, const char * fmt, ...);

    // Print an error message to error log when you have an 'errno'.
    void swerrErrno( const char * file, int line, const char * method, int errno_, const char * api);
}
#endif // BASEDEBUG_HH
