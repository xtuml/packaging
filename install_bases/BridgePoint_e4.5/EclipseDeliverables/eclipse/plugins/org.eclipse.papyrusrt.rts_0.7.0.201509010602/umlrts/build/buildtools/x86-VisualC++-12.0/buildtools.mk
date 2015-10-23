#*******************************************************************************#
# * Copyright (c) 2015 Zeligsoft (2009) Limited and others.                     #
# * All rights reserved. This program and the accompanying materials            #
# * are made available under the terms of the Eclipse Public License v1.0       #
# * which accompanies this distribution, and is available at                    #
# * http://www.eclipse.org/legal/epl-v10.html                                   #
#*******************************************************************************#

# tools
# Compiler
CC=cl.exe
# Archiver
AR=lib.exe
# Linker
LD=link.exe

# compiler options
CC_DEF				= -D
CC_DEP				= -FD
CC_UNDEF			= -U
CC_INC				= -I
CC_OUT				= -Fo
CC_DEBUG			= -Zi
CC_DEFINES			= _CRT_SECURE_NO_WARNINGS
CC_FLAGS			= -c -EHsc -nologo -W3 -wd4101 -wd4996
CC_PPFLAGS			= 
CC_INCLUDES			= 

# dependencies
DEP_FLAGS			= 
DEP_TARGET			= 
DEP_FILE			= 

# linker options
LD_LIB				= 
LD_LIBPATH			= -libpath:
LD_OUT				= -out:
LD_FLAGS			= -nologo
LD_PATHS	=
LD_LIBS				= rts.lib Ws2_32.lib

LD_OBJS				= 
LD_PRE_PROCESS		=
LD_POST_PROCESS		= 

# archiver options
AR_FLAGS			= -nologo
AR_OUT				= -out:
AR_PRE_PROCESS		=
AR_POST_PROCESS		=

# extensions
CC_EXT		= .cc
OBJ_EXT		= .obj
EXE_EXT		= .exe
LIB_EXT		= .lib 
SHLIB_EXT	= .dll
LIB_PRFX	=
DEP_EXT		= .d
DBG_FILES	= *.idb *.pdb *.ilk

# Can optionally skip building dependencies (default is 'build dependencies')
# This can be overridden while invoking make with DEPEND=0
DEPEND=0

# This can be overridden while invoking make with DEBUG=1
DEBUG=0
# Add debugging compile flags
ifeq ($(DEBUG),1)
CC_FLAGS+= -Ob0 -Zi
LD_FLAGS+= -DEBUG
else
# optimizations
CC_FLAGS+= -Ob1 -Ot
endif



