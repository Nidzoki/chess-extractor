@echo off
echo === QUICK TEST SUITE ===
echo.

echo Building everything...
call build.bat >nul 2>&1
if errorlevel 1 (
    echo Build failed! Run build.bat manually for details.
    pause
    exit /b 1
)

echo Running quick tests...
echo.

echo 1. Basic functionality test...
examples\basic_usage.exe >nul 2>&1
if errorlevel 1 (
    echo ❌ Basic test failed
) else (
    echo ✅ Basic test passed
)

echo 2. Advanced statistics test...
examples\advanced_test.exe >nul 2>&1
if errorlevel 1 (
    echo ❌ Advanced test failed
) else (
    echo ✅ Advanced test passed
)

echo 3. Performance test...
examples\performance_test.exe >nul 2>&1
if errorlevel 1 (
    echo ❌ Performance test failed
) else (
    echo ✅ Performance test passed
)

if exist tests\huge_file_test.pgn (
    echo 4. Memory comparison test (this will take a while)...
    examples\memory_comparison.exe >nul 2>&1
    if errorlevel 1 (
        echo ❌ Memory comparison test failed
    ) else (
        echo ✅ Memory comparison test passed
    )
) else (
    echo 4. Memory comparison test skipped (no huge file)
)

echo.
echo === QUICK TEST COMPLETED ===
echo Check individual test outputs for detailed results.
pause