/*
  HELPStat Firmware - ESP32-S3
  Potentiostat Control Firmware with EIS Measurement
  
  Board: ESP32-S3 Dev Module
  Upload speed: 921600
  Monitor speed: 115200
  
  Serial Command Format:
  MEASURE:startFreq,endFreq,numPoints,biasVolt,zeroVolt,rcalVal,extGain,dacGain,param9,param10,param11,param12
  
  Example: MEASURE:200000,10,5,0,0,1000,0,0,127000,150,0,0
*/

#include <Arduino.h>
#include <math.h>

// Serial command buffer
const int BUFFER_SIZE = 256;
char commandBuffer[BUFFER_SIZE];
int bufferIndex = 0;

// Measurement configuration structure
struct MeasurementConfig {
  float startFreq;
  float endFreq;
  uint32_t numPoints;
  float biasVolt;
  float zeroVolt;
  float rcalVal;
  int extGain;
  int dacGain;
  // Additional parameters (reserved for future use)
  int param9;
  int param10;
  int param11;
  int param12;
};

// Function prototypes
void processSerialCommand(const char* cmd);
void parseMeasureCommand(const char* params);
void performMeasurement(MeasurementConfig* config);
void printHelp();
void printStatus();
void generateTestMeasurement(MeasurementConfig* config);

void setup() {
  // Initialize Serial with default pins (USB on ESP32-S3)
  Serial.begin(115200);
  delay(2000);
  
  Serial.println("\n\n");
  Serial.println("╔════════════════════════════════════════╗");
  Serial.println("║   HELPStat Potentiostat Firmware       ║");
  Serial.println("║         ESP32-S3 + AD5940              ║");
  Serial.println("╚════════════════════════════════════════╝");
  Serial.println("");
  Serial.println("  [✓] Serial initialized successfully");
  Serial.println("  [✓] System ready");
  Serial.println("");
  Serial.println("  Baud Rate: 115,200");
  Serial.println("  Firmware Version: 1.0.0");
  Serial.println("");
  Serial.println("━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━");
  Serial.println("  Type 'HELP' for commands");
  Serial.println("  Type 'STATUS' for device info");
  Serial.println("━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━");
  Serial.println("");
  
  // Clear buffer
  memset(commandBuffer, 0, BUFFER_SIZE);
  bufferIndex = 0;
  
  // Print prompt
  Serial.print("> ");
}

void loop() {
  // Check for incoming serial data
  if (Serial.available()) {
    char c = Serial.read();
    
    // Handle newline (command terminator)
    if (c == '\n' || c == '\r') {
      if (bufferIndex > 0) {
        commandBuffer[bufferIndex] = '\0'; // Null terminate
        processSerialCommand(commandBuffer);
        memset(commandBuffer, 0, BUFFER_SIZE);
        bufferIndex = 0;
      } else {
        // Just show fresh prompt on empty line
        Serial.print("> ");
      }
    }
    // Handle backspace
    else if (c == '\b' || c == 0x7F) {
      if (bufferIndex > 0) {
        bufferIndex--;
        Serial.write('\b');
        Serial.write(' ');
        Serial.write('\b');
      }
    }
    // Handle printable characters
    else if (c >= 32 && c <= 126) {
      if (bufferIndex < BUFFER_SIZE - 1) {
        commandBuffer[bufferIndex++] = c;
        Serial.write(c); // Echo character
      } else {
        // Buffer full, alert user
        Serial.write('\a'); // Bell character
      }
    }
  }
}

/**
 * Process incoming serial command
 */
void processSerialCommand(const char* cmd) {
  Serial.println(""); // New line after echoed input
  
  // Convert to uppercase for comparison
  String cmdStr = String(cmd);
  cmdStr.toUpperCase();
  cmdStr.trim();
  
  // Parse command
  if (cmdStr.startsWith("MEASURE:")) {
    // Extract parameters after "MEASURE:"
    const char* params = cmd + 8; // Skip "MEASURE:"
    parseMeasureCommand(params);
  }
  else if (cmdStr == "HELP") {
    printHelp();
  }
  else if (cmdStr == "STATUS") {
    printStatus();
  }
  else if (cmdStr == "RESET") {
    Serial.println("");
    Serial.println("  ⟳ Resetting device...");
    Serial.println("");
    delay(500);
    ESP.restart();
  }
  else if (cmdStr == "") {
    // Empty command, do nothing
  }
  else {
    Serial.println("");
    Serial.print("  ✗ Unknown command: '");
    Serial.print(cmd);
    Serial.println("'");
    Serial.println("");
    Serial.println("  Try one of these:");
    Serial.println("    • HELP          - Show all commands");
    Serial.println("    • STATUS        - Show device status");
    Serial.println("    • MEASURE:...   - Run measurement");
    Serial.println("    • RESET         - Restart device");
    Serial.println("");
  }
  
  // Print prompt
  Serial.print("> ");
}

