#!/usr/bin/env pwsh
<#
.SYNOPSIS
    HELPStat Firmware Upload Script
    One-click compilation and upload to ESP32-S3

.DESCRIPTION
    This script automates the process of building and uploading the HELPStat
    firmware to an ESP32-S3 device via USB serial connection.

.EXAMPLE
    .\UPLOAD.ps1

.NOTES
    Requirements: Python, PlatformIO, USB drivers for ESP32-S3
#>

# Colors for output
$Colors = @{
    'Success' = 'Green'
    'Error'   = 'Red'
    'Warning' = 'Yellow'
    'Info'    = 'Cyan'
}

function Write-Title {
    param([string]$Text)
    Write-Host ""
    Write-Host "╔════════════════════════════════════════════════════════════╗" -ForegroundColor Cyan
    Write-Host "║ $Text$(Get-Padding $Text 58)" -ForegroundColor Cyan
    Write-Host "╚════════════════════════════════════════════════════════════╝" -ForegroundColor Cyan
    Write-Host ""
}

function Get-Padding {
    param([string]$Text)
    $padding = 57 - $Text.Length
    return " " * $padding + "║"
}

function Write-Status {
    param([string]$Message, [string]$Status = "Info")
    $prefix = if ($Status -eq "Success") { "✓" } elseif ($Status -eq "Error") { "✗" } else { "→" }
    $color = $Colors[$Status] ?? 'White'
    Write-Host "  $prefix $Message" -ForegroundColor $color
}

function Test-CommandExists {
    param([string]$CommandName)
    $command = Get-Command $CommandName -ErrorAction SilentlyContinue
    return $null -ne $command
}

Clear-Host

Write-Title "HELPStat Firmware Uploader"
Write-Host "One-Click Build and Deploy"
Write-Host ""

# Step 1: Check prerequisites
Write-Host "[1/5] Checking System Prerequisites" -ForegroundColor Cyan
Write-Host "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━" -ForegroundColor Cyan
Write-Host ""

# Check Python
if (-not (Test-CommandExists "python")) {
    Write-Status "Python NOT found" "Error"
    Write-Host ""
    Write-Host "Please install Python from https://www.python.org/" -ForegroundColor Yellow
    Write-Host "Make sure to add Python to PATH during installation"
    Write-Host ""
    Read-Host "Press Enter to exit"
    exit 1
}
Write-Status "Python found" "Success"

# Check PlatformIO
python -m platformio --version > $null 2>&1
if ($LASTEXITCODE -ne 0) {
    Write-Status "PlatformIO NOT installed" "Error"
    Write-Host ""
    Write-Host "Install PlatformIO using:" -ForegroundColor Yellow
    Write-Host "  pip install platformio" -ForegroundColor Yellow
    Write-Host ""
    Read-Host "Press Enter to exit"
    exit 1
}

$pioVersion = python -m platformio --version 2>&1
Write-Status "PlatformIO installed: $pioVersion" "Success"

Write-Host ""

# Step 2: Detect connected device
Write-Host "[2/5] Detecting Connected Devices" -ForegroundColor Cyan
Write-Host "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━" -ForegroundColor Cyan
Write-Host ""

$comPorts = @(Get-WmiObject Win32_SerialPort | Select-Object -ExpandProperty DeviceID)

if ($comPorts.Count -eq 0) {
    Write-Status "No serial port found" "Warning"
    Write-Host ""
    Write-Host "Troubleshooting:" -ForegroundColor Yellow
    Write-Host "  • Check USB cable connection"
    Write-Host "  • Install CH340 drivers if needed"
    Write-Host "  • Try different USB port"
    Write-Host ""
} else {
    Write-Status "Found $($comPorts.Count) serial port(s)" "Success"
    foreach ($port in $comPorts) {
        Write-Host "    • $port" -ForegroundColor Green
    }
}

Write-Host ""

# Step 3: Compile firmware
Write-Host "[3/5] Compiling Firmware" -ForegroundColor Cyan
Write-Host "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━" -ForegroundColor Cyan
Write-Host ""

Write-Status "Building project..." "Info"
python -m platformio run -s

if ($LASTEXITCODE -ne 0) {
    Write-Host ""
    Write-Status "Compilation FAILED" "Error"
    Write-Host ""
    Write-Host "Check the error messages above for details" -ForegroundColor Yellow
    Write-Host ""
    Read-Host "Press Enter to exit"
    exit 1
}

Write-Host ""
Write-Status "Compilation successful" "Success"
Write-Host ""

# Step 4: Upload to device
Write-Host "[4/5] Uploading to Device" -ForegroundColor Cyan
Write-Host "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━" -ForegroundColor Cyan
Write-Host ""

Write-Status "Starting upload process..." "Info"
python -m platformio run --target upload

if ($LASTEXITCODE -ne 0) {
    Write-Host ""
    Write-Status "Upload FAILED" "Error"
    Write-Host ""
    Write-Host "Troubleshooting:" -ForegroundColor Yellow
    Write-Host "  • Verify USB connection"
    Write-Host "  • Check if COM port is correct"
    Write-Host "  • Try pressing RESET button on device"
    Write-Host "  • Update platformio.ini with correct upload_port"
    Write-Host ""
    Read-Host "Press Enter to exit"
    exit 1
}

Write-Host ""
Write-Status "Upload successful" "Success"
Write-Host ""

# Step 5: Ready to test
Write-Host "[5/5] Ready for Testing" -ForegroundColor Cyan
Write-Host "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━" -ForegroundColor Cyan
Write-Host ""

Write-Title "Upload Complete!"

Write-Host "Next Steps:" -ForegroundColor Green
Write-Host ""
Write-Host "1. Open a serial monitor at 115,200 baud:"
Write-Host "   pio device monitor --ports COMx --baud 115200" -ForegroundColor Yellow
Write-Host ""
Write-Host "2. Try these test commands:"
Write-Host "   • Type: HELP" -ForegroundColor Yellow
Write-Host "   • Type: STATUS" -ForegroundColor Yellow
Write-Host "   • Type: MEASURE:200000,10,5,0,0,1000,0,0,127000,150,0,0" -ForegroundColor Yellow
Write-Host ""
Write-Host "3. Monitor output to verify operation"
Write-Host ""

$openMonitor = Read-Host "Open serial monitor now? (y/n)"
if ($openMonitor -eq 'y' -or $openMonitor -eq 'Y') {
    Write-Host ""
    Write-Status "Starting serial monitor..." "Info"
    Write-Host ""
    
    if ($comPorts.Count -gt 0) {
        $port = $comPorts[0]
        python -m platformio device monitor --port $port --baud 115200
    } else {
        Write-Status "Cannot open monitor - no COM port detected" "Warning"
        Write-Host ""
        Read-Host "Press Enter to continue"
    }
}

Write-Host ""
Write-Status "Done!" "Success"
