@echo off
echo Starting offline tests at:
date /T
time /T
if "%PATH_GNU_XPIC%"=="" (
  echo ERROR: PATH_GNU_XPIC not set!
) else (
  echo using xPIC compiler from directory: %PATH_GNU_XPIC%
  %PATH_GNU_XPIC%\bin\llc.exe --version
  for /D %%A in ("*") do (
    echo --------------------------------------------------------------------------------
    echo start test: %%A
    cd %%A
    if exist "start_test.bat" (
      call start_test.bat
    )
    cd ..
  )
)
echo finishing offline tests at
date /T
time /T