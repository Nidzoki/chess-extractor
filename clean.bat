@echo off
if exist build rmdir /s /q build
if exist examples\*.exe del examples\*.exe
echo Build files cleaned!
pause