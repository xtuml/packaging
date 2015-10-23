#*******************************************************************************#
# * Copyright (c) 2015 Zeligsoft (2009) Limited and others.                     #
# * All rights reserved. This program and the accompanying materials            #
# * are made available under the terms of the Eclipse Public License v1.0       #
# * which accompanies this distribution, and is available at                    #
# * http://www.eclipse.org/legal/epl-v10.html                                   #
#*******************************************************************************#

# os specific include directories
CC_INCLUDES+= \
   $(RTSROOT)/os/$(TARGETOS)/include
   
CC_OBJS+= \
   $(BUILDROOT)/$(CONFIG)/util/basedebug$(OBJ_EXT) \
   $(BUILDROOT)/$(CONFIG)/util/basefatal$(OBJ_EXT) \
   $(BUILDROOT)/$(CONFIG)/os/$(TARGETOS)/osbasicthread$(OBJ_EXT) \
   $(BUILDROOT)/$(CONFIG)/os/$(TARGETOS)/osmutex$(OBJ_EXT) \
   $(BUILDROOT)/$(CONFIG)/os/$(TARGETOS)/osnotify$(OBJ_EXT) \
   $(BUILDROOT)/$(CONFIG)/os/$(TARGETOS)/ossemaphore$(OBJ_EXT) \
   $(BUILDROOT)/$(CONFIG)/os/$(TARGETOS)/ostime$(OBJ_EXT) \
   $(BUILDROOT)/$(CONFIG)/os/$(TARGETOS)/ostimespec$(OBJ_EXT)


 
