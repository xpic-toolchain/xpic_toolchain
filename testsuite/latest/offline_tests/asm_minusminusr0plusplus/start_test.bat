@echo off
cd test_sources
%PATH_GNU_XPIC%\bin\xpic-as -gdwarf2 test.s -o test.o
%PATH_GNU_XPIC%\bin\xpic-objdump -D -S test.o > dump.tmp
fc dump.txt dump.tmp > null
if %ERRORLEVEL%==0 (
echo test failed!
) else (
echo test needs manual check!
echo check if assembler returns with error message
echo Error message has to mentioned wrong assembler opcode: "load r7, [r5 + --r6++]"
)
REM if exist test.o (
REM del test.o
REM )
REM if exist dump.tmp (
REM del dump.tmp
REM )
REM if exist null (
REM del null
REM )
cd ..
