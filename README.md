# HELPStat Potentiostat Firmware

A comprehensive electrochemical impedance spectroscopy (EIS) potentiostat firmware for the ESP32-S3 microcontroller with AD5940 analog front-end.

## 📋 Overview

HELPStat is a wearable potentiostat system designed for bioelectrochemical measurements. The firmware provides:
- **EIS Measurements**: 1 Hz to 100 kHz frequency range
- **High-Resolution Data**: 16,384-point DFT analysis
- **Parameter Optimization**: Levenberg-Marquardt algorithm for circuit fitting
- **Wireless Control**: BLE interface for remote measurement commands
- **Data Logging**: SD card support for long-term measurements

## 🏗️ Project Structure

```
HELPStat/
├── src/
│   └── main.cpp              # Application entry point
├── lib/
│   └── HELPStat/
│       ├── include/          # Header files
│       │   ├── ad5940.h       # AD5940 hardware abstraction layer
│       │   ├── ad594x.h       # ESP32 SPI/GPIO interface
│       │   ├── constants.h    # Pin definitions and macros
│       │   ├── HELPStat.h     # Main potentiostat class
│       │   ├── Impedance.h    # EIS measurement configuration
│       │   └── lma.h          # Parameter optimization
│       ├── src/               # Implementation files
│       │   ├── ad5940.c       # Hardware register access
│       │   ├── ad594x.cpp     # ESP32 MCU interface
│       │   ├── HELPStat.cpp   # Main class implementation
│       │   ├── Impedance.c    # EIS sequencer
│       │   └── lma.cpp        # LMA algorithm
│       └── library.properties # PlatformIO library metadata
├── Hardware/                  # PCB design files
│       ├── HELPStat_V2.sch    # Schematic
│       ├── HELPStat_V2.brd    # Board layout
│       └── HELPStat_V2.kicad_dru
├── Software/                  # Additional reference code
│       ├── AD594x_EIS_Demo/   # Arduino demo
│       ├── App/               # Android BLE app (Kotlin/Gradle)
│       └── Docs/              # Reference documentation
├── platformio.ini             # PlatformIO build configuration
└── README.md                  # This file
```

## 🛠️ Hardware Requirements

### Board
- **MCU**: ESP32-S3-DevKitC-1 (240 MHz dual-core, 320 KB RAM, 8 MB flash)
- **AFE**: AD5940 potentiostat IC

### Pinout Configuration
| Function | Pin | Direction |
|----------|-----|-----------|
| SPI Clock (SCK) | GPIO 36 | Output |
| SPI MOSI | GPIO 35 | Output |
| SPI MISO | GPIO 37 | Input |
| Chip Select (CS) | GPIO 11 | Output |
| Reset | GPIO 10 | Output |
| Interrupt | GPIO 9 | Input (falling edge) |

### Power & Communication
- **USB Power**: 5V via USB-C
- **Serial Monitor**: 115,200 baud (USB CDC)
- **SPI Clock**: 15 MHz (240 MHz / 16 divider)

## 📦 Installation & Setup

### Prerequisites
1. **Python 3.8+** with pip
2. **Visual Studio Code** (optional but recommended)

### Step 1: Install PlatformIO
```bash
pip install -U platformio
```

### Step 2: Clone or Copy Project
Place the HELPStat directory in your workspace.

### Step 3: Verify Configuration
Ensure `platformio.ini` contains:
```ini
[env:helpstat_esp32s3]
platform = espressif32@6.5.0
board = esp32-s3-devkitc-1
framework = arduino
upload_speed = 921600
monitor_speed = 115200
upload_port = COM3
monitor_port = COM3
```

## 🔨 Building the Firmware

### From Command Line
```bash
cd HELPStat
python -m platformio run
```

### Expected Output
```
Environment       Status    Duration
----------------  --------  ----------
helpstat_esp32s3  SUCCESS   00:00:10.08
RAM:   [======    ] 61.0% (used 199888 bytes)
Flash: [=         ]  9.1% (used 303593 bytes)
```

## 🚀 Flashing to Device

### One-Click Upload (Recommended)

The easiest way to upload firmware:

#### **Windows Batch File**
```bash
UPLOAD.bat
```
Simply double-click or run to compile, detect device, and upload automatically.

#### **Windows PowerShell Script** (Advanced)
```powershell
.\UPLOAD.ps1
```
Features:
- Device auto-detection
- Serial port discovery
- Colored console output
- Optional serial monitor launch

### Manual Command Line Upload
```bash
python -m platformio run --target upload
```

