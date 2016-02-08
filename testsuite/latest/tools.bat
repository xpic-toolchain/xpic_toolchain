@echo off
echo PATH_GNU_XPIC: %PATH_GNU_XPIC%

REM install these tools on top of the xpic-compiler
REM use the archive "xpic-llvm_testsuite_tools_$VERSION.zip"


start /D "%PATH_GNU_XPIC%\..\openocd" openocd.exe -s "..\netx_openocd_scripts" -f "..\netx_openocd_scripts\interface\hilscher_nxhx51_etm.cfg" -f "..\netx_openocd_scripts\board\hilscher_nxhx51.cfg"
echo "wait until openocd is started before continue with any key"
pause

start /D "%PATH_GNU_XPIC%\..\gdb_switch" gdb_switch_v2.exe -c netX51
echo "wait until gdb_switch is started before continue with any key"
pause
start /D "%PATH_GNU_XPIC%\bin" xpic-gdb --xpic-int-dram-addr 0x10280000 %1

REM usage of GDB (innerhalb der GBD Fensters eingeben)
REM tools über das tools.bat script anstarten. Damit nicht mit ADMIN gerechten gearbeitet wird. Könnte ein Problem sein hier mit Adminrechten zu arbeiten.
REM target remote :3335  # connect with target
REM file netx51_xpic_coremark_intram.elf  ## new file for debugger afterwards a load is required
REM continue ## started das geladenen Programm an
REM load 
REM ctrl-C halt den XPIC wieder an.

REM in normaler CMD Shell.
REM Hilfe netstat -na ## zum anzeigen der geöffneten Ports (listen)
