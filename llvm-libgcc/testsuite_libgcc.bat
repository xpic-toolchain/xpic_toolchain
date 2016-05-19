@echo OFF
echo Starting at:
date /T
time /T
echo using xPIC compiler from directory: %PATH_GNU_XPIC%
%PATH_GNU_XPIC%\bin\llc --version

c:\Python27\python.exe "..\scons-local\scons.py" check gdb_connection=usb netx=netx51 target=%1 test=%2

echo finishing at
date /T
time /T
