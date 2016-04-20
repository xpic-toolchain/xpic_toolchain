@echo off
cd test_sources

REM test case with option -gdwarf2
%PATH_GNU_XPIC%\bin\xpic-as -gdwarf2 test.s -o test_gdwarf2.o 2> dump.tmp
fc dump.txt dump.tmp > null
if %ERRORLEVEL%==0 (
echo test with option -gdwarf2 was succeesful.
REM clean directory if test case was successful
if exist test_gdwarf2.o (
del test_gdwarf2.o
)
if exist dump.tmp (
del dump.tmp
)
if exist null (
del null
)

) else (
echo test test_gdwarf2 failed!
)

REM repeat test case with option -gdwarf4
%PATH_GNU_XPIC%\bin\xpic-as -gdwarf4 test.s -o test_gdwarf4.o 2> dump.tmp
fc dump.txt dump.tmp > null
if %ERRORLEVEL%==0 (
echo test with option -gdwarf4 was succeesful.
REM clean directory if test case was successful
if exist test_gdwarf4.o (
del test_gdwarf4.o
)
if exist dump.tmp (
del dump.tmp
)
if exist null (
del null
)

) else (
echo test test_gdwarf4 failed!
)

cd ..
