@echo off
cd test_sources
%PATH_GNU_XPIC%\bin\xpic-as -gdwarf2 test.s -o test.o 2> error.log

REM return Error as long as no error is reported by xpic-as
fc error.log error.log_ref > null
if %ERRORLEVEL%==0 (
echo test was succeesful.

) else (
echo test FAILED.
)
cd ..
