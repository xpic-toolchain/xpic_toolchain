all:
	cd modultests
	nmake
	cd ..\switchtester
	nmake

run:
	@modultests\bin\tester_helper.exe
	@modultests\bin\tester_handlearm.exe
	@start switchtester\bin\netx10_gdb_switch_switchtester.exe
	@switchtester\bin\switchtester.exe

clean:
	cd modultests
	nmake clean
	cd ..\switchtester
	nmake clean
