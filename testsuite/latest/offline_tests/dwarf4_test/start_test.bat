@echo off

cd test_sources

REM select dwarf-4
%PATH_GNU_XPIC%\bin\clang -O0 -gdwarf-4 --target=xpic -c -save-temps  main.c caller.c
%PATH_GNU_XPIC%\bin\xpic-as -gdwarf4 main.s -o main.o 
%PATH_GNU_XPIC%\bin\xpic-as -gdwarf4 caller.s -o caller.o 
%PATH_GNU_XPIC%\bin\xpic-ld -A xpic -o a.out -T linker.ld caller.o main.o
%PATH_GNU_XPIC%\bin\xpic-objdump -g -S -d a.out > objdump.txt

REM commit correct objdump.org file after dwarf4 is working
echo test failed!

REM remove following comments if test case is working

REM fc objdump.org objdump.txt > null
REM if %ERRORLEVEL%==0 (
REM echo test was successful.
REM ) else (
REM echo test failed!
REM )
cd ..
