@echo off
cd test_sources
%PATH_GNU_XPIC%\bin\xpic-as -gdwarf2 test.s -o test.o 2> dump.tmp
fc dump.txt dump.tmp > null
if %ERRORLEVEL%==0 (
echo test was succeesful.
) else (
echo test failed!
)
if exist test.o (
del test.o
)
if exist dump.tmp (
del dump.tmp
)
if exist null (
del null
)
cd ..
