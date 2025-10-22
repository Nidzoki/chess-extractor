@echo off
echo === RUNNING ALL TESTS ===
echo.

echo === Basic Test ===
examples\basic_usage.exe
if errorlevel 1 echo Basic test failed!
echo.

echo === Advanced Test ===
examples\advanced_test.exe
if errorlevel 1 echo Advanced test failed!
echo.

echo === Performance Test ===
examples\performance_test.exe
if errorlevel 1 echo Performance test failed!
echo.

echo === Huge File Test (if available) ===
if exist tests\huge_file_test.pgn (
    echo Found huge file, running test...
    examples\huge_file_test.exe
) else (
    echo No huge file found, skipping...
)
echo.

echo === ALL TESTS COMPLETED ===
pause