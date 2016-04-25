@echo off
cd test_sources
%PATH_GNU_XPIC%\bin\xpic-as -gdwarf2 test.s -o test_dwarf2.o 
%PATH_GNU_XPIC%\bin\xpic-as -gdwarf4 test.s -o test_dwarf4.o 
%PATH_GNU_XPIC%\bin\xpic-objdump --dwarf=decodedline test_dwarf2.o > objdump_dwarf2.tmp
%PATH_GNU_XPIC%\bin\xpic-objdump --dwarf=decodedline test_dwarf4.o > objdump_dwarf4.tmp

REM expect code is starting at address 0x0
REM at error case the code is started at address 0x4
fc objdump_dwarf2.tmp objdump_dwarf2.ref > null
if %ERRORLEVEL%==0 (

  fc objdump_dwarf4.tmp objdump_dwarf4.ref > null
  if %ERRORLEVEL%==0 (
  
    echo test was successful.
  ) else (
   echo test failed!
  )
) else (
  echo test failed!
)
cd ..
