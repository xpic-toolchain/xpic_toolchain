@set TOP_DIR=d:\work\netx10_gdb_switch
@echo %TOP_DIR%
d:

start %TOP_DIR%\bin\openocd.exe -f %TOP_DIR%\bin\nxhx10_etm.cfg

timeout /T 6

start cmd.exe /K %TOP_DIR%\WIN7_X64_DEBUG\netx10_gdb_switch.exe -v -s

cd %TOP_DIR%\bin\xpic\isr_test

start %TOP_DIR%\bin\xpic\xpic-gdb.exe xpic.elf -ex "target remote :3335" -ex "set remotetimeout 10"

start C:\Windows\System32\cmd.exe /E:ON /V:ON /T:0E /K "D:\Program Files\Microsoft SDKs\Windows\v7.0\Bin\SetEnv.cmd"
