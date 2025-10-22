@echo off
echo Cleaning build files...

:: Remove build directory
if exist build (
    rmdir /s /q build
    echo Removed build directory
)

:: Remove all executables from examples
if exist examples\*.exe (
    del examples\*.exe
    echo Removed executables from examples directory
)

:: Remove any other build artifacts
if exist *.o del *.o
if exist *.a del *.a

echo.
echo Clean complete! All build files have been removed.
pause