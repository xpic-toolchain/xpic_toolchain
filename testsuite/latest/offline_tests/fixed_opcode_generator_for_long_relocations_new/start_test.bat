@echo off

cd test_sources


%PATH_GNU_XPIC%\bin\clang -O0 --target=xpic -c -save-temps  main.c caller.c
%PATH_GNU_XPIC%\bin\xpic-as -gdwarf4 main.s -o main.o 
%PATH_GNU_XPIC%\bin\xpic-as -gdwarf4 caller.s -o caller.o 
%PATH_GNU_XPIC%\bin\xpic-ld -A xpic -o a.out -T linker.ld caller.o main.o
%PATH_GNU_XPIC%\bin\xpic-objdump -d a.out > objdump.txt


REM /home/mbober/xpic_toolchain/install/local/bin/clang -Wall -S -emit-llvm -O0 main.c
REM /home/mbober/xpic_toolchain/install/local/bin/llc -march=xpic -O0 -asm-verbose main.ll 


fc objdump.org objdump.txt > null
if %ERRORLEVEL%==0 (
echo test was successful.
) else (
echo test failed!
)
REM if exist a.out (
REM del a.out
REM )
REM if exist objdump.txt (
REM del objdump.txt
REM )
REM if exist null (
REM del null
REM )
cd ..