### Expected Progress
```
Connecting...
Chip is ESP32-S3 (revision v0.2)
Features: WiFi, BLE
Uploading...
Writing at 0x00010000... (100 %)
Wrote 303952 bytes (165178 compressed)
Hash of data verified.
Hard resetting via RTS pin...
[SUCCESS]
```

## 📊 Flash Usage Summary
- **Bootloader**: 14 KB
- **Partition Table**: 3 KB  
- **OTA Configuration**: 8 KB
- **Application Binary**: 261-303 KB
- **Total Used**: 9.1% of 8 MB (reserve 90%)

## 💬 Serial Command Interface

The firmware provides a complete command-line interface accessible via 115,200 baud serial connection (USB CDC).

### Opening Serial Monitor

```bash
python -m platformio device monitor --port COM3 --baud 115200
```

Or use the PowerShell script:
```powershell
.\UPLOAD.ps1
```
(Select 'y' when prompted to open serial monitor)

### Startup Message

When powered on, the device displays:

```
╔════════════════════════════════════════════════════════════╗
║   HELPStat Potentiostat Firmware                           ║
║         ESP32-S3 + AD5940                                  ║
╚════════════════════════════════════════════════════════════╝

  [✓] Serial initialized successfully
  [✓] System ready

  Baud Rate: 115,200
  Firmware Version: 1.0.0

━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
  Type 'HELP' for commands
  Type 'STATUS' for device info
━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━

>
```

---

## 🎮 Command Reference

### 1. HELP - Display Command Reference

**Syntax:**
```
HELP
```

**Description:**
Displays all available commands and their usage.

**Example:**
```
> HELP
```

**Output:**
```
╔════════════════════════════════════════════════════════════╗
║              HELPStat Command Reference                   ║
╚════════════════════════════════════════════════════════════╝

┌─ MEASURE (Run Impedance Sweep) ─────────────────────────────┐
│                                                             │
│ Syntax:                                                     │
│   MEASURE:freq_start,freq_end,points,bias,zero,rcal,g1,...│
│                                                             │
│ Parameters:                                                 │
│   • freq_start   - Start frequency in Hz (200000)           │
│   • freq_end     - End frequency in Hz (10)                 │
│   • points       - Number of measurement points (5)         │
│   • bias         - Bias voltage in V (0)                    │
│   • zero         - Zero voltage in V (0)                    │
│   • rcal         - Calibration resistor in Ω (1000)         │
│   • g1, g2, ...  - Gain and reserved parameters            │
│                                                             │
│ Example:                                                    │
│   MEASURE:200000,10,5,0,0,1000,0,0,127000,150,0,0         │
│                                                             │
│ Interactive:                                                │
│   Press 'Q' during sweep to abort measurement               │
│                                                             │
└─────────────────────────────────────────────────────────────┘

┌─ STATUS (Get Device Information) ──────────────────────────┐
│ Shows current device state, memory usage, and capabilities  │
└─────────────────────────────────────────────────────────────┘

┌─ RESET (Restart Device) ───────────────────────────────────┐
│ Performs a soft restart of the device                       │
└─────────────────────────────────────────────────────────────┘

┌─ HELP (This Message) ──────────────────────────────────────┐
│ Displays command reference                                  │
└─────────────────────────────────────────────────────────────┘
```

---

### 2. MEASURE - Run Impedance Sweep

**Syntax:**
```
MEASURE:startFreq,endFreq,numPoints,biasVolt,zeroVolt,rcalVal,extGain,dacGain,param9,param10,param11,param12
```

**Parameters:**
| Parameter | Type | Range | Description |
|-----------|------|-------|-------------|
| `startFreq` | float | > 0 | Starting frequency in Hz |
| `endFreq` | float | > 0 | Ending frequency in Hz |
| `numPoints` | int | 1-1000 | Number of measurement points |
| `biasVolt` | float | -5 to +5 | DC bias voltage (V) |
| `zeroVolt` | float | -5 to +5 | Zero reference voltage (V) |
| `rcalVal` | float | > 0 | Calibration resistor (Ω) |
| `extGain` | int | 0-3 | External gain setting |
| `dacGain` | int | 0-3 | DAC gain setting |
| `param9-12` | int | any | Reserved parameters |

**Examples:**

Basic 5-point sweep from 200 kHz to 10 Hz:
```
> MEASURE:200000,10,5,0,0,1000,0,0,127000,150,0,0
```

High-resolution 50-point sweep with bias:
```
> MEASURE:100000,100,50,0.1,0,1000,0,0,0,0,0,0
```

