// umlrtlogprotocol.hh

/*******************************************************************************
* Copyright (c) 2014-2015 Zeligsoft (2009) Limited  and others.
* All rights reserved. This program and the accompanying materials
* are made available under the terms of the Eclipse Public License v1.0
* which accompanies this distribution, and is available at
* http://www.eclipse.org/legal/epl-v10.html
*******************************************************************************/

#ifndef UMLRTLOGPROTOCOL_HH
#define UMLRTLOGPROTOCOL_HH

#include "umlrtmutex.hh"
#include <stdio.h>
#include <stdarg.h>

struct UMLRTObject_class;
struct UMLRTTypedValue;

// Protocol for log ports.

class UMLRTLogProtocol_baserole
{
public:

    UMLRTLogProtocol_baserole ( ) : ostream(stderr), isOpenFile(false) { }

    // Output data with the new-line appended.
    int log ( const char * fmt, ... ) const;
    int log ( char c ) const;
    int log ( short s ) const;
    int log ( int i ) const;
    int log ( long l ) const;
    int log ( long long ll ) const;
    int log ( unsigned char uc ) const;
    int log ( unsigned short us ) const;
    int log ( unsigned int ui ) const;
    int log ( unsigned long ul ) const;
    int log ( unsigned long long ull ) const;
    int log ( float f ) const;
    int log ( double d ) const;
    int log ( const void * userData, const UMLRTObject_class * type, int arraySize = 1 ) const;
    int log ( const UMLRTTypedValue & value ) const;

    // Output data with NO new-line appended.
    int show ( const char * fmt, ... ) const;
    int show ( char c ) const;
    int show ( short s ) const;
    int show ( int i ) const;
    int show ( long l ) const;
    int show ( long long ll ) const;
    int show ( unsigned char uc ) const;
    int show ( unsigned short us ) const;
    int show ( unsigned int ui ) const;
    int show ( unsigned long ul ) const;
    int show ( unsigned long long ull ) const;
    int show ( float f ) const;
    int show ( double d ) const;
    int show ( const void * userData, const UMLRTObject_class * type, int arraySize = 1 ) const;
    int show ( const UMLRTTypedValue & value ) const;

    // Output newline(s).
    int cr ( int numCr = 1 ) const;

    // Output one newline followed by TAB characters.
    int crtab ( int numTab = 1 ) const;

    // Output a number of space characters.
    int space ( int numSpace = 1 ) const;

    // Output a number of TAB characters.
    int tab ( int numTab = 1 ) const;

    // Flush the output buffer.
    int commit ( ) const;

    // Redirect logging output. The application that uses this variant of #redirect is responsible
    // for opening and closing the stream.
    bool redirect ( FILE * ostream ) const;

    // Redirect to a file by name. The RTS library is responsible for opening and closing the stream
    // associated with the file. (The current version of the library will open the file when this
    // method is called and only close the file when output is redirected to a different destination.
    bool redirect ( const char * fname ) const;

    // stdout and stderr are protected by a mutex to avoid individual log calls from clobbering
    // each other during output. The application is expected to handle mutual exclusion for streams
    // other than stdout and stderr.
    UMLRTMutex * takeMutex ( ) const;
    void giveMutex ( UMLRTMutex * mutex ) const;

private:

    // Output streams to direct log output.
    mutable FILE * ostream;

    // True if ostream is a file and needs to be closed if output is re-directed.
    mutable bool isOpenFile;

    // We avoid concurrent writes from multiple threads to stdout and stderr and the application
    // must handle mutual exclusion for writes to other streams.
    static UMLRTMutex stdoutMutex;
    static UMLRTMutex stderrMutex;
};

#endif // UMLRTLLOGROTOCOL_HH
