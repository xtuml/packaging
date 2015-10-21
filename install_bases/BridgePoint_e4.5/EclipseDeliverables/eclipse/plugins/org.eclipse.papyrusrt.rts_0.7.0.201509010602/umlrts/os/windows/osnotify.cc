// umlrtnotify.cc

/*******************************************************************************
 * Copyright (c) 2015 Zeligsoft (2009) Limited  and others.
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * which accompanies this distribution, and is available at
 * http://www.eclipse.org/legal/epl-v10.html
 *******************************************************************************/

#include <winsock2.h>
#include <WS2tcpip.h>
#include "basedebug.hh"
#include "basedebugtype.hh"
#include "basefatal.hh"
#include "osnotify.hh"

#define BUFLEN 256  // Max length of buffer
#define PORT 0      // a random unoccupied port will be chosen

// See umlrtnotify.hh for documentation.
UMLRTNotify::UMLRTNotify()
{
    // Declare and initialize variables
    WSADATA wsaData =
    { 0 };
    int iResult = 0;

    // Initialize Winsock
    iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (iResult != 0)
    {
        FATAL("WSAStartup failed: %d\n", iResult);
    }

    m_socket = INVALID_SOCKET;
    m_socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (m_socket == INVALID_SOCKET)
    {
        FATAL("socket function failed with error = %d\n", WSAGetLastError());
    }

    // Bytes available on the socket wake up the controller as 'notify'. The controller
    // has to be able to clean out the socket buffer without blocking.
    u_long iMode = 1; // iMode != 0, non - blocking mode is enabled.
    iResult = ioctlsocket(m_socket, FIONBIO, &iMode);
    if (iResult != NO_ERROR)
    {
        FATAL("ioctlsocket failed with error: %ld\n", iResult);
    }

    //Prepare the sockaddr_in structure
    struct sockaddr_in saddr;
    saddr.sin_family = AF_INET;
    InetPton(AF_INET, "127.0.0.1", &(saddr.sin_addr));
    saddr.sin_port = htons(PORT);

    //Bind
    if (bind(m_socket, (struct sockaddr *) &saddr, sizeof(saddr)) == SOCKET_ERROR)
    {
        FATAL("Bind failed with error code : %d", WSAGetLastError());
    }
}

UMLRTNotify::~UMLRTNotify()
{
    int iResult = 0;

    iResult = closesocket(m_socket);
    if (iResult == SOCKET_ERROR)
    {
        BDEBUG(BD_CONTROLLER, "closesocket failed with error = %d\n", WSAGetLastError());
    }
    WSACleanup();
}

void UMLRTNotify::sendNotification()
{
    int iResult = 0;
    DWORD dwBytesWritten = 0;
    u_long bytes_ready;

    iResult = ioctlsocket(m_socket, FIONREAD, &bytes_ready);
    if (iResult != NO_ERROR)
    {
        FATAL("ioctlsocket failed with error: %ld\n", iResult);
    }
    if (!bytes_ready)
    {
        // Write a byte to the socket as a notification of a pending message.
        struct sockaddr_storage saddr;
        int saddrSize = sizeof(struct sockaddr_storage);
        if (getsockname(m_socket, (struct sockaddr*) &saddr, &saddrSize) == SOCKET_ERROR)
        {
            FATAL("getsockname() failed with error code : %d", WSAGetLastError());
        }

        char notifyByte = 0;
        //now reply the client with the same data
        if (sendto(m_socket, &notifyByte, 1, 0, (struct sockaddr*) &saddr, saddrSize)
                == SOCKET_ERROR)
        {
            FATAL("sendto() failed with error code : %d", WSAGetLastError());
        }
    }
}

void UMLRTNotify::clearNotifyFd()
{
    int iResult = 0;
    char recvBuf;
    u_long bytes_ready = 0;

    iResult = ioctlsocket(m_socket, FIONREAD, &bytes_ready);
    if (iResult != NO_ERROR)
    {
        FATAL("ioctlsocket failed with error: %ld\n", iResult);
    }
    // Indicates synchronization logic error. Should always get at least one byte.
    if (!bytes_ready)
    {
        FATAL_ERRNO("initial read - synchronization mechanism implies a byte is waiting");
    }

    // Clear out the notification-socket.
    bool done = false;
    while (!done)
    {
        iResult = recvfrom(m_socket, &recvBuf, 1, 0, NULL, NULL);
        if (iResult == SOCKET_ERROR)
        {
            int err = WSAGetLastError();
            if (err = WSAEWOULDBLOCK)
            {
                // WSAEWOULDBLOCK occurs when no more data on the socket
                done = true;
            }
            else
            {
                FATAL("recvfrom failed with error %d\n", err);
            }
        }
    }
}

// See umlrtnotify.cc for documentation.
int UMLRTNotify::getNotifyFd()
{
    return m_socket;
}

