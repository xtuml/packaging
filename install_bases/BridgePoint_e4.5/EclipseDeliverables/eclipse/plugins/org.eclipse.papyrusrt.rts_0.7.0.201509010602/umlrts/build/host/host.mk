#*******************************************************************************#
# * Copyright (c) 2015 Zeligsoft (2009) Limited and others.                     #
# * All rights reserved. This program and the accompanying materials            #
# * are made available under the terms of the Eclipse Public License v1.0       #
# * which accompanies this distribution, and is available at                    #
# * http://www.eclipse.org/legal/epl-v10.html                                   #
#*******************************************************************************#

# GNU Makefile for determining the host

ifeq ($(OS),Windows_NT)

# win32 definitions.
SHELL = cmd.exe
MKDIR = mkdir
RMDIR = rmdir /q /s
NUL = nul
RM = del /f
CP = copy
MV = move
NOOP = cmd /C

else

# linux definitions.
MKDIR = mkdir -p
RMDIR = rm -rf
NUL = /dev/null
RM = rm -f
CP = cp
MV = mv
NOOP = $(SHELL) -c true

endif




