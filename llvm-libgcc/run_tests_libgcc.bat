@echo off
echo executing test libgcc
echo remove output directory from previous runs
:DELETE
if exist .\build\output (
    echo PLEASE delete directory .\build\output
	pause
    goto DELETE
)
:DELETE2
if exist .\install (
    echo PLEASE delete directory .\install
	pause
    goto DELETE2
)
echo using compile option release release
call .\testsuite_libgcc.bat release release > test_result_release.log  2>&1
type .\test_result_release.log
echo using compile option debugrel
call .\testsuite_libgcc.bat debugrel debugrel > test_result_debugrel.log  2>&1
type .\test_result_debugrel.log
echo using compile option debug
call .\testsuite_libgcc.bat debug debug > test_result_debug.log  2>&1
type .\test_result_debug.log
echo Done.