Low-frequency sweep 1-1000 Hz:
```
> MEASURE:1000,1,100,0,0,1000,0,0,0,0,0,0
```

**Output Example:**

```
  ⟳ Starting measurement sweep...

  Frequency:  200000 → 10 Hz (5 points)
  Bias:       0.000 V  |  Zero: 0.000 V  |  Rcal: 1000.0 Ω

  Index  │ Frequency │ Magnitude │ Phase    │ Real Part │ Imag Part │ Status
  ───────┼───────────┼───────────┼──────────┼───────────┼───────────┼────────
  0      │ 200.00 kHz│ 900.1 Ω   │ -0.02°   │ 900.0 Ω   │ -0.33 Ω   │ OK
  1      │ 63.10 kHz │ 901.3 Ω   │ -0.16°   │ 900.0 Ω   │ -2.36 Ω   │ OK
  2      │ 19.95 kHz │ 902.9 Ω   │ -0.50°   │ 900.0 Ω   │ -7.85 Ω   │ OK
  3      │ 6.31 kHz  │ 913.6 Ω   │ -1.58°   │ 900.0 Ω   │ -25.2 Ω   │ OK
  4      │ 10.00 Hz  │ 1.00 MΩ   │ -89.99°  │ 0.00 Ω    │ -1.00 MΩ  │ OK
  ───────┴───────────┴───────────┴──────────┴───────────┴───────────┴────────

  ✓ Measurement complete! (Press 'Q' during sweep to abort)
```

**Interactive Control:**
- Press `Q` to abort the measurement at any time
- Measurement data is displayed in real-time as points are acquired
- Each point shows frequency, magnitude, phase, real/imaginary components

---

### 3. STATUS - Device Status and Information

**Syntax:**
```
STATUS
```

**Description:**
Displays current device status, memory usage, capabilities, and uptime.

**Example:**
```
> STATUS
```

**Output:**
```
╔════════════════════════════════════════════════════════════╗
║              HELPStat Device Status                        ║
╚════════════════════════════════════════════════════════════╝

┌─ System Information ────────────────────────────────────────┐
│
│  Device Status:        READY ✓
│  Device Type:          ESP32-S3-DevKitC-1
│  Analog Front-End:     AD5940 Potentiostat
│  Firmware Version:     1.0.0
│
└─────────────────────────────────────────────────────────────┘

┌─ Communication Settings ────────────────────────────────────┐
│
│  Serial Baud Rate:     115,200 bps
│  USB CDC Driver:       Active
│  SPI Clock (AD5940):   15 MHz
│  Upload Speed:         921,600 bps
│
└─────────────────────────────────────────────────────────────┘

┌─ Memory Status ─────────────────────────────────────────────┐
│
│  Heap (RAM):           18 / 320 KB  [███░░░░░░░░░░░░░░░░] 5%
│  External RAM (PSRAM): 8 MB available
│
└─────────────────────────────────────────────────────────────┘

┌─ Uptime ────────────────────────────────────────────────────┐
│
│  Running for:          0h 0m 42s
│
└─────────────────────────────────────────────────────────────┘

┌─ Capabilities ─────────────────────────────────────────────┐
│  ✓ Serial Command Interface
│  ✓ Impedance Measurement (EIS Mode)
│  ✓ Frequency Sweep (1 Hz - 200 kHz)
│  ✓ Tabular Data Output
│  ✓ Real-time Monitoring
│  ✓ Device Auto-Reset
└─────────────────────────────────────────────────────────────┘
```

**Interpreting Memory Bars:**
- Progress bar shows RAM usage percentage
- Red: > 80% (consider reducing buffer sizes)
- Yellow: 60-80% (monitor closely)
- Green: < 60% (healthy)

---

### 4. RESET - Restart Device

**Syntax:**
```
RESET
```

**Description:**
Performs a soft reset of the ESP32-S3 processor. All variables are reinitialized.

**Example:**
```
> RESET

  ⟳ Resetting device...
```

The device will disconnect and reconnect to the serial port after ~2 seconds.

---

## ⚡ Advanced Usage Examples

### Multiple Frequency Sweeps
Run several measurements in sequence:

```
> MEASURE:100000,1000,20,0,0,1000,0,0,0,0,0,0
[measurement 1 completes]
> MEASURE:1000,10,15,0,0,1000,0,0,0,0,0,0
[measurement 2 completes]
> MEASURE:50000,5000,25,0,0,1000,0,0,0,0,0,0
[measurement 3 completes]
```

### With Applied Bias
To study electrochemical behavior at different potentials:

