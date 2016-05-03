@echo off
cd test_sources
%PATH_GNU_XPIC%\bin\xpic-as -gdwarf2 test.s -o test.o 2> error.log

REM return Error as long as wrong error is reported by xpic-as
fc error.log error.log_ref > NULL
if %ERRORLEVEL%==0 (
echo test was succeesful.

) else (
echo test failed!
)
cd ..
