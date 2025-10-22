@echo off
echo RAM Monitoring Utility
echo.
echo Usage: monitor_ram.bat [process_name] [interval_seconds]
echo Default: monitor_ram.bat memory_comparison.exe 2
echo.

set PROCESS_NAME=%1
if "%PROCESS_NAME%"=="" set PROCESS_NAME=memory_comparison.exe

set INTERVAL=%2
if "%INTERVAL%"=="" set INTERVAL=2

echo Monitoring RAM usage for: %PROCESS_NAME%
echo Sampling interval: %INTERVAL% seconds
echo Press Ctrl+C to stop
echo.

:loop
tasklist /fi "imagename eq %PROCESS_NAME%" | find "%PROCESS_NAME%" >nul
if errorlevel 1 (
    echo [%time%] Process %PROCESS_NAME% not running...
) else (
    for /f "tokens=5" %%a in ('tasklist /fi "imagename eq %PROCESS_NAME%" /fo table /nh') do (
        echo [%time%] %PROCESS_NAME% - Memory: %%a
    )
)

timeout /t %INTERVAL% /nobreak >nul
goto :loop