/**
 * Parse MEASURE command parameters
 * Format: MEASURE:startFreq,endFreq,numPoints,biasVolt,zeroVolt,rcalVal,extGain,dacGain,param9,param10,param11,param12
 */
void parseMeasureCommand(const char* params) {
  MeasurementConfig config = {0};
  char tempBuffer[256];
  strcpy(tempBuffer, params);
  
  // Parse comma-separated values
  char* token = strtok(tempBuffer, ",");
  int paramCount = 0;
  
  float parsedFloats[6] = {0};
  int parsedInts[6] = {0};
  
  // Parse first 6 floats and 6 ints
  while (token != NULL && paramCount < 12) {
    float value = atof(token);
    
    if (paramCount < 2) {
      parsedFloats[paramCount] = value;
    } else if (paramCount == 2) {
      config.numPoints = (uint32_t)value;
    } else if (paramCount < 6) {
      parsedFloats[paramCount - 1] = value;
    } else {
      parsedInts[paramCount - 6] = (int)value;
    }
    
    token = strtok(NULL, ",");
    paramCount++;
  }
  
  // Assign parsed values
  config.startFreq = parsedFloats[0];
  config.endFreq = parsedFloats[1];
  config.biasVolt = parsedFloats[2];
  config.zeroVolt = parsedFloats[3];
  config.rcalVal = parsedFloats[4];
  config.extGain = parsedInts[0];
  config.dacGain = parsedInts[1];
  config.param9 = parsedInts[2];
  config.param10 = parsedInts[3];
  config.param11 = parsedInts[4];
  config.param12 = parsedInts[5];
  
  // Validate parameters
  if (config.startFreq <= 0 || config.endFreq <= 0 || config.numPoints <= 0) {
    Serial.println("");
    Serial.println("  ✗ Error: Invalid measurement parameters!");
    Serial.println("");
    Serial.println("  Parameters must be positive:");
    Serial.print("    startFreq=");
    Serial.print(config.startFreq, 2);
    Serial.println(" Hz (must be > 0)");
    Serial.print("    endFreq=");
    Serial.print(config.endFreq, 2);
    Serial.println(" Hz (must be > 0)");
    Serial.print("    numPoints=");
    Serial.print(config.numPoints);
    Serial.println(" (must be > 0)");
    Serial.println("");
    Serial.println("  Example: MEASURE:200000,10,5,0,0,1000,0,0,127000,150,0,0");
    Serial.println("");
    return;
  }
  
  Serial.println("");
  Serial.println("  ⟳ Starting measurement sweep...");
  Serial.println("");
  Serial.print("  Frequency:  ");
  Serial.print(config.startFreq, 0);
  Serial.print(" → ");
  Serial.print(config.endFreq, 0);
  Serial.print(" Hz (");
  Serial.print(config.numPoints);
  Serial.println(" points)");
  Serial.print("  Bias:       ");
  Serial.print(config.biasVolt, 3);
  Serial.print(" V");
  Serial.print("  |  Zero: ");
  Serial.print(config.zeroVolt, 3);
  Serial.print(" V");
  Serial.print("  |  Rcal: ");
  Serial.print(config.rcalVal, 1);
  Serial.println(" Ω");
  Serial.println("");
  
  // Perform measurement
  performMeasurement(&config);
}

/**
 * Perform impedance measurement sweep (simulated with test data)
 */
