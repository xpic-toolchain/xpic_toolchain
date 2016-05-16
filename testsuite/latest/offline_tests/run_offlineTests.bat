@echo off
echo executing offline test cases ...
call start_tests.bat > test_result.log  2>&1
type test_result.log
echo Done.
