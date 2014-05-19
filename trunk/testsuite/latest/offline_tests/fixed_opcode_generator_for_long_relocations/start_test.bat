@echo off
cd test_sources
%PATH_GNU_XPIC%\bin\xpic-llvmc -O0 -Wa,-mmcu=xpic2 -T linker.ld main.s caller.s
%PATH_GNU_XPIC%\bin\xpic-objdump -S a.out > objdump.txt
fc objdump.org objdump.txt > null
if %ERRORLEVEL%==0 (
echo test was successful.
) else (
echo test failed!
)
if exist a.out (
del a.out
)
if exist objdump.txt (
del objdump.txt
)
if exist null (
del null
)
cd ..