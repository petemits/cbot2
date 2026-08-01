@echo off
chcp 65001 >nul
echo ========================
echo QUICK COMPILE & RUN
echo ========================
echo.

:: First, make sure we're in the right directory
cd /d "%~dp0"

:: Compile
echo Step 1: Compiling...
g++ leadbot.cpp -o leadbot.exe -lwininet 2> compile_errors.txt

if not exist leadbot.exe (
    echo ❌ Compilation failed.
    echo Check compile_errors.txt
    type compile_errors.txt
    pause
    exit /b 1
)

echo ✅ Compilation successful!
echo.
echo Step 2: Running LeadBot...
echo.
leadbot.exe