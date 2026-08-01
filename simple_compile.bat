@echo off
chcp 65001 >nul
echo SIMPLE COMPILATION
echo ==================
echo.

:: Check if file exists
if not exist leadbot.cpp (
    echo ❌ leadbot.cpp not found!
    echo Run create_leadbot.bat first
    pause
    exit /b 1
)

echo 🔧 Compiling...
g++ leadbot.cpp -o leadbot.exe -lwininet

if exist leadbot.exe (
    echo ✅ SUCCESS! Compiled leadbot.exe
    echo.
    echo 🚀 Run with: leadbot.exe
    echo.
    pause
) else (
    echo ❌ Compilation failed.
    echo.
    echo Trying alternative...
    g++ leadbot.cpp -o leadbot.exe
    
    if exist leadbot.exe (
        echo ✅ SUCCESS without wininet!
        echo Run: leadbot.exe
        pause
    ) else (
        echo ❌ Still failed.
        pause
    )
)