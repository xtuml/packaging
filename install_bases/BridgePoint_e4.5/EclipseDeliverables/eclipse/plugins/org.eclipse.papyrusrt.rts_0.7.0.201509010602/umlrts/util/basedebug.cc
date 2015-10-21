// basedebug.cpp - Platform independent Base Debug (BD) run-time debugging.

/*******************************************************************************
 * Copyright (c) 2015 Zeligsoft (2009) Limited  and others.
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * which accompanies this distribution, and is available at
 * http://www.eclipse.org/legal/epl-v10.html
 *******************************************************************************/

// See basedebug.hh for interface details.
#include <stdarg.h>
#include <stdio.h>
#include "osutil.hh"
#include "ostime.hh"
#include "umlrtbasicthread.hh"
#include "umlrtsemaphore.hh"
#include "basedebug.hh"
#include "basefatal.hh"
#include "basedebugtype.hh"

namespace base
{
// Colour codes:
//static const char * ANSI_RESET = "\u001B[0m";
//static const char * ANSI_DIM_BLACK = "\u001B[2;30m";
//static const char * ANSI_DIM_RED = "\u001B[2;31m";
//static const char * ANSI_DIM_GREEN = "\u001B[2;32m";
//static const char * ANSI_DIM_YELLOW = "\u001B[2;33m";
//static const char * ANSI_DIM_BLUE = "\u001B[2;34m";
//static const char * ANSI_DIM_PURPLE = "\u001B[2;35m";
//static const char * ANSI_DIM_CYAN = "\u001B[2;36m";
//static const char * ANSI_BRIGHT_BLACK = "\u001B[1;30m";
//static const char * ANSI_BRIGHT_RED = "\u001B[1;31m";
//static const char * ANSI_BRIGHT_GREEN = "\u001B[1;32m";
//static const char * ANSI_BRIGHT_YELLOW = "\u001B[1;33m";
//static const char * ANSI_BRIGHT_BLUE = "\u001B[1;34m";
//static const char * ANSI_BRIGHT_PURPLE = "\u001B[1;35m";
//static const char * ANSI_BRIGHT_CYAN = "\u001B[1;36m";
//static const char * ANSI_WHITE = "\u001B[37m";

// To output BD type as string.
const char *debugMessageTypePrefix[BD_MAXPLUS1] = BD_PREFIX_STRING;

// ---- RUN-TIME ENABLE OF BD MESSAGES HERE ----
// Un-comment the lines that correspond to the types you want enabled.
long int debugEnableMessageType = 0
//                      | (1 << BD_BIND)
//                      | (1 << BD_BINDDEBUG)
//                      | (1 << BD_BINDFAIL)
//                      | (1 << BD_COMMAND)
//                      | (1 << BD_CONNECT)
//                      | (1 << BD_CONTROLLER)
//                      | (1 << BD_DESTROY)
//                      | (1 << BD_ERROR)
//                      | (1 << BD_IMPORT)
//                      | (1 << BD_INJECT)
//                      | (1 << BD_INSTANTIATE)
//                      | (1 << BD_LOCK)
//                      | (1 << BD_MODEL)
//                      | (1 << BD_MSG)
//                      | (1 << BD_MSGALLOC)
//                      | (1 << BD_SAP)
//                      | (1 << BD_SEND)
//                      | (1 << BD_SERIALIZE)
//                      | (1 << BD_SIGNAL)
//                      | (1 << BD_SIGNALDATA)
//                      | (1 << BD_SIGNALREF)
//                      | (1 << BD_SIGNALINIT)
//                      | (1 << BD_SIGNALALLOC)
//                      | (1 << BD_SWERR)
//                      | (1 << BD_TIMER)
//                      | (1 << BD_TIMERALLOC)
//                      | (1 << BD_MAXPLUS1)
;

// ---- RUN-TIME ENABLE OF BD MESSAGE COMPONENTS HERE ----
// Set to true to enable that part of the BD message output.
bool debugEnable = false; // Overall enable - if this is false, no debug output is produced regardless of all other settings.
bool debugEnableColor = false; // Enable color escape-sequences for output text.
bool debugEnableTypeName = true;
bool debugEnableTime = true;
bool debugEnableThreadId = false;
bool debugEnableFilename = false;
bool debugEnableLineNumber = false;
bool debugEnableMethod = false;
bool debugEnableUserMessage = true;

// Convenience class for serializing debug log output.
   class bdguard {
   public:
       bdguard( UMLRTSemaphore * * m );
       ~bdguard() { sem->post(); }

   private:
       UMLRTSemaphore * sem;
   };

   bdguard::bdguard( UMLRTSemaphore * * m )
   {
       if (!(*m))
       {
           (*m) = new UMLRTSemaphore(1);
       }
       (*m)->wait();
       sem = (*m);
   }

