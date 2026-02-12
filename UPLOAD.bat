@echo off
REM ============================================================
REM HELPStat Firmware Upload Script
REM One-click compilation and upload to ESP32-S3
REM ============================================================

setlocal enabledelayedexpansion
cls

echo.
echo ╔════════════════════════════════════════════════════════════╗
echo ║                HELPStat Firmware Uploader                  ║
echo ║              One-Click Build and Deploy                    ║
echo ╚════════════════════════════════════════════════════════════╝
echo.

REM Check if PlatformIO is installed
echo [1/4] Checking PlatformIO installation...
where python >nul 2>&1
if errorlevel 1 (
    echo.
    echo ✗ ERROR: Python not found in PATH
    echo   Please install Python and add it to your PATH
    echo.
    pause
    exit /b 1
)

python -m platformio --version >nul 2>&1
if errorlevel 1 (
    echo ✗ ERROR: PlatformIO not installed
    echo   Install it with: pip install platformio
    echo.
    pause
    exit /b 1
)

echo ✓ PlatformIO ready
echo.

REM Compile the firmware
echo [2/4] Compiling firmware...
python -m platformio run -s
if errorlevel 1 (
    echo.
    echo ✗ Compilation FAILED
    echo.
    pause
    exit /b 1
)

echo.
echo ✓ Compilation successful
echo.

REM Upload to device
echo [3/4] Uploading firmware to device...
echo   (Searching for ESP32-S3 on available COM ports...)
echo.

python -m platformio run --target upload
if errorlevel 1 (
    echo.
    echo ✗ Upload FAILED
    echo   Check the following:
    echo   • Is the ESP32-S3 connected via USB?
    echo   • Do drivers need to be installed?
    echo   • Try manually specifying port in platformio.ini
    echo.
    pause
    exit /b 1
)

echo.
echo ✓ Upload successful
echo.

REM Monitor serial output (optional)
echo [4/4] Ready for testing
echo.
echo ╔════════════════════════════════════════════════════════════╗
echo ║                  Upload Complete!                          ║
echo ╚════════════════════════════════════════════════════════════╝
echo.
echo Next Steps:
echo   1. Open a serial monitor (115200 baud)
echo   2. Type 'HELP' to see available commands
echo   3. Send a measurement: MEASURE:200000,10,5,0,0,1000,0,0,127000,150,0,0
echo.
echo Run this to monitor serial output:
echo   pio device monitor --port COMx --baud 115200
echo   (Replace COMx with your device's COM port)
echo.
pause
