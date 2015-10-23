#*******************************************************************************
# Copyright (c) 2014-2015 Zeligsoft (2009) Limited  and others.
# All rights reserved. This program and the accompanying materials
# are made available under the terms of the Eclipse Public License v1.0
# which accompanies this distribution, and is available at
# http://www.eclipse.org/legal/epl-v10.html
#*******************************************************************************

# Top level makefile for building RTS services library.
#
# By default, the RTS library is put into a 'lib' directory under the $RTSROOT
# directory. The user can specify an alternate location with LIBDEST=xxx.
# The LIBDEST directory is created if it doesn't exist.
#
# Build products are put into BUILDROOT (default is +/build). Created by build.
# 
# Targets:
#     (none)        : The default target is 'all' - same as 'library'.
#     all           : everything - currently only 'library'
#     library       : the RTS services library
#     clean         : Delete all build products.
#
# Variables should be specified during make and make clean
# invoke with <var>=<value> or with environment variables:
#     TARGETOS      : Target platform. (i.e. TARGETOS=linux)
#     BUILDTOOLS    : (i.e. BUILDTOOLS=x86-gcc-4.6.3)
#
# Variables (can be overridden during make invoke with <var>=<value>):
#     RTSROOT       : default RTSROOT=. : umlrt.rts repo dir.
#     BUILDROOT     : default BUILDROOT=$(RTSROOT)/build : build output dir.
#     CONFIG        : default CONFIG=$(TARGETOS).$(BUILDTOOLS)
#     LIBDEST       : Change default library destination directory.
#     DEPEND        : default DEPEND=1 : 'DEPEND=0' to skip dependency generation.
#
# Local variables (not typically passed in via env) are in lower-case.
#
# Examples:
#   make                   : Default - same as 'make all' and 'make library'
#   make all               : Default - same as 'make' and 'make library'
#   make library           : Default - same as 'make' and 'make all'
#   make DEPEND=0          : Build the library but skip building dependencies
#   make clean             : clean all build products (library, objects, dependency files.)
#   make LIBDEST=~/lib     : Put library into '~/lib'.
#   make BUILDROOT=~/build : Put build-products into '~/build'.
#
# After a clean pull from the repo, "make DEPEND=0" will build everything but
# will skip building dependencies (hence will be faster). (Dependencies are not
# required for this scenario to guarantee everything gets explicitly built.)
#
# RTSROOT and BUILDROOT notes
# ---------------------------
# RTSROOT names the top-level (umlrt.rts) directory. The default RTSROOT
# is current working directory (where this Makefile currently resides).
#
# BUILDROOT names the top-level build-products output directory tree.
# Object files and dependency files are written here.
# This directory tree is created by the build process if it doesn't yexist.
# The tree-structure under BUILDROOT matches the associated directory tree
# under RTSROOT.

# set default value for TARGETOS if is it not defined
ifeq ($(TARGETOS), )
$(warning warning: TARGETOS not defined. Choosing linux)
TARGETOS=linux
endif

# set default value for BUILDTOOLS if is it not defined
ifeq ($(BUILDTOOLS), )
$(warning warning: BUILDTOOLS not defined. Choosing x86-gcc-4.6.3)
BUILDTOOLS=x86-gcc-4.6.3
endif

# cwd is source root - current version assumes cwd is top-level src directory.
# This can be overridden while invoking make with RTSROOT=xxx.
RTSROOT=.

# Build output (objects) are placed in a directory tree
# This can be overridden while invoking make with BUILDROOT=xxx.
BUILDROOT=$(RTSROOT)/obj

# This can be overridden while invoking make with TARGETOS=xxx, BUILDTOOLS=yyy.
CONFIG=$(TARGETOS).$(BUILDTOOLS)

# Destination directory for the RTS services library.
LIBDEST=$(RTSROOT)/lib/$(CONFIG)

# Ouput RTS services library
library=$(LIBDEST)/$(LIB_PRFX)rts$(LIB_EXT)

-include $(RTSROOT)/build/host/host.mk

ifneq ($(MAKECMDGOALS),clean)
-include $(RTSROOT)/build/buildtools/$(BUILDTOOLS)/buildtools.mk
-include $(RTSROOT)/build/os/default.mk
-include $(RTSROOT)/build/os/$(TARGETOS)/os.mk
-include $(RTSROOT)/build/config/$(CONFIG)/config.mk

ifeq ($(DEPEND),1)
-include $(obj:$(OBJ_EXT)=$(DEP_EXT))
endif
endif

CC_DEFINES:=$(foreach d, $(CC_DEFINES), $(CC_DEF)$d) 
CC_INCLUDES:=$(foreach i, $(CC_INCLUDES), $(CC_INC)$i)

# Phony targets.
.PHONY : all clean library

# Build everything.
all : library

# phony target for the RTS services library (is currently the default target)
library : $(library)

# Clean everything.
clean :
	-@$(RMDIR) "$(RTSROOT)/lib/$(CONFIG)" 2> $(NUL)
	@echo $(RMDIR) "$(RTSROOT)/lib/$(CONFIG)"
	-@$(RMDIR) "$(RTSROOT)/obj/$(CONFIG)" 2> $(NUL)
	@echo $(RMDIR) "$(RTSROOT)/obj/$(CONFIG)"
	
# RTS services library build
$(library) : $(CC_OBJS)
	-@$(MKDIR) "$(LIBDEST)" 2> $(NUL)
	$(AR) $(AR_FLAGS) $(AR_OUT)$@ $^

#TODO replace -M? options with variables
# dependencies rule
$(BUILDROOT)/$(CONFIG)/%$(DEP_EXT) : $(RTSROOT)/%$(CC_EXT)
	-@$(MKDIR) "$(dir $@)" 2> $(NUL)
	-@$(CC) $(DEP_FLAGS) $^ $(CC_FLAGS) $(DEP_TARGET) $(DEP_FILE) $@

# objects rule
$(BUILDROOT)/$(CONFIG)/%$(OBJ_EXT) : $(RTSROOT)/%$(CC_EXT)
	-@$(MKDIR) "$(dir $@)" 2> $(NUL)
	$(CC) $< $(CC_FLAGS) $(CC_DEFINES) $(CC_INCLUDES) $(CC_OUT)$@

