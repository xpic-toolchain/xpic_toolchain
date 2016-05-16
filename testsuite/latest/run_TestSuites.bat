@echo off
echo executing test suites
echo remove output directory from previous runs
:DELETE
if exist .\output (
    echo PLEASE delete directory .\output
	echo continue with ENTER
	pause
    goto DELETE
)
echo using compile option release
call .\testsuite.bat release > test_result_release.log  2>&1
type .\test_result_release.log
echo using compile option debugrel
call .\testsuite.bat debugrel > test_result_debugrel.log  2>&1
type .\test_result_debugrel.log
echo using compile option debug
call .\testsuite.bat debug > test_result_debug.log  2>&1
type .\test_result_debug.log
echo Done.
