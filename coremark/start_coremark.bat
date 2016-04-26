@echo off

REM test release version

echo prepare Waf tool chain for debug option -gdwarf-2

REM set local waf environment to OLD debug option -gdwarf-2
REM verify manually, that only the gdwarf option is different
del .\Waf\hilscher_toolchains.py
del .\Waf\hilscher_toolchains.pyc
copy .\hilscher_toolchainsgdwarf2.py .\Waf\hilscher_toolchains.py


call ..\waf_1.6.11\waf.bat distclean
call ..\waf_1.6.11\waf.bat distclean
call ..\waf_1.6.11\waf.bat configure
call ..\waf_1.6.11\waf.bat build --conditions=release

echo generate disassembly of elf files
cd .\build\release\Targets\netX51_xpic\
%PATH_GNU_XPIC%\bin\xpic-objdump -g -S -d .\netx51_xpic_coremark_intram.elf > .\netx51_xpic_coremark_intram_objdump.txt
%PATH_GNU_XPIC%\bin\xpic-objdump -g -S -d .\netx51_xpic_coremark_tcm.elf >    .\netx51_xpic_coremark_tcm_objdump.txt
%PATH_GNU_XPIC%\bin\xpic-objdump -g -S -d .\netx51_xpic_coremark_sdram.elf >  .\netx51_xpic_coremark_sdram_objdump.txt
cd ..\..\..\..\


call ..\waf_1.6.11\waf.bat build --conditions=debugrel

echo generate disassembly of elf files
cd .\build\debugrel\Targets\netX51_xpic\
%PATH_GNU_XPIC%\bin\xpic-objdump -g -S -d .\netx51_xpic_coremark_intram.elf > .\netx51_xpic_coremark_intram_objdump.txt
%PATH_GNU_XPIC%\bin\xpic-objdump -g -S -d .\netx51_xpic_coremark_tcm.elf >    .\netx51_xpic_coremark_tcm_objdump.txt
%PATH_GNU_XPIC%\bin\xpic-objdump -g -S -d .\netx51_xpic_coremark_sdram.elf >  .\netx51_xpic_coremark_sdram_objdump.txt
cd ..\..\..\..\


call ..\waf_1.6.11\waf.bat build --conditions=debug

echo generate disassembly of elf files
cd .\build\debug\Targets\netX51_xpic\
%PATH_GNU_XPIC%\bin\xpic-objdump -g -S -d .\netx51_xpic_coremark_intram.elf > .\netx51_xpic_coremark_intram_objdump.txt
%PATH_GNU_XPIC%\bin\xpic-objdump -g -S -d .\netx51_xpic_coremark_tcm.elf >    .\netx51_xpic_coremark_tcm_objdump.txt
%PATH_GNU_XPIC%\bin\xpic-objdump -g -S -d .\netx51_xpic_coremark_sdram.elf >  .\netx51_xpic_coremark_sdram_objdump.txt
cd ..\..\..\..\


echo prepare Waf tool chain for debug option -gdwarf-4

REM set local waf environment to NEW debug option -gdwarf-4
REM verify manually, that only the gdwarf option is different
del .\Waf\hilscher_toolchains.py
del .\Waf\hilscher_toolchains.pyc
copy .\hilscher_toolchainsgdwarf4.py .\Waf\hilscher_toolchains.py


call ..\waf_1.6.11\waf.bat distclean
call ..\waf_1.6.11\waf.bat distclean
call ..\waf_1.6.11\waf.bat configure
call ..\waf_1.6.11\waf.bat build --conditions=release

echo generate disassembly of elf files
cd .\build\release\Targets\netX51_xpic\
%PATH_GNU_XPIC%\bin\xpic-objdump -g -S -d .\netx51_xpic_coremark_intram.elf > .\netx51_xpic_coremark_intram_objdump.txt
%PATH_GNU_XPIC%\bin\xpic-objdump -g -S -d .\netx51_xpic_coremark_tcm.elf >    .\netx51_xpic_coremark_tcm_objdump.txt
%PATH_GNU_XPIC%\bin\xpic-objdump -g -S -d .\netx51_xpic_coremark_sdram.elf >  .\netx51_xpic_coremark_sdram_objdump.txt
cd ..\..\..\..\


call ..\waf_1.6.11\waf.bat build --conditions=debugrel

echo generate disassembly of elf files
cd .\build\debugrel\Targets\netX51_xpic\
%PATH_GNU_XPIC%\bin\xpic-objdump -g -S -d .\netx51_xpic_coremark_intram.elf > .\netx51_xpic_coremark_intram_objdump.txt
%PATH_GNU_XPIC%\bin\xpic-objdump -g -S -d .\netx51_xpic_coremark_tcm.elf >    .\netx51_xpic_coremark_tcm_objdump.txt
%PATH_GNU_XPIC%\bin\xpic-objdump -g -S -d .\netx51_xpic_coremark_sdram.elf >  .\netx51_xpic_coremark_sdram_objdump.txt
cd ..\..\..\..\


call ..\waf_1.6.11\waf.bat build --conditions=debug

echo generate disassembly of elf files
cd .\build\debug\Targets\netX51_xpic\
%PATH_GNU_XPIC%\bin\xpic-objdump -g -S -d .\netx51_xpic_coremark_intram.elf > .\netx51_xpic_coremark_intram_objdump.txt
%PATH_GNU_XPIC%\bin\xpic-objdump -g -S -d .\netx51_xpic_coremark_tcm.elf >    .\netx51_xpic_coremark_tcm_objdump.txt
%PATH_GNU_XPIC%\bin\xpic-objdump -g -S -d .\netx51_xpic_coremark_sdram.elf >  .\netx51_xpic_coremark_sdram_objdump.txt
cd ..\..\..\..\


REM commit correct objdump.org file after dwarf4 is working
echo test failed!