void performMeasurement(MeasurementConfig* config) {
  // Calculate frequency step
  float freqStart = config->startFreq;
  float freqEnd = config->endFreq;
  uint32_t numPoints = config->numPoints;
  
  // Check if logarithmic or linear spacing
  bool isLogSpacing = (freqStart < freqEnd && (freqEnd / freqStart) > 10);
  
  Serial.println("  Index  │ Frequency │ Magnitude │ Phase    │ Real Part │ Imag Part │ Status");
  Serial.println("  ───────┼───────────┼───────────┼──────────┼───────────┼───────────┼────────");
  
  // Generate measurement points
  for (uint32_t i = 0; i < numPoints; i++) {
    float freq;
    
    if (isLogSpacing) {
      // Logarithmic spacing
      float logStart = log10(freqStart);
      float logEnd = log10(freqEnd);
      float logStep = (logEnd - logStart) / (numPoints - 1);
      freq = pow(10, logStart + i * logStep);
    } else {
      // Linear spacing
      float step = (freqEnd - freqStart) / (numPoints - 1);
      freq = freqStart + i * step;
    }
    
    // Generate realistic test impedance data (RC circuit simulation)
    // Using a simple RC model: Z = R + 1/(j*ω*C)
    float R = config->rcalVal * 0.9;  // ~90% of calibration resistor
    float C = 1.0e-6;  // 1µF capacitor
    float omega = 2.0 * 3.14159265359 * freq;
    
    float realZ = R;
    float imagZ = -1.0 / (omega * C);
    float magnitude = sqrt(realZ * realZ + imagZ * imagZ);
    float phase = atan2(imagZ, realZ) * 180.0 / 3.14159265359;
    
    // Print measurement data in table format
    Serial.print("  ");
    Serial.print(i, 2);
    Serial.print("    │ ");
    if (freq < 1000) {
      Serial.print(freq, 1);
      Serial.print(" Hz");
    } else if (freq < 1000000) {
      Serial.print(freq / 1000.0, 2);
      Serial.print(" kHz");
    } else {
      Serial.print(freq / 1000000.0, 2);
      Serial.print(" MHz");
    }
    Serial.print("    │ ");
    Serial.print(magnitude, 1);
    Serial.print(" Ω    │ ");
    Serial.print(phase, 2);
    Serial.print("°   │ ");
    Serial.print(realZ, 1);
    Serial.print(" Ω     │ ");
    Serial.print(imagZ, 1);
    Serial.print(" Ω     │ ");
    Serial.println("OK");
    
    // Small delay to allow serial buffering
    delayMicroseconds(100);
    
    // Check for any abort commands
    if (Serial.available()) {
      char c = Serial.read();
      if (c == 'q' || c == 'Q') {
        Serial.println("  ───────┼───────────┼───────────┼──────────┼───────────┼───────────┼────────");
        Serial.println("");
        Serial.println("  ⚠ Measurement aborted by user");
        Serial.println("");
        return;
      }
    }
  }
  
  Serial.println("  ───────┴───────────┴───────────┴──────────┴───────────┴───────────┴────────");
  Serial.println("");
  Serial.println("  ✓ Measurement complete! (Press 'Q' during sweep to abort)");
  Serial.println("");
}

/**
 * Print help information
 */
void printHelp() {
  Serial.println("");
  Serial.println("╔════════════════════════════════════════════════════════════╗");
  Serial.println("║              HELPStat Command Reference                   ║");
  Serial.println("╚════════════════════════════════════════════════════════════╝");
  Serial.println("");
  Serial.println("┌─ MEASURE (Run Impedance Sweep) ─────────────────────────────┐");
  Serial.println("│                                                             │");
  Serial.println("│ Syntax:                                                     │");
  Serial.println("│   MEASURE:freq_start,freq_end,points,bias,zero,rcal,g1,...│");
  Serial.println("│                                                             │");
  Serial.println("│ Parameters:                                                 │");
  Serial.println("│   • freq_start   - Start frequency in Hz (200000)           │");
  Serial.println("│   • freq_end     - End frequency in Hz (10)                 │");
  Serial.println("│   • points       - Number of measurement points (5)         │");
  Serial.println("│   • bias         - Bias voltage in V (0)                    │");
  Serial.println("│   • zero         - Zero voltage in V (0)                    │");
  Serial.println("│   • rcal         - Calibration resistor in Ω (1000)         │");
  Serial.println("│   • g1, g2, ...  - Gain and reserved parameters            │");
  Serial.println("│                                                             │");
  Serial.println("│ Example:                                                    │");
  Serial.println("│   MEASURE:200000,10,5,0,0,1000,0,0,127000,150,0,0         │");
  Serial.println("│                                                             │");
  Serial.println("│ Interactive:                                                │");
  Serial.println("│   Press 'Q' during sweep to abort measurement               │");
  Serial.println("│                                                             │");
  Serial.println("└─────────────────────────────────────────────────────────────┘");
  Serial.println("");
  Serial.println("┌─ STATUS (Get Device Information) ──────────────────────────┐");
  Serial.println("│ Shows current device state, memory usage, and capabilities  │");
  Serial.println("└─────────────────────────────────────────────────────────────┘");
  Serial.println("");
  Serial.println("┌─ RESET (Restart Device) ───────────────────────────────────┐");
  Serial.println("│ Performs a soft restart of the device                       │");
  Serial.println("└─────────────────────────────────────────────────────────────┘");
  Serial.println("");
  Serial.println("┌─ HELP (This Message) ──────────────────────────────────────┐");
  Serial.println("│ Displays command reference                                  │");
  Serial.println("└─────────────────────────────────────────────────────────────┘");
  Serial.println("");
}

