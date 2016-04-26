@echo off

cd test_sources

%PATH_GNU_XPIC%\bin\xpic-as -gdwarf4 main.s -o main.o 
%PATH_GNU_XPIC%\bin\xpic-as -gdwarf4 caller.s -o caller.o 
%PATH_GNU_XPIC%\bin\xpic-ld -A xpic -o a.out -T linker.ld caller.o main.o
%PATH_GNU_XPIC%\bin\xpic-objdump -d a.out > objdump.txt
fc objdump.org objdump.txt > null
if %ERRORLEVEL%==0 (
echo test was successful.
) else (
echo test failed!
)
cd ..
