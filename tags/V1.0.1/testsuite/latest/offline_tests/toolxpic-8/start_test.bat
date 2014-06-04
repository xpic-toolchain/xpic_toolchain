@echo off
cd test_sources
%PATH_GNU_XPIC%\bin\xpic-llvmc -O0 -Wa,-mmcu=xpic2 main.c
if %ERRORLEVEL%==0 (
echo test was successful.
) else (
echo test failed!
)
if exist a.out (
del a.out
)
cd ..
