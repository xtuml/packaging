// osprioritymessagequeue.cc

/*******************************************************************************
 * Copyright (c) 2015 Zeligsoft (2009) Limited  and others.
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * which accompanies this distribution, and is available at
 * http://www.eclipse.org/legal/epl-v10.html
 *******************************************************************************/

#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include "basedebug.hh"
#include "basedebugtype.hh"
#include "basefatal.hh"
#include "osnotify.hh"

/* Constants 0 and 1 for READ and WRITE */
enum PIPES
{
    READ, WRITE
};

// See umlrtnotify.hh for documentation.
UMLRTNotify::UMLRTNotify()
{
    // Appenders write the pipe. Controllers 'wait' on a select() on the pipe with
    // a timeout associated with any running timer. A write on the pipe wakes the controller.
    if (pipe(notifyFd) < 0)
    {
        FATAL_ERRNO("pipe");
    }
    // Bytes available in the pipe wake up the controller as 'notify'. The controller
    // has to be able to clean out the pipe (read) without blocking.
    int flags = fcntl(notifyFd[READ], F_GETFL, 0);
    if (flags < 0)
    {
        FATAL_ERRNO("fcntl F_GETFL");
    }
    if (fcntl(notifyFd[READ], F_SETFL, flags | O_NONBLOCK) < 0)
    {
        FATAL_ERRNO("fcntl F_SETFL");
    }
}

UMLRTNotify::~UMLRTNotify()
{
    close(notifyFd[READ]);
    close(notifyFd[WRITE]);
}

void UMLRTNotify::sendNotification()
{
    int bytes_ready;

    if (ioctl(notifyFd[READ], FIONREAD, &bytes_ready) < 0)
    {
        FATAL_ERRNO("ioctl");
    }
    if (!bytes_ready)
    {
        // Write a byte to the notification-pipe as a notification of a pending message.
        uint8_t notifyByte = 0;

        if (write(notifyFd[WRITE], &notifyByte, 1) < 0)
        {
            FATAL_ERRNO("write");
        }
    }
}

void UMLRTNotify::clearNotifyFd()
{
    uint8_t ignore;
    int bytes;
    if ((bytes = read(notifyFd[READ], &ignore, 1)) < 0)
    {
        FATAL_ERRNO("initial read - synchronization mechanism implies a byte is waiting");
    }
    else if (!bytes)
    {
        // Indicates synchronization logic error. Should always get at least one byte.
        FATAL("Should never find pipe empty when clearing a notification.");
    }
    // Clear out the notification-pipe.
    // Can shrink this after notify is debugged.
    bool done = false;
    while (!done)
    {
        int bytes = read(notifyFd[READ], &ignore, 1);
        if (bytes < 0)
        {
            if (errno != EAGAIN)
            {
                FATAL_ERRNO("read");
            }
            else
            {
                done = true;
            }
        }
        if (bytes > 0)
        {
            // BDEBUG(0, "read returned bytes(%d)\n", bytes);
        }
        else
        {
            done = true;
        }
    }
}

// See umlrtnotify.cc for documentation.
int UMLRTNotify::getNotifyFd()
{
    return notifyFd[0];
}