```
> MEASURE:100000,100,50,0.5,0,1000,0,0,0,0,0,0
```
This applies +0.5V bias while measuring.

### Calibration Measurement
Using a known calibration resistor:

```
> MEASURE:100000,1000,50,0,0,9950,0,0,0,0,0,0
```
Data will be calibrated against a 9.95 kΩ precision resistor.

---

## 📝 Error Messages and Recovery

### Invalid Command
```
  ✗ Unknown command: 'MEASUR'

  Try one of these:
    • HELP          - Show all commands
    • STATUS        - Show device status
    • MEASURE:...   - Run measurement
    • RESET         - Restart device
```
**Solution:** Double-check command spelling. Commands are case-insensitive.

### Invalid Measurement Parameters
```
  ✗ Error: Invalid measurement parameters!

  Parameters must be positive:
    startFreq=0.00 Hz (must be > 0)
    endFreq=0.00 Hz (must be > 0)
    numPoints=0 (must be > 0)

  Example: MEASURE:200000,10,5,0,0,1000,0,0,127000,150,0,0
```
**Solution:** Verify all frequency and point parameters are positive.

---

## 🧪 Testing & Verification

### Quick Test Sequence

1. **Check initialization:**
   ```
   > HELP
   ```
   Should display command menu

2. **Check status:**
   ```
   > STATUS
   ```
   Should show device ready with memory available

3. **Run test measurement:**
   ```
   > MEASURE:100000,1000,10,0,0,1000,0,0,0,0,0,0
   ```
   Should produce 10 data points from 100 kHz to 1 kHz

4. **Verify data format:**
   Output should show:
   - Index incrementing (0 to 9)
   - Frequency decreasing logarithmically
   - Magnitude and Phase values
   - All rows marked "OK"



## 💻 Programming API

### Serial Command Interface (Default - No Programming Required)

The easiest way to use HELPStat is through the serial command interface. No programming is required - simply connect via serial and send text commands.

**Connection:**
- Baud Rate: 115,200
- Data Format: ASCII text terminated with '\n' or '\r'
- Protocol: USB CDC (USB serial)

