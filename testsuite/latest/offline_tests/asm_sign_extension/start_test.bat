@echo off
cd test_sources
%PATH_GNU_XPIC%\bin\xpic-as -gdwarf2 test.s -o test.o
%PATH_GNU_XPIC%\bin\xpic-objdump -d -S test.o > dump.tmp
fc dump.txt dump.tmp > null
if %ERRORLEVEL%==0 (
echo test was succeesful.
) else (
echo test failed!
)
cd ..
