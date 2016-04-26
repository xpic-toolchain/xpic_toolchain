@echo off

cd test_sources

REM select dwarf-2
%PATH_GNU_XPIC%\bin\clang -O0 -gdwarf-2 --target=xpic -c -save-temps  main.c caller.c
%PATH_GNU_XPIC%\bin\xpic-as -gdwarf2 main.s -o main.o 
%PATH_GNU_XPIC%\bin\xpic-as -gdwarf2 caller.s -o caller.o 
%PATH_GNU_XPIC%\bin\xpic-ld -A xpic -o a.out -T linker.ld caller.o main.o
%PATH_GNU_XPIC%\bin\xpic-objdump -g -S -d a.out > objdump.txt


fc objdump.org objdump.txt > null
if %ERRORLEVEL%==0 (
echo test was successful.
) else (
echo test failed!
)
cd ..