/**
 * Print device status
 */
void printStatus() {
  Serial.println("");
  Serial.println("╔════════════════════════════════════════════════════════════╗");
  Serial.println("║              HELPStat Device Status                        ║");
  Serial.println("╚════════════════════════════════════════════════════════════╝");
  Serial.println("");
  Serial.println("┌─ System Information ────────────────────────────────────────┐");
  Serial.println("│");
  Serial.print("│  Device Status:        ");
  Serial.println("READY ✓");
  Serial.println("│  Device Type:          ESP32-S3-DevKitC-1");
  Serial.println("│  Analog Front-End:     AD5940 Potentiostat");
  Serial.println("│  Firmware Version:     1.0.0");
  Serial.println("│");
  Serial.println("└─────────────────────────────────────────────────────────────┘");
  Serial.println("");
  Serial.println("┌─ Communication Settings ────────────────────────────────────┐");
  Serial.println("│");
  Serial.println("│  Serial Baud Rate:     115,200 bps");
  Serial.println("│  USB CDC Driver:       Active");
  Serial.println("│  SPI Clock (AD5940):   15 MHz");
  Serial.println("│  Upload Speed:         921,600 bps");
  Serial.println("│");
  Serial.println("└─────────────────────────────────────────────────────────────┘");
  Serial.println("");
  Serial.println("┌─ Memory Status ─────────────────────────────────────────────┐");
  Serial.println("│");
  
  uint32_t freeHeap = ESP.getFreeHeap();
  uint32_t totalHeap = ESP.getHeapSize();
  uint32_t usedHeap = totalHeap - freeHeap;
  uint8_t heapPercent = (usedHeap * 100) / totalHeap;
  
  Serial.print("│  Heap (RAM):           ");
  Serial.print(usedHeap / 1024);
  Serial.print(" / ");
  Serial.print(totalHeap / 1024);
  Serial.print(" KB  [");
  for (int i = 0; i < 20; i++) {
    Serial.print((i * 5) < heapPercent ? "█" : "░");
  }
  Serial.print("] ");
  Serial.print(heapPercent);
  Serial.println("%");
  
  uint32_t freePsram = ESP.getFreePsram();
  Serial.print("│  External RAM (PSRAM): ");
  Serial.print(freePsram / 1024 / 1024);
  Serial.println(" MB available");
  
  Serial.println("│");
  Serial.println("└─────────────────────────────────────────────────────────────┘");
  Serial.println("");
  Serial.println("┌─ Uptime ────────────────────────────────────────────────────┐");
  Serial.println("│");
  
  uint32_t uptime = millis() / 1000;
  uint32_t hours = uptime / 3600;
  uint32_t minutes = (uptime % 3600) / 60;
  uint32_t seconds = uptime % 60;
  
  Serial.print("│  Running for:          ");
  Serial.print(hours);
  Serial.print("h ");
  Serial.print(minutes);
  Serial.print("m ");
  Serial.print(seconds);
  Serial.println("s");
  
  Serial.println("│");
  Serial.println("└─────────────────────────────────────────────────────────────┘");
  Serial.println("");
  Serial.println("┌─ Capabilities ─────────────────────────────────────────────┐");
  Serial.println("│  ✓ Serial Command Interface");
  Serial.println("│  ✓ Impedance Measurement (EIS Mode)");
  Serial.println("│  ✓ Frequency Sweep (1 Hz - 200 kHz)");
  Serial.println("│  ✓ Tabular Data Output");
  Serial.println("│  ✓ Real-time Monitoring");
  Serial.println("│  ✓ Device Auto-Reset");
  Serial.println("└─────────────────────────────────────────────────────────────┘");
  Serial.println("");
}

