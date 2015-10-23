// umlrtlogprotocol.cc

/*******************************************************************************
* Copyright (c) 2014-2015 Zeligsoft (2009) Limited  and others.
* All rights reserved. This program and the accompanying materials
* are made available under the terms of the Eclipse Public License v1.0
* which accompanies this distribution, and is available at
* http://www.eclipse.org/legal/epl-v10.html
*******************************************************************************/

#include "umlrtobjectclass.hh"
#include "umlrtlogprotocol.hh"
#include "umlrtmutex.hh"

// Avoid concurrent output stdout and stderr. The application is responsible for mutual exclusion all other streams.
/*static*/ UMLRTMutex UMLRTLogProtocol_baserole::stdoutMutex;
/*static*/ UMLRTMutex UMLRTLogProtocol_baserole::stderrMutex;

// Gain exclusive access to stdout or stderr - return NULL if stream is not one of these two.
UMLRTMutex * UMLRTLogProtocol_baserole::takeMutex() const
{
    UMLRTMutex * mutex = NULL;
    if (ostream)
    {
        if (ostream == stdout)
        {
            UMLRTLogProtocol_baserole::stdoutMutex.take();
            mutex = &UMLRTLogProtocol_baserole::stdoutMutex;
        }
        else if (ostream == stderr)
        {
            UMLRTLogProtocol_baserole::stderrMutex.take();
            mutex = &UMLRTLogProtocol_baserole::stderrMutex;
        }
    }
    return mutex;
}

void UMLRTLogProtocol_baserole::giveMutex ( UMLRTMutex * mutex ) const
{
    if (mutex != NULL)
    {
        mutex->give();
    }
}

// Output data with the new-line appended.
int UMLRTLogProtocol_baserole::log ( const char * fmt, ... ) const
{
    int nchar = 0;

    if (ostream != NULL)
    {
        va_list ap;
        va_start(ap, fmt);
        UMLRTMutex * mutex = takeMutex();
        nchar = vfprintf(ostream, fmt, ap);
        nchar += fprintf(ostream, "\n");
        giveMutex(mutex);
        va_end(ap);
    }
    return nchar;
}

int UMLRTLogProtocol_baserole::log ( char c ) const
{
    int nchar = 0;

    if (ostream != NULL)
    {
        UMLRTMutex * mutex = takeMutex();
        nchar = fprintf(ostream, "%c\n", c);
        giveMutex(mutex);
    }
    return nchar;
}

int UMLRTLogProtocol_baserole::log ( short s ) const
{
    int nchar = 0;

    if (ostream != NULL)
    {
        UMLRTMutex * mutex = takeMutex();
        nchar = fprintf(ostream, "%d\n", s);
        giveMutex(mutex);
    }
    return nchar;
}

int UMLRTLogProtocol_baserole::log ( int i ) const
{
    int nchar = 0;

    if (ostream != NULL)
    {
        UMLRTMutex * mutex = takeMutex();
        nchar = fprintf(ostream, "%d\n", i);
        giveMutex(mutex);
    }
    return nchar;
}

int UMLRTLogProtocol_baserole::log ( long l ) const
{
    int nchar = 0;

    if (ostream != NULL)
    {
        UMLRTMutex * mutex = takeMutex();
        nchar = fprintf(ostream, "%ld\n", l);
        giveMutex(mutex);
    }
    return nchar;
}

int UMLRTLogProtocol_baserole::log ( long long ll ) const
{
    int nchar = 0;

    if (ostream != NULL)
    {
        UMLRTMutex * mutex = takeMutex();
        nchar = fprintf(ostream, "%lld\n", ll);
        giveMutex(mutex);
    }
    return nchar;
}

int UMLRTLogProtocol_baserole::log ( unsigned char uc ) const
{
    int nchar = 0;

    if (ostream != NULL)
    {
        UMLRTMutex * mutex = takeMutex();
        nchar = fprintf(ostream, "%c\n", uc);
        giveMutex(mutex);
    }
    return nchar;
}

int UMLRTLogProtocol_baserole::log ( unsigned short us ) const
{
    int nchar = 0;

    if (ostream != NULL)
    {
        UMLRTMutex * mutex = takeMutex();
        nchar = fprintf(ostream, "%u\n", us);
        giveMutex(mutex);
    }
    return nchar;
}

int UMLRTLogProtocol_baserole::log ( unsigned int ui ) const
{
    int nchar = 0;

    if (ostream != NULL)
    {
        UMLRTMutex * mutex = takeMutex();
        nchar = fprintf(ostream, "%u\n", ui);
        giveMutex(mutex);
    }
    return nchar;
}

int UMLRTLogProtocol_baserole::log ( unsigned long ul ) const
{
    int nchar = 0;

    if (ostream != NULL)
    {
        UMLRTMutex * mutex = takeMutex();
        nchar = fprintf(ostream, "%lu\n", ul);
        giveMutex(mutex);
    }
    return nchar;
}

int UMLRTLogProtocol_baserole::log ( unsigned long long ull ) const
{
    int nchar = 0;

    if (ostream != NULL)
    {
        UMLRTMutex * mutex = takeMutex();
        nchar = fprintf(ostream, "%llu\n", ull);
        giveMutex(mutex);
    }
    return nchar;
}

int UMLRTLogProtocol_baserole::log ( float f ) const
{
    int nchar = 0;

    if (ostream != NULL)
    {
        UMLRTMutex * mutex = takeMutex();
        nchar = fprintf(ostream, "%f\n", f);
        giveMutex(mutex);
    }
    return nchar;
}

int UMLRTLogProtocol_baserole::log ( double d ) const
{
    int nchar = 0;

    if (ostream != NULL)
    {
        UMLRTMutex * mutex = takeMutex();
        nchar = fprintf(ostream, "%f\n", d);
        giveMutex(mutex);
    }
    return nchar;
}

int UMLRTLogProtocol_baserole::log ( const void * userData, const UMLRTObject_class * type, int arraySize ) const
{
    int nchar;

    nchar = show(userData, type, arraySize);
    nchar += log("");

    return nchar;
}

int UMLRTLogProtocol_baserole::log ( const UMLRTTypedValue & value ) const
{
    return log( value.data, value.type );
}

// Output data with no new-line appended.
int UMLRTLogProtocol_baserole::show ( const char * fmt, ... ) const
{
    int nchar = 0;

    if (ostream != NULL)
    {
        va_list ap;
        va_start(ap, fmt);
        UMLRTMutex * mutex = takeMutex();
        nchar = vfprintf(ostream, fmt, ap);
        giveMutex(mutex);
        va_end(ap);
    }
    return nchar;
}

int UMLRTLogProtocol_baserole::show ( char c ) const
{
    int nchar = 0;

    if (ostream != NULL)
    {
        UMLRTMutex * mutex = takeMutex();
        nchar = fprintf(ostream, "%c", c);
        giveMutex(mutex);
    }
    return nchar;
}

int UMLRTLogProtocol_baserole::show ( short s ) const
{
    int nchar = 0;

    if (ostream != NULL)
    {
        UMLRTMutex * mutex = takeMutex();
        nchar = fprintf(ostream, "%d", s);
        giveMutex(mutex);
    }
    return nchar;
}

int UMLRTLogProtocol_baserole::show ( int i ) const
{
    int nchar = 0;

    if (ostream != NULL)
    {
        UMLRTMutex * mutex = takeMutex();
        nchar = fprintf(ostream, "%d", i);
        giveMutex(mutex);
    }
    return nchar;
}

int UMLRTLogProtocol_baserole::show ( long l ) const
{
    int nchar = 0;

    if (ostream != NULL)
    {
        UMLRTMutex * mutex = takeMutex();
        nchar = fprintf(ostream, "%ld", l);
        giveMutex(mutex);
    }
    return nchar;
}

int UMLRTLogProtocol_baserole::show ( long long ll ) const
{
    int nchar = 0;

    if (ostream != NULL)
    {
        UMLRTMutex * mutex = takeMutex();
        nchar = fprintf(ostream, "%lld", ll);
        giveMutex(mutex);
    }
    return nchar;
}

int UMLRTLogProtocol_baserole::show ( unsigned char uc ) const
{
    int nchar = 0;

    if (ostream != NULL)
    {
        UMLRTMutex * mutex = takeMutex();
        nchar = fprintf(ostream, "%c", uc);
        giveMutex(mutex);
    }
    return nchar;
}

int UMLRTLogProtocol_baserole::show ( unsigned short us ) const
{
    int nchar = 0;

    if (ostream != NULL)
    {
        UMLRTMutex * mutex = takeMutex();
        nchar = fprintf(ostream, "%u", us);
        giveMutex(mutex);
    }
    return nchar;
}

int UMLRTLogProtocol_baserole::show ( unsigned int ui ) const
{
    int nchar = 0;

    if (ostream != NULL)
    {
        UMLRTMutex * mutex = takeMutex();
        nchar = fprintf(ostream, "%u", ui);
        giveMutex(mutex);
    }
    return nchar;
}

int UMLRTLogProtocol_baserole::show ( unsigned long ul ) const
{
    int nchar = 0;

    if (ostream != NULL)
    {
        UMLRTMutex * mutex = takeMutex();
        nchar = fprintf(ostream, "%lu", ul);
        giveMutex(mutex);
    }
    return nchar;
}

int UMLRTLogProtocol_baserole::show ( unsigned long long ull ) const
{
    int nchar = 0;

    if (ostream != NULL)
    {
        UMLRTMutex * mutex = takeMutex();
        nchar = fprintf(ostream, "%llu", ull);
        giveMutex(mutex);
    }
    return nchar;
}

int UMLRTLogProtocol_baserole::show ( float f ) const
{
    int nchar = 0;

    if (ostream != NULL)
    {
        UMLRTMutex * mutex = takeMutex();
        nchar = fprintf(ostream, "%f", f);
        giveMutex(mutex);
    }
    return nchar;
}

int UMLRTLogProtocol_baserole::show ( double d ) const
{
    int nchar = 0;

    if (ostream != NULL)
    {
        UMLRTMutex * mutex = takeMutex();
        nchar = fprintf(ostream, "%f", d);
        giveMutex(mutex);
    }
    return nchar;
}

int UMLRTLogProtocol_baserole::show ( const void * userData, const UMLRTObject_class * type, int arraySize ) const
{
    int nchar = 0;

    if (ostream != NULL)
    {
        UMLRTMutex * mutex = takeMutex();
        nchar = UMLRTObject_fprintf(ostream, type, userData, 0/*nest*/, arraySize);
        giveMutex(mutex);
    }
    return nchar;
}

int UMLRTLogProtocol_baserole::show ( const UMLRTTypedValue & value ) const
{
    return show( value.data, value.type );
}

int UMLRTLogProtocol_baserole::cr ( int numCr ) const
{
    int nchar = 0;

    if (ostream != NULL)
    {
        UMLRTMutex * mutex = takeMutex();
        while (numCr > 0)
        {
            nchar += fprintf(ostream, "\n");
            --numCr;
        }
        giveMutex(mutex);
    }
    return nchar;
}

int UMLRTLogProtocol_baserole::crtab ( int numTab ) const
{
    int nchar = 0;

    if (ostream != NULL)
    {
        UMLRTMutex * mutex = takeMutex();
        nchar = fprintf(ostream, "\n");
        while (numTab > 0)
        {
            nchar += fprintf(ostream, "\t");
            --numTab;
        }
        giveMutex(mutex);
    }
    return nchar;
}

int UMLRTLogProtocol_baserole::space ( int numSpace ) const
{
    int nchar = 0;

    if (ostream != NULL)
    {
        UMLRTMutex * mutex = takeMutex();
        while (numSpace > 0)
        {
            nchar = fprintf(ostream, " ");
            --numSpace;
        }
        giveMutex(mutex);
    }
    return nchar;
}

int UMLRTLogProtocol_baserole::tab ( int numTab ) const
{
    int nchar = 0;

    if (ostream != NULL)
    {
        UMLRTMutex * mutex = takeMutex();
        while (numTab > 0)
        {
            nchar += fprintf(ostream, "\t");
            --numTab;
        }
        giveMutex(mutex);
    }
    return nchar;
}

int UMLRTLogProtocol_baserole::commit ( ) const
{
    if (ostream != NULL)
    {
        fflush(ostream);
    }
    return 0;
}

// Redirect logging output - closes previous output stream if it was opened via redirect(fname). Returns false if error.
bool UMLRTLogProtocol_baserole::redirect ( FILE * ostream_ ) const
{
    if (isOpenFile)
    {
        fclose(ostream);
        isOpenFile = false;
    }
    ostream = ostream_;
    return true;
}

// Redirect to a file by name - removes previous output stream(s). Returns false if error.
bool UMLRTLogProtocol_baserole::redirect ( const char * fname ) const
{
    bool ok = false;
    if (isOpenFile)
    {
        fclose(ostream);
        isOpenFile = false;
    }
    if (fname)
    {
        if ((ostream = fopen(fname, "w")) == NULL)
        {
            // Redirect to stderr in case file open fails.
            ostream = stderr;
        }
        else
        {
            isOpenFile = true;
            ok = true;
        }
    }
    return ok;
}

