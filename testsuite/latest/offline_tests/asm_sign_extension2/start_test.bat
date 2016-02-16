@echo off
cd test_sources
%PATH_GNU_XPIC%\bin\xpic-as -gdwarf2 test.s -o test.o 2> error.log
fc error.log error.log_ref > null
if %ERRORLEVEL%==0 (
echo test was succeesful.
) else (
echo error message changed
echo test failed!
)
cd ..
