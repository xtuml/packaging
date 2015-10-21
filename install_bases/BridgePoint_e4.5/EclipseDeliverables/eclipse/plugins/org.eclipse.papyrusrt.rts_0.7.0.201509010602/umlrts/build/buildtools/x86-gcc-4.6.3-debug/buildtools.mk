#*******************************************************************************#
# * Copyright (c) 2015 Zeligsoft (2009) Limited and others.                     #
# * All rights reserved. This program and the accompanying materials            #
# * are made available under the terms of the Eclipse Public License v1.0       #
# * which accompanies this distribution, and is available at                    #
# * http://www.eclipse.org/legal/epl-v10.html                                   #
#*******************************************************************************#

# tools
# Compiler
CC=g++
# Archiver
AR=ar
# Linker
LD=g++

# compiler options
CC_DEF 	 			= -D
CC_UNDEF 			= -U
CC_INC 				= -I
CC_OUT 	 			= -o
CC_DEBUG 			= -g -O0
CC_DEFINES 			= 
CC_FLAGS			= -c -Wall
CC_PPFLAGS			= 
CC_INCLUDES			= 

# dependencies
DEP_FLAGS			= -MP -MM
DEP_TARGET			= -MT $(subst .d,.o,$@)
DEP_FILE			= -MF

# linker options
LD_LIB 				= -l
LD_LIBPATH 			= -L
LD_OUT 				= -o
LD_FLAGS 			= 
LD_PATHS 			=
LD_LIBS				= rts pthread
LD_OBJS 			= 
LD_PRE_PROCESS 		=
LD_POST_PROCESS 	= 

# archiver options
AR_FLAGS 			= -rs
AR_OUT				=
AR_PRE_PROCESS 		=
AR_POST_PROCESS 	=

# extensions
CC_EXT		= .cc
OBJ_EXT 	= .o
EXE_EXT 	=
LIB_EXT 	= .a 
SHLIB_EXT 	= .so
LIB_PRFX	= lib
DEP_EXT 	= .d
DBG_FILES	= 

# Add debugging compile flags
CC_FLAGS+= $(CC_DEBUG)

# Can optionally skip building dependencies (default is 'build dependencies')
# This can be overridden while invoking make with DEPEND=0
DEPEND=1

