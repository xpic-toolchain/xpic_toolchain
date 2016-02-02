@echo off

REM unpack XPIC compiler Version into any directory and add the path to the installation
REM e.g. "unpack" xpic-llvm_201602011609.zip
REM set path to XPIC compiler installation
REM set PATH_GNU_XPIC=C:\Daten\Projekte\XPIC_Testenvironment\xpic-llvm_201412090825\xpic-llvm
set PATH_GNU_XPIC=C:\Daten\Projekte\XPIC_Testenvironment\xpic-llvm_201602011609\xpic-llvm
set PATH=%PATH%;%PATH_GNU_XPIC%\bin
echo PATH_GNU_XPIC: %PATH_GNU_XPIC%
