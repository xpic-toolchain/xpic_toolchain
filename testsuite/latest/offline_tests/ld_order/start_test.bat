@echo off
set i=0
cd test_sources
%PATH_GNU_XPIC%\bin\clang -O0 -gdwarf-2 --target=xpic -c -save-temps b.c
:DELETE
if exist libb.a (
    echo deleting libb.a
    del libb.a
    goto DELETE
)


%PATH_GNU_XPIC%\bin\xpic-ar cr libb.a b.o 
%PATH_GNU_XPIC%\bin\xpic-objdump -s -S -d libb.a > objdump_libb.txt

REM compare lib object dump
fc objdump_libb.txt objdump_libb.txt_ref > NUL:
if %ERRORLEVEL%==0 (
  set /a i=%i% + 1
) else (
  echo libb.a dump failed!
)


%PATH_GNU_XPIC%\bin\clang -O0 -gdwarf-2 --target=xpic -c -save-temps d.c
:DELETE2
if exist libd.a (
    echo deleting libd.a
    del libd.a
    goto DELETE2
)
%PATH_GNU_XPIC%\bin\xpic-ar cr libd.a d.o 
%PATH_GNU_XPIC%\bin\xpic-objdump -s -S -d libd.a > objdump_libd.txt

REM compare lib object dump
fc objdump_libd.txt objdump_libd.txt_ref > NUL:
if %ERRORLEVEL%==0 (
  set /a i=%i% + 1
) else (
  echo libd.a dump failed!
)

%PATH_GNU_XPIC%\bin\clang -O0 -gdwarf-2 --target=xpic -c -save-temps a.c

REM enter manually wrong order inside command line
%PATH_GNU_XPIC%\bin\xpic-ld -L. a.o -ld -lb -o a_manually_wrong.out -Map a_manually_wrong.map 1> error.log 2>&1
fc error.log error.log_ref > NUL:
if %ERRORLEVEL%==0 (
  set /a i=%i% + 1
) else (
  echo test with wrong lib order failed!
)


REM enter manually correct order inside command line
%PATH_GNU_XPIC%\bin\xpic-ld -L. a.o -lb -ld -o a_manually.out -Map a_manually.map
fc a_manually.map a_manually.map_ref > NUL:
if %ERRORLEVEL%==0 (
  set /a i=%i% + 1
) else (
  echo test with correct lib order failed!
)
%PATH_GNU_XPIC%\bin\xpic-objdump -s -S -d a_manually.out > objdump_a_manually.txt
fc objdump_a_manually.txt objdump_a_manually.txt_ref > NUL:
if %ERRORLEVEL%==0 (
  set /a i=%i% + 1
) else (
  echo test with correct lib order failed!
)


REM let linker search for correct order
%PATH_GNU_XPIC%\bin\xpic-ld -L. a.o -( -ld -lb -) -o a_linker_automatic.out -Map a_linker_automatic.map
fc a_linker_automatic.map a_linker_automatic.map_ref > NUL:
if %ERRORLEVEL%==0 (
  set /a i=%i% + 1
) else (
  echo test with let linker search for correct lib order failed!
)
%PATH_GNU_XPIC%\bin\xpic-objdump -s -S -d a_linker_automatic.out > objdump_a_linker_automatic.txt
fc objdump_a_linker_automatic.txt objdump_a_linker_automatic.txt_ref > NUL:
if %ERRORLEVEL%==0 (
  set /a i=%i% + 1
) else (
  echo test with let linker search for correct lib order failed!
)

if %i%==7 (
  echo tests succeeded.
) else (
  echo test failed.
)

cd ..
