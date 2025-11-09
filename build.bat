@echo off
REM Build script for Data Recovery Tool on Windows

echo ========================================
echo Data Recovery Tool - Build Script
echo ========================================
echo.

REM Check if CMake is installed
cmake --version >nul 2>&1
if %errorlevel% neq 0 (
    echo ERROR: CMake is not installed or not in PATH
    echo Please install CMake from https://cmake.org/download/
    pause
    exit /b 1
)

echo Creating build directory...
if not exist build mkdir build
cd build

echo.
echo Generating build files...
cmake .. -G "Visual Studio 16 2019" -A x64
if %errorlevel% neq 0 (
    echo.
    echo Trying with MinGW...
    cmake .. -G "MinGW Makefiles"
    if %errorlevel% neq 0 (
        echo ERROR: Failed to generate build files
        pause
        exit /b 1
    )
)

echo.
echo Building project...
cmake --build . --config Release
if %errorlevel% neq 0 (
    echo ERROR: Build failed
    pause
    exit /b 1
)

echo.
echo ========================================
echo Build completed successfully!
echo Executable location: build\bin\Release\DataRecoveryTool.exe
echo ========================================
echo.
echo IMPORTANT: Run the executable as Administrator!
echo.

pause
