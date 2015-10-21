#*******************************************************************************#
# * Copyright (c) 2015 Zeligsoft (2009) Limited and others.                     #
# * All rights reserved. This program and the accompanying materials            #
# * are made available under the terms of the Eclipse Public License v1.0       #
# * which accompanies this distribution, and is available at                    #
# * http://www.eclipse.org/legal/epl-v10.html                                   #
#*******************************************************************************#

# platform independent include directories
CC_INCLUDES=\
   $(RTSROOT)/util/include \
   $(RTSROOT)/include \
   $(RTSROOT)/umlrt/src/include 
   
  # Objects that make up the RTS services library
CC_OBJS=\
	$(BUILDROOT)/$(CONFIG)/umlrt/src/umlrtgetopt$(OBJ_EXT) \
    $(BUILDROOT)/$(CONFIG)/umlrt/umlrtapi$(OBJ_EXT) \
    $(BUILDROOT)/$(CONFIG)/umlrt/umlrtbasicthread$(OBJ_EXT) \
    $(BUILDROOT)/$(CONFIG)/umlrt/umlrtcapsule$(OBJ_EXT) \
    $(BUILDROOT)/$(CONFIG)/umlrt/umlrtcapsuleid$(OBJ_EXT) \
    $(BUILDROOT)/$(CONFIG)/umlrt/umlrtcapsuletocontrollermap$(OBJ_EXT) \
    $(BUILDROOT)/$(CONFIG)/umlrt/umlrtcommsport$(OBJ_EXT) \
    $(BUILDROOT)/$(CONFIG)/umlrt/umlrtcontroller$(OBJ_EXT) \
    $(BUILDROOT)/$(CONFIG)/umlrt/umlrtframeprotocol$(OBJ_EXT) \
    $(BUILDROOT)/$(CONFIG)/umlrt/umlrtframeservice$(OBJ_EXT) \
    $(BUILDROOT)/$(CONFIG)/umlrt/umlrthashmap$(OBJ_EXT) \
    $(BUILDROOT)/$(CONFIG)/umlrt/umlrtinmessage$(OBJ_EXT) \
    $(BUILDROOT)/$(CONFIG)/umlrt/umlrtinsignal$(OBJ_EXT) \
    $(BUILDROOT)/$(CONFIG)/umlrt/umlrtlogprotocol$(OBJ_EXT) \
    $(BUILDROOT)/$(CONFIG)/umlrt/umlrtmain$(OBJ_EXT) \
    $(BUILDROOT)/$(CONFIG)/umlrt/umlrtmainloop$(OBJ_EXT) \
    $(BUILDROOT)/$(CONFIG)/umlrt/umlrtmaintargetshutdown$(OBJ_EXT) \
    $(BUILDROOT)/$(CONFIG)/umlrt/umlrtmaintargetstartup$(OBJ_EXT) \
    $(BUILDROOT)/$(CONFIG)/umlrt/umlrtmessage$(OBJ_EXT) \
    $(BUILDROOT)/$(CONFIG)/umlrt/umlrtmessagepool$(OBJ_EXT) \
    $(BUILDROOT)/$(CONFIG)/umlrt/umlrtmessagequeue$(OBJ_EXT) \
    $(BUILDROOT)/$(CONFIG)/umlrt/umlrtobjectclass$(OBJ_EXT) \
    $(BUILDROOT)/$(CONFIG)/umlrt/umlrtoutsignal$(OBJ_EXT) \
    $(BUILDROOT)/$(CONFIG)/umlrt/umlrtpool$(OBJ_EXT) \
    $(BUILDROOT)/$(CONFIG)/umlrt/umlrtprioritymessagequeue$(OBJ_EXT) \
    $(BUILDROOT)/$(CONFIG)/umlrt/umlrtprotocol$(OBJ_EXT) \
    $(BUILDROOT)/$(CONFIG)/umlrt/umlrtqueue$(OBJ_EXT) \
    $(BUILDROOT)/$(CONFIG)/umlrt/umlrtrtsinterfaceumlrt$(OBJ_EXT) \
    $(BUILDROOT)/$(CONFIG)/umlrt/umlrtsignal$(OBJ_EXT) \
    $(BUILDROOT)/$(CONFIG)/umlrt/umlrtsignalelement$(OBJ_EXT) \
    $(BUILDROOT)/$(CONFIG)/umlrt/umlrtsignalelementpool$(OBJ_EXT) \
    $(BUILDROOT)/$(CONFIG)/umlrt/umlrttimerid$(OBJ_EXT) \
    $(BUILDROOT)/$(CONFIG)/umlrt/umlrttimerpool$(OBJ_EXT) \
    $(BUILDROOT)/$(CONFIG)/umlrt/umlrttimerprotocol$(OBJ_EXT) \
    $(BUILDROOT)/$(CONFIG)/umlrt/umlrttimerqueue$(OBJ_EXT) \
    $(BUILDROOT)/$(CONFIG)/umlrt/umlrttimespec$(OBJ_EXT) 
    
