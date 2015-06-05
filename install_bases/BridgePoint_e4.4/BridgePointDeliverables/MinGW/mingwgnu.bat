@ECHO OFF
ECHO setting up environment for mingw32 GNU tools

SET INSTALLDIR=%dp0\..

REM set this directory to match your machine...
set GNUROOT=%INSTALLDIR%\MinGW

REM you shouldn't have to change any of these... (except for maybe c:\bin which is assumed to contain make)
set PATH=%GNUROOT%\bin;%GNUROOT%\lib\gcc\mingw32\3.4.5;%GNUROOT%\mingw32\bin;c:\bin;%INSTALLDIR%\tools;%PATH%
REM BridgePoint - The following line is commented out by default in this batch
REM SET GCC_EXEC_PREFIX=%GNUROOT%\lib\gcc\mingw32\3.4.5
REM BridgePoint - The following includes are not commented out by default, but are commented out
REM  by us.  When they are left in place we get duplicate path problems decorated on new projects
REM  by CDT.  With them commented out, the problems don't show and the code compilation still works fine.
REM SET C_INCLUDE_PATH=%GNUROOT%\include\
REM SET CPLUS_INCLUDE_PATH=%GNUROOT%\include\c++\3.4.5

set DIRCMD=/P /O:GEN
@echo on
