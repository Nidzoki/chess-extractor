@echo off
echo Building libpgn...

:: Check if g++ exists
g++ --version >nul 2>&1
if errorlevel 1 (
    echo ERROR: g++ not found. Please install MinGW-w64.
    pause
    exit /b 1
)

:: Create build directory if it doesn't exist
if not exist build mkdir build

:: Compile the library
echo Compiling library...
g++ -std=c++17 -O2 -Iinclude -c src/parser.cpp -o build/parser.o
if errorlevel 1 (
    echo Compilation failed!
    pause
    exit /b 1
)

:: Create static library
echo Creating library...
ar rcs build/libpgn.a build/parser.o

:: Build examples
echo Building examples...
g++ -std=c++17 -O2 -Iinclude examples/basic_usage.cpp build/libpgn.a -o examples/basic_usage.exe
g++ -std=c++17 -O2 -Iinclude examples/advanced_test.cpp build/libpgn.a -o examples/advanced_test.exe
g++ -std=c++17 -O2 -Iinclude examples/performance_test.cpp build/libpgn.a -o examples/performance_test.exe
g++ -std=c++17 -O2 -Iinclude examples/huge_file_test.cpp build/libpgn.a -o examples/huge_file_test.exe

if errorlevel 1 (
    echo Example compilation failed!
    pause
    exit /b 1
)

echo.
echo Build successful! 🎉
echo.
echo To run tests:
echo   test.bat
echo.
echo To test huge file:
echo   examples\huge_file_test.exe
pause