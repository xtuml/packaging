// osnotify.hh

/*******************************************************************************
 * Copyright (c) 2015 Zeligsoft (2009) Limited  and others.
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * which accompanies this distribution, and is available at
 * http://www.eclipse.org/legal/epl-v10.html
 *******************************************************************************/

#ifndef OSNOTIFY_HH
#define OSNOTIFY_HH

class UMLRTNotify
{
public:

    UMLRTNotify();
    ~UMLRTNotify();

    // Calling this causes the notification mechanism to be 'set'. Threads waiting
    // for notification on this queue will receive notification.
    void sendNotification();

    // Return the notification file descriptor used to indicate a command is queued for execution.
    int getNotifyFd();

    // Clear notifications of pending commands.
    void clearNotifyFd();

private:

    // Notification-pipe. Those waiting for an element to be queued can wait on select() on the
    // 'getNotifyFd()' (the read-end of the pipe).
    // Threads enqueuing messages write a byte to the pipe to notify waiting threads.
    // Clearing the notifications involves reading until the pipe is empty.
    int notifyFd[2];

};
#endif // OSNOTIFY_HH