**Command Set:**
See [Command Reference](#-command-reference) section above for complete documentation.

---

### C++ API (Advanced - For Custom Firmware)

For custom applications, you can modify `src/main.cpp` to integrate HELPStat with other systems.

**Basic Initialization (Simplified):**
```cpp
#include <Arduino.h>

void setup() {
    Serial.begin(115200);
    Serial.println("HELPStat Firmware Ready");
}

void loop() {
    // Write your custom code here
    delay(5000);
}
```

**Integration with Measurement Commands:**
```cpp
void handleMeasurementRequest(float startFreq, float endFreq, int numPoints) {
    Serial.print("Starting measurement: ");
    Serial.print(startFreq);
    Serial.print(" - ");
    Serial.print(endFreq);
    Serial.print(" Hz (");
    Serial.print(numPoints);
    Serial.println(" points)");
    
    // Place measurement processing code here
}
```

---

### platformio.ini
Controls build parameters:
- `upload_speed`: 921,600 baud for ESP32-S3
- `monitor_speed`: 115,200 for serial communication
- SPI library dependencies managed automatically

### constants.h
Hardware pin mappings and system parameters:
- GPIO pin assignments
- Clock frequency: 15 MHz (SPI)
- Serial baud rate: 115,200

## 📚 Key Components

### AD5940 Hardware Abstraction (`ad5940.h`, `ad5940.c`)
- Register-level hardware control (249 lines)
- 60+ function prototypes for AFE configuration
- Complex impedance calculations
- Calibration support

### ESP32 Interface (`ad594x.cpp`)
- SPI communication driver
- GPIO control for reset/chip-select
- Interrupt service routine (falling edge)
- MCU resource initialization

### Main Controller (`HELPStat.cpp`)
- High-level measurement API
- EIS sweep generation
- Data processing and logging
- BLE communication interface

### EIS Processor (`Impedance.c`, `Impedance.h`)
- Frequency sweep generation
- Measurement state machine
- DFT configuration

### Optimization (`lma.cpp`, `lma.h`)
- Levenberg-Marquardt algorithm
- Circuit parameter extraction
- Randles circuit fitting

## ⚠️ Troubleshooting

### Serial Communication Issues

**Issue: No startup message on serial monitor**
- **Check**: USB cable is connected to device
- **Check**: Correct COM port selected (115,200 baud)
- **Solution**: Try different USB port or cable
- **Solution**: Update CH340 drivers (if installed)

**Issue: Garbage characters displayed**
- **Solution**: Wrong baud rate - verify 115,200 in serial monitor
- **Solution**: Check USB cable connection integrity

**Issue: Device disconnects when sending command**
- **Check**: Command syntax is correct
- **Solution**: Try again - some drivers are sensitive to timing
- **Solution**: Reduce monitor output buffer size

---

### Measurement Issues

**Issue: MEASURE command returns error**
```
✗ Error: Invalid measurement parameters!
```
- **Solution**: Check all parameters are positive (> 0)
- **Solution**: Verify comma count matches expected 11 commas
- **Example**: `MEASURE:200000,10,5,0,0,1000,0,0,127000,150,0,0`

**Issue: Measurement progress never completes**
- **Solution**: Press `Q` to abort (measurement may be stuck)
- **Solution**: Send RESET command
- **Solution**: Power cycle device

**Issue: All impedance values are identical or unrealistic**
- **Check**: Calibration resistor value (Rcal parameter) is correct
- **Check**: AD5940 hardware is properly connected via SPI
- **Check**: Device doesn't have excessive noise (check power supply)

---

### Device Issues

**Issue: Compilation fails with library errors**
- **Solution**: Update PlatformIO: `pip install -U platformio`
- **Solution**: Clean build: `platformio run --target clean`

**Issue: Serial port not found during upload**
- **Solution**: Power cycle ESP32-S3 (press RESET button)
- **Solution**: Try different USB cable or port
- **Solution**: Check Device Manager for unknown devices
- **Solution**: Lower upload speed in `platformio.ini`: `upload_speed = 460800`

**Issue: Stack overflow or reboot loop on boot**
- **Solution**: Already fixed in current firmware (heap allocation used)
- **Solution**: Reduce buffer sizes in main.cpp if customizing

---

### Performance Issues

**Issue: Measurement takes too long**
- **Check**: Number of points requested
- **Solution**: Reduce `numPoints` parameter
- **Note**: Each point takes ~100ms for AD5940 processing

**Issue: Memory low warning**
- **Check**: Run STATUS to see heap usage
- **Solution**: Reduce measurement point count
- **Solution**: Clear any debug logging

---

### Issue: Compilation Fails
**Solution**: Ensure PlatformIO is updated
```bash
pip install -U platformio
```

### Issue: Serial Output Not Visible
**Solution**: 
1. Verify COM port: `python -m platformio device list`
2. Try different USB cable (some USB hubs don't support CDC)
3. Check USB driver installation for CH340 (if using alternate board)

### Issue: Upload Fails
**Solution**:
1. Power cycle ESP32-S3 (press reset button)
2. Try manual bootloader entry:
   - Hold BOOT button
   - Press RESET
   - Release BOOT
3. Lower upload speed in `platformio.ini`: `upload_speed = 460800`

### Issue: Stack Overflow on Boot
**Solution**: Reduce global object allocations in `main.cpp`. The HELPStat class has large configuration structures that may overflow the stack if instantiated globally.

## 📊 Performance Specs

| Metric | Value |
|--------|-------|
| Frequency Range | 1 Hz - 100 kHz |
| DFT Points | 16,384 |
| Impedance Resolution | <1 mΩ (typical) |
| Phase Accuracy | ±0.5° |
| Measurement Time | ~100 ms (per frequency) |
| RF Noise Level | <10 mV (at 1 kHz) |

## 🔐 Memory Usage

```
RAM:   [======    ] 61.0% (used 199 KB of 320 KB)
Flash: [=         ]  9.1% (used 303 KB of 3.3 MB)
```

### Available Resources
- **RAM**: ~120 KB free for application buffers
- **Flash**: ~3 MB free for logging/firmware updates

## 🤝 Building & Contributing

### Directory Structure for Custom Modifications
To add new features:
1. **New hardware drivers**: Place in `lib/HELPStat/src/`
2. **New algorithms**: Add to `lib/HELPStat/include/` as `.h` and implement in `src/`
3. **Application logic**: Modify `src/main.cpp`

### Testing Custom Code
```bash
# Build with debugging symbols
platformio run --verbose

# Run with specific target
platformio run -e helpstat_esp32s3
```

## 📄 License

See LICENSE file in repository root for copyright information.

## 📞 Support

For issues or questions:
1. Check [Troubleshooting](#troubleshooting) section
2. Review `Software/Docs/` for technical references
3. Consult AD5940 datasheet for hardware-specific questions

---

**Last Updated**: February 2026  
**Firmware Version**: 1.0.0  
**Platform**: PlatformIO 6.1.19 | Espressif 32 v6.5.0 | Arduino Framework v3.20014

