@echo off
cd test_sources
%PATH_GNU_XPIC%\bin\xpic-as -gdwarf2 test.s -o test.o 2> error.log

REM return Error as long as no error is reported by xpic-as
REM fc error.log error.log_ref > null
if %ERRORLEVEL%==0 (
%PATH_GNU_XPIC%\bin\xpic-objdump -D -S test.o > dump.tmp
echo test failed!
) else (
echo Error reported.
echo Verify error message
)
cd ..
