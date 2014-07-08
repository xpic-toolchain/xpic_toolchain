@echo off
if "%PATH_GNU_XPIC%"=="" (
echo ERROR: PATH_GNU_XPIC not set!
) else (
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