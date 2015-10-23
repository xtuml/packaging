// umlrtmain.hh

/*******************************************************************************
* Copyright (c) 2014-2015 Zeligsoft (2009) Limited  and others.
* All rights reserved. This program and the accompanying materials
* are made available under the terms of the Eclipse Public License v1.0
* which accompanies this distribution, and is available at
* http://www.eclipse.org/legal/epl-v10.html
*******************************************************************************/

#ifndef UMLRTMAIN_HH
#define UMLRTMAIN_HH

class UMLRTMain
{
public:

    // setArgs: Pass in initial command-line arguments.

    // Arguments (command-line options) are either intended for the underlying RTS library or
    // for the application code.

    // The user must specify the option "--userargs" before those arguments intended for the
    // application. All preceding options are assumed to be intended for the RTS library.

    // The getArgCount() and getArg() methods refer only to those arguments which appear after
    // "--userargs". The application code has no way to retrieve RTS-library options after
    // the RTS is started.

    static void setArgs( int argc, char * const * argv );

    // Get the number of user command-line arguments.
    static int getArgCount();

    // Get the i-th command-line argument.
    static const char * getArg( int index );

    // Target startup and target shutdown and the 'mainLoop'.
    //
    // Default targetStartup, targetShutdown and mainLoop methods are provided by this class.
    // These all appear in separate compilation units and can be overridden by the application
    // by linking in their own versions of these methods.

    // targetStartup:
    // This method can be overridden by the application code to initialize their run-time platform.
    // The method must return 'true' if the application is to continue running. If it returns 'false',
    // the application is terminated with a failure indication.
    static bool targetStartup();

    // mainLoop:
    // This method can be overridden by the application code to execute code a 'main thread' that
    // runs in parallel with the set of controllers. The main thread need not block or wait and
    // can return immediately. By default, after the 'mainLoop()' returns, the main thread
    // waits for the controllers to run-to-completion (terminate).
    //
    // The method should return a Boolean value indicating whether the main loop should wait for
    // the controllers to terminate normally. If 'false' is returned, we don't wait for the controllers
    // to run-to-completion but instead we call targetShutdown() immediately.
    static bool mainLoop();

    // targetShutdown:
    // This method can be overridden by the application to shut-down the execution platform.
    // The method takes a single Boolean parameter value returned by 'mainLoop()' and should
    // itself return the exit-status of the overall application.
    static int targetShutdown( bool mainLoopReturnValue );

private:

    // Print usage in response to -h or --help or invalid option.
    static void usage( const char * const program );

    // Number of user arguments.
    static int argc;

    // List of user arguments.
    static const char * * argv;

    // True when arguments are defined - false to detect attempt to fetch undefined arguments.
    static bool argsDefined;
};

#endif // UMLRTMAIN_HH
