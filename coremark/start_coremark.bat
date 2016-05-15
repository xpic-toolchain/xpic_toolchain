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


REM echo prepare Waf tool chain for debug option -gdwarf-4
REM 
REM REM set local waf environment to NEW debug option -gdwarf-4
REM REM verify manually, that only the gdwarf option is different
REM del .\Waf\hilscher_toolchains.py
REM del .\Waf\hilscher_toolchains.pyc
REM copy .\hilscher_toolchainsgdwarf4.py .\Waf\hilscher_toolchains.py
REM 
REM 
REM call ..\waf_1.6.11\waf.bat distclean
REM call ..\waf_1.6.11\waf.bat distclean
REM call ..\waf_1.6.11\waf.bat configure
REM call ..\waf_1.6.11\waf.bat build --conditions=release
REM 
REM echo generate disassembly of elf files
REM cd .\build\release\Targets\netX51_xpic\
REM %PATH_GNU_XPIC%\bin\xpic-objdump -g -S -d .\netx51_xpic_coremark_intram.elf > .\netx51_xpic_coremark_intram_objdump.txt
REM %PATH_GNU_XPIC%\bin\xpic-objdump -g -S -d .\netx51_xpic_coremark_tcm.elf >    .\netx51_xpic_coremark_tcm_objdump.txt
REM %PATH_GNU_XPIC%\bin\xpic-objdump -g -S -d .\netx51_xpic_coremark_sdram.elf >  .\netx51_xpic_coremark_sdram_objdump.txt
REM cd ..\..\..\..\
REM 
REM 
REM call ..\waf_1.6.11\waf.bat build --conditions=debugrel
REM 
REM echo generate disassembly of elf files
REM cd .\build\debugrel\Targets\netX51_xpic\
REM %PATH_GNU_XPIC%\bin\xpic-objdump -g -S -d .\netx51_xpic_coremark_intram.elf > .\netx51_xpic_coremark_intram_objdump.txt
REM %PATH_GNU_XPIC%\bin\xpic-objdump -g -S -d .\netx51_xpic_coremark_tcm.elf >    .\netx51_xpic_coremark_tcm_objdump.txt
REM %PATH_GNU_XPIC%\bin\xpic-objdump -g -S -d .\netx51_xpic_coremark_sdram.elf >  .\netx51_xpic_coremark_sdram_objdump.txt
REM cd ..\..\..\..\
REM 
REM 
REM call ..\waf_1.6.11\waf.bat build --conditions=debug
REM 
REM echo generate disassembly of elf files
REM cd .\build\debug\Targets\netX51_xpic\
REM %PATH_GNU_XPIC%\bin\xpic-objdump -g -S -d .\netx51_xpic_coremark_intram.elf > .\netx51_xpic_coremark_intram_objdump.txt
REM %PATH_GNU_XPIC%\bin\xpic-objdump -g -S -d .\netx51_xpic_coremark_tcm.elf >    .\netx51_xpic_coremark_tcm_objdump.txt
REM %PATH_GNU_XPIC%\bin\xpic-objdump -g -S -d .\netx51_xpic_coremark_sdram.elf >  .\netx51_xpic_coremark_sdram_objdump.txt
REM cd ..\..\..\..\


REM TODO: commit correct objdump.org files after dwarf4 is working
REM TODO: add dwarf compare after DWARF-4 is working
echo run tests at hw and get the result via UART interface from HW!


REM some help follows
REM see inside KB page: https://kb.hilscher.com/x/GtSTAg