   UMLRTSemaphore * bdprintflock = (UMLRTSemaphore *) 0;

// BD debug printf - called from BDEBUG macros.
void debugPrintf( int bdtype, const char * file, int line, const char * method, const char * fmt, ...)
{
    static bool outputprefix = true;
    static osthreadid_t lastthread = 0;

    outputprefix |= (lastthread != UMLRTBasicThread::selfId());
    lastthread = UMLRTBasicThread::selfId();

    if (debugEnable && (bdtype < BD_MAXPLUS1) && (debugEnableMessageType & (1 << bdtype)))
    {
        va_list ap;
        va_start(ap, fmt);
        bdguard guard(&bdprintflock);
        debugColourBrightBlue();

        if (debugEnableTime && outputprefix)
        {
            struct timeval now;
            OSTime::get_timeofday(&now);
            struct tm from_localtime;
            const time_t seconds = now.tv_sec;
            localtime_r( &seconds, &from_localtime );
            char tmbuf[64];
            size_t length = strftime(tmbuf, sizeof(tmbuf), "%Y-%m-%d:%H:%M:%S", &from_localtime);
            snprintf(&tmbuf[length], sizeof(tmbuf)-length, ".%03ld", now.tv_usec/1000);
            if (length <= 0)
            {
                strcpy(tmbuf, "?");
            }
            printf("%s:", tmbuf);
        }
        if (debugEnableTypeName && outputprefix)
        {
            printf("%s:", debugMessageTypePrefix[bdtype]);
        }
        if (debugEnableThreadId && outputprefix)
        {
            printf("%ld:", (long int) UMLRTBasicThread::selfId());
        }
        if (debugEnableFilename && outputprefix)
        {
            printf("%s:", file);
        }
        if (debugEnableLineNumber && outputprefix)
        {
            printf("line %d:", line);
        }
        if (debugEnableMethod && outputprefix)
        {
            printf("%s:", method);
        }
        if (debugEnableUserMessage)
        {
            vprintf(fmt, ap);
        }
        size_t len;
        if ((len = strlen(fmt)) > 0)
        {
            outputprefix = (fmt[len-1] == '\n');
        }
        va_end(ap);
        debugColourReset();
        fflush(stdout);

    }
}

// See UMLRTMain.hh
long int debugGetEnabledTypes()
{
    return debugEnableMessageType;
}

// See UMLRTMain.hh
void debugEnableSet(bool enabled)
{
    debugEnable = enabled;
}

// See UMLRTMain.hh
void debugEnableColorSet(bool enabled)
{
    // color not platform independent
    debugEnableColor = false;
}

// See UMLRTMain.hh
bool debugEnableTypeMaskNameSet(const char * name, bool enable)
{
    long int index = -1;
    bool ok = true;

    // Special name 'all'
    if (!strcasecmp(name, "ALL"))
    {
        for (int i = 1; (i < BD_MAXPLUS1) && ok; ++i)
        {
            ok = debugEnableTypeMaskNameSet(debugMessageTypePrefix[i], enable);
        }
    }
    else
    {
        for (int i = 0; (i < BD_MAXPLUS1) && (index == -1); ++i)
        {
            if (!strcasecmp(name, debugMessageTypePrefix[i]))
            {
                index = i;
            }
        }
        if (index == -1)
        {
            printf("ERROR: debug-type '%s' not found.\n", name);
            ok = false;
        }
        else if (enable)
        {
            debugEnableMessageType |= (1 << index);
        }
        else
        {
            debugEnableMessageType &= ~(1 << index);
        }
    }
    return ok;
}

// See UMLRTMain.hh
bool debugEnableTypeMaskSpecSet(char * typeMaskSpec, bool enable)
{
    bool ok = true;
    bool oneset = false;
    char * next;
    char * str = typeMaskSpec;
    while (ok)
    {
        // str non-NULL the first time, get the first token.
        char * token = strtok_r(str, " ", &next);
        // str set NULL, to use 'next' for subsequent calls.
        str = NULL;
        if (token == NULL)
        {
            break;
        }
        oneset = true;
        // Got a debug-type name - enable or disable it.
        ok = debugEnableTypeMaskNameSet(token, enable);
    }
    if (ok && oneset)
    {
        debugEnableSet(true);
    }
    return ok;
}

// See UMLRTMain.hh
void debugEnableTypeNameDisplaySet(bool enabled)
{
    debugEnableTypeName = enabled;
    if (enabled)
    {
        debugEnableSet(true);
    }
}

// See UMLRTMain.hh
void debugEnableTimeDisplaySet(bool enabled)
{
    debugEnableTime = enabled;
    if (enabled)
    {
        debugEnableSet(true);
    }
}

// See UMLRTMain.hh
void debugEnableThreadDisplaySet(bool enabled)
{
    debugEnableThreadId = enabled;
    if (enabled)
    {
        debugEnableSet(true);
    }
}

// See UMLRTMain.hh
void debugEnableFilenameDisplaySet(bool enabled)
{
    debugEnableFilename = enabled;
    if (enabled)
    {
        debugEnableSet(true);
    }
}

// See UMLRTMain.hh
void debugEnableLineNumberDisplaySet(bool enabled)
{
    debugEnableLineNumber = enabled;
    if (enabled)
    {
        debugEnableSet(true);
    }
}

// See UMLRTMain.hh
void debugEnableMethodDisplaySet(bool enabled)
{
    debugEnableMethod = enabled;
    if (enabled)
    {
        debugEnableSet(true);
    }
}

// See UMLRTMain.hh
void debugEnableUserMsgDisplaySet(bool enabled)
{
    debugEnableUserMessage = enabled;
    if (enabled)
    {
        debugEnableSet(true);
    }
}

// See UMLRTMain.hh
bool debugTypeEnabled(long int type)
{
    return ((debugEnableMessageType & (1 << type)) != 0);
}

// See UMLRTMain.hh
void debugLogData(long int type, uint8_t * data, size_t size)
{
    if (base::debugTypeEnabled(type) && (size != 0))
    {
        int extra = size & 0x7;

        for (size_t i = 0; i < ((size / 8) + (extra != 0 ? 1 : 0)); ++i)
        {
            char buf[(8 * 6) + 2];
            buf[0] = '\0';
            char * p = buf;
            int count = 0;
            for (size_t j = 0; (j < 8) && (((i * 8) + j) < size); ++j, ++count)
            {
                p += snprintf(p, sizeof(buf) - (p - buf), "0x%02X ", data[(i * 8) + j]);
            }
            for (size_t j = count; j < 8; ++j)
            {
                p += snprintf(p, sizeof(buf) - (p - buf), "     ");
            }
            *(p++) = ' ';
            for (size_t j = 0; (j < 8) && ((j + (i * 8)) < size); ++j)
            {
                char c = data[(i * 8) + j];
                if ((c >= 0x20) && (c <= 0x7F))
                {
                    *(p++) = c;
                }
                else
                {
                    *(p++) = '.';
                }
            }
            *p = '\0';
            BDEBUG(type, "%s\n", buf);
        }
    }
}

// See UMLRTMain.hh
void debugOptionSummary()
{
    debugColourBrightBlue();

    printf("Overall debug log enable (option 'debug/D')  : %s\n",
            debugEnable ? "enabled" : "disabled");
    printf("Debug message types (option 'debugtype/T')\n");
    for (int i = 1; i < BD_MAXPLUS1; ++i)
    {
        printf("    %13s : %s\n", debugMessageTypePrefix[i],
                ((debugEnableMessageType & (1 << i)) != 0) ? "enabled" : "disabled");
    }
    printf("Debug log message components displayed:\n");
    printf("      time-stamp (option 'debugtime/S')      : %s\n",
            debugEnableTime ? "displayed" : "suppressed");
    printf("       type name (option 'debugname/N')      : %s\n",
            debugEnableTypeName ? "displayed" : "suppressed");
    printf("       thread-id (option 'debugthread/n')    : %s\n",
            debugEnableThreadId ? "displayed" : "suppressed");
    printf("       file-name (option 'debugfile/F')      : %s\n",
            debugEnableFilename ? "displayed" : "suppressed");
    printf("     line-number (option 'debugline/L')      : %s\n",
            debugEnableLineNumber ? "displayed" : "suppressed");
    printf("     method name (option 'debugmethod/M')    : %s\n",
            debugEnableMethod ? "displayed" : "suppressed");
    printf("    user message (option 'debugmsg/m')       : %s\n",
            debugEnableUserMessage ? "displayed" : "suppressed");

    debugColourReset();
}

// See UMLRTMain.hh
void debugTypeSummary()
{
    for (int i = 1; i < BD_MAXPLUS1; ++i)
    {
        printf("%s ", debugMessageTypePrefix[i]);
    }
}

// See basedebug.hh
void debugColourReset()
{
    if (debugEnableColor)
    {
        // printf("%s", ANSI_RESET);
    }
}

void debugColourBrightBlue()
{
    if (debugEnableColor)
    {
        // printf("%s", ANSI_BRIGHT_BLUE);
    }
}
void debugColourBrightRed()
{
    if (debugEnableColor)
    {
        // printf("%s", ANSI_BRIGHT_RED);
    }
}
void debugColourDimBlue()
{
    if (debugEnableColor)
    {
        // printf("%s", ANSI_DIM_BLUE);
    }
}

// See basedebug.hh for documentation.
void swerr(const char * file, int line, const char * method, const char * fmt, ...)
{
    if (debugEnable && (debugEnableMessageType & (1 << BD_SWERR)))
    {
        va_list ap;
        va_start(ap, fmt);
        bdguard guard(&bdprintflock);
        debugColourBrightRed();
        printf("SWERR:%s:%d:%s:%ld:", file, line, method, (long int) UMLRTBasicThread::selfId());
        vprintf(fmt, ap);
        printf("\n");
        va_end(ap);
        debugColourReset();
    }
}
// See basedebug.hh for documentation.
void swerrErrno(const char * file, int line, const char * method, int errno_, const char * api)
{
    if (debugEnable && (debugEnableMessageType & (1 << BD_SWERR)))
    {
        bdguard guard(&bdprintflock);
        debugColourBrightRed();
        printf("SWERR:%s:%d:%s:%ld:%s:errno(%d):%s\n", file, line, method, (long int) UMLRTBasicThread::selfId(), api, errno_, strerror(errno_));
        debugColourReset();
    }
}
}
