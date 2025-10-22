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
g++ -std=c++17 -O2 -Iinclude -c src/streaming_parser.cpp -o build/streaming_parser.o
if errorlevel 1 (
    echo Compilation failed!
    pause
    exit /b 1
)

:: Create static libraries
echo Creating libraries...
ar rcs build/libpgn.a build/parser.o
ar rcs build/libpgn_streaming.a build/streaming_parser.o build/parser.o
if errorlevel 1 (
    echo Library creation failed!
    pause
    exit /b 1
)

:: Build examples
echo Building examples...
echo Building basic_usage...
g++ -std=c++17 -O2 -Iinclude examples/basic_usage.cpp build/libpgn.a -o examples/basic_usage.exe

echo Building advanced_test...
g++ -std=c++17 -O2 -Iinclude examples/advanced_test.cpp build/libpgn.a -o examples/advanced_test.exe

echo Building performance_test...
g++ -std=c++17 -O2 -Iinclude examples/performance_test.cpp build/libpgn.a -o examples/performance_test.exe

echo Building huge_file_test...
g++ -std=c++17 -O2 -Iinclude examples/huge_file_test.cpp build/libpgn.a -o examples/huge_file_test.exe

echo Building memory_test...
g++ -std=c++17 -O2 -Iinclude examples/memory_test.cpp build/libpgn_streaming.a -o examples/memory_test.exe

echo Building memory_tracking_test...
g++ -std=c++17 -O2 -Iinclude examples/memory_tracking_test.cpp build/libpgn_streaming.a -o examples/memory_tracking_test.exe -lpsapi

echo Building memory_comparison...
g++ -std=c++17 -O2 -Iinclude examples/memory_comparison.cpp build/libpgn.a build/libpgn_streaming.a -o examples/memory_comparison.exe -lpsapi

if errorlevel 1 (
    echo Example compilation failed!
    pause
    exit /b 1
)

echo.
echo Build successful! 🎉
echo.
echo Available executables:
echo   examples\basic_usage.exe          - Basic functionality test
echo   examples\advanced_test.exe        - Detailed statistics test  
echo   examples\performance_test.exe     - Performance comparison
echo   examples\huge_file_test.exe       - Original parser with huge files
echo   examples\memory_test.exe          - Memory-optimized parser
echo   examples\memory_tracking_test.exe - Memory usage tracking
echo   examples\memory_comparison.exe    - Compare both parsers side-by-side
echo.
echo To run all tests:
echo   test.bat
echo.
echo To compare memory usage:
echo   examples\memory_comparison.exe
pause