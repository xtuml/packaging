// umlrtbasicthread.hh

/*******************************************************************************
* Copyright (c) 2014-2015 Zeligsoft (2009) Limited  and others.
* All rights reserved. This program and the accompanying materials
* are made available under the terms of the Eclipse Public License v1.0
* which accompanies this distribution, and is available at
* http://www.eclipse.org/legal/epl-v10.html
*******************************************************************************/

#ifndef UMLRTBASICTHREAD_HH
#define UMLRTBASICTHREAD_HH

typedef void * osthreadid_t;

class UMLRTBasicThread
{
    // Basic thread-class.

    // Sub-class this to implement a basic thread.
    //
    // - 'start()' to start the thread and pass it an argument.
    // - 'run()' the main entry-point - implemented in sub-class.
    // - 'join()' to wait for thread to exit and get return value.
    // - 'isMyThread()' returns true if this is currently running thread.
    // - 'getName()' to get the thread's name (defined at instantiation).

private:
    osthreadid_t tid;

    // Thread name - TODO: remove magic #.
    char name[80];

    // Used to pass sub-class instance and start argument
    // to main entry point (sub-class method 'run').
    typedef struct
    {
        UMLRTBasicThread * inst;
        void * args;

    } threadargs_t;

    threadargs_t threadargs;

    // Static entry point for pthread. Needs both sub-class
    // instance and argument passed to 'start()'.

    static void * static_entrypoint( void * arg );

public:

    UMLRTBasicThread( const char * name_ );

    // Main entry point - must be implemented in sub-class.

    virtual void * run( void * args ) = 0;

    // Start the thread, passing in a single argument.

    void start( void * args );

    // Wait for the thread to complete and get returned value.

    void * join();

    // Returns true if this thread is currently running thread.

    bool isMyThread();

    // Return my name

    const char * getName() const;

    // Return running thread id.
    static osthreadid_t selfId();
};

#endif // UMLRTBASICTHREAD_HH
