@echo off

REM   "Path to waf-executable relative to waf.bat"
SET WAF_EXE=dist\waf-1.6.11.exe


REM   "Try to set path with "%~dp0"expression"
SET WAFDIR=%~dp0

REM   "Check if target exist and path is Ok"
IF NOT EXIST "%WAFDIR%%WAF_EXE%" GOTO TARGET_NOT_FOUND1

REM   "Path is Ok, execute the programm"
"%WAFDIR%%WAF_EXE%" %*

REM   "jump to exit"
goto EXIT_SCRIPT



REM   "Wrong path found (possible under Win7)"
:TARGET_NOT_FOUND1

REM   "Search for the "waf.bat" in all pathes defined in environment PATH"
for %%P in (. %path:)=^)%) do if exist "%%P\%~nx0" set "WAFDIR=%%~fP\"

REM "Prepare error code for target not found"
cmd /c "exit /b 1"

REM   "Check if target exist and path is Ok"
IF NOT EXIST "%WAFDIR%%WAF_EXE%" GOTO TARGET_NOT_FOUND2

REM   "Path is Ok, execute the programm"
"%WAFDIR%%WAF_EXE%" %*

REM   "jump to exit"
goto EXIT_SCRIPT

REM   "Waf not found, error"
:TARGET_NOT_FOUND2
echo error: waf.bat not found in PATH

:EXIT_SCRIPT
REM   "On exit return a last error, let to know the upper layer (e.g. buildserver) an execution status"
cmd /c "exit /b %errorlevel%"
