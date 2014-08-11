#
# File:     Makefile, 1.0
# Modified: 12/25/99, 16:47:31
#
# This file builds the libTRANS.dll with the following command:
#
#   make [HELP | ALL | TEST | CLEAN]
#
# All files must be in the current working directory.

CC = cl.exe
LD = link.exe
CCFLAGS = /nologo /MT /W3 /GX /O2 /D WIN32 /D NDEBUG /D _WINDOWS \
/D WINNT /FplibTRANS.pch /YX /Fo$*.o
LDFLAGS = kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib \
advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib \
odbccp32.lib /nologo /subsystem:windows /dll /incremental:no \
/pdb:libTRANS.pdb /machine:Ix86 /def:libTRANS.def \
/out:libTRANS.dll /implib:libTRANS.lib
LIB_DIR = .
LIB_TRANS = $(LIB_DIR)/libTRANS.lib
LIB_OBJECTS = pt_trans.o

HELP:
	@echo "==============================="
	@echo "make command                   "
	@echo "     lib  : make libTRANS.dll  "
	@echo "     clean: clean all          "
	@echo "==============================="

lib:	$(LIB_TRANS)

clean:
	rm -f $(LIB_OBJECTS)
	rm -f $(LIB_TRANS)
	rm -f vc50.idb
	rm -f libTRANS.dll
	rm -f libTRANS.exp
	rm -f libTRANS.pch

$(LIB_TRANS):	$(LIB_OBJECTS)
	$(LD) $(LDFLAGS) $(LIB_OBJECTS)

pt_trans.o:	 pt_trans.c
	$(CC) $(CCFLAGS) -c pt_trans.c
