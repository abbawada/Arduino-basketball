# Basketball Haptic Training System - Setup Guide

## Quick Setup for Immediate Building

### Prerequisites
- Arduino IDE installed
- ESP32 board support package
- USB cable for ESP32
- All hardware components ready

## Arduino IDE Setup (15 minutes)

### 1. Install ESP32 Board Support
```
1. Open Arduino IDE
2. Go to File → Preferences
3. Add this URL to "Additional Board Manager URLs":
   https://dl.espressif.com/dl/package_esp32_index.json
4. Go to Tools → Board → Boards Manager
5. Search "ESP32" and install "ESP32 by Espressif Systems"
```

### 2. Install Required Libraries
```
Go to Sketch → Include Library → Manage Libraries
Install these libraries:

REQUIRED LIBRARIES:
├── Adafruit BNO055 (by Adafruit)
├── Adafruit Unified Sensor (by Adafruit)  
├── MPU6050 (by Electronic Cats)
├── Adafruit DRV2605 Library (by Adafruit)
├── WiFi (built-in with ESP32)
└── BluetoothSerial (built-in with ESP32)

OPTIONAL (for advanced features):
├── SD (built-in)
├── SPI (built-in)
└── ArduinoJson (for data logging)
```

### 3. Board Configuration
```
Tools → Board → ESP32 Arduino → "ESP32 Dev Module"
Tools → Upload Speed → "921600"
Tools → CPU Frequency → "240MHz (WiFi/BT)"
Tools → Flash Frequency → "80MHz"
Tools → Flash Mode → "QIO"
Tools → Flash Size → "4MB (32Mb)"
Tools → Port → [Select your ESP32 port]
```

## Hardware Setup (30 minutes)

### 1. Basic Component Test
**Test ESP32 first:**
```cpp
// Upload this simple test to verify ESP32 works
void setup() {
  Serial.begin(115200);
  pinMode(2, OUTPUT);
}

void loop() {
  digitalWrite(2, HIGH);
  delay(500);
  digitalWrite(2, LOW);
  delay(500);
  Serial.println("ESP32 Working!");
}
```

### 2. Power System Setup
```
CRITICAL: Set up power distribution first!

1. Connect ESP32 3.3V pin to breadboard red rail
2. Connect ESP32 GND pin to breadboard blue rail
3. Verify 3.3V with multimeter
4. Test continuity on GND rail
```

### 3. Component Connection Order
```
Connect in this order for easier debugging:

1. BNO055 (main sensor)
   └── Test with sensor reading sketch
2. MPU6050 #1 (first motion sensor)  
   └── Test with MPU6050 example
3. DRV2605L + 1 LRA motor
   └── Test with haptic example
4. Add remaining MPU6050s
5. Add battery system last
```

## Software Upload & Testing

### 1. Upload Main Firmware
```
1. Open firmware/basketball_free_throw.ino
2. Select correct board and port
3. Click Upload (Ctrl+U)
4. Open Serial Monitor (115200 baud)
5. Look for "System initialized successfully!"
```

### 2. Initial Testing Protocol
```
Serial Monitor Output Should Show:
├── "Basketball Free Throw Haptic Training System"
├── "System initialized successfully!"
├── "Ready for basketball training!"
├── Sensor readings every 2 seconds
└── No error messages
```

### 3. Basic Function Tests
```
Test 1: Sensor Reading
├── Move BNO055 → Should see orientation changes
├── Move MPU6050 → Should see acceleration changes
└── All values should be reasonable (not 0 or NaN)

Test 2: Haptic Feedback
├── System should detect arm movement
├── LRA motor should vibrate
└── Different vibration patterns for different motions

Test 3: System States
├── Press mode button → Should cycle through modes
├── Press calibration button → Should start calibration
└── LED should indicate current state
```

## Calibration Process

### 1. Initial Calibration (Required)
```
1. Put system in STANDBY mode
2. Press Calibration button
3. System enters CALIBRATION mode (LED solid)
4. Take 10 successful free throws
5. System learns your optimal form
6. Returns to STANDBY when complete
```

### 2. Training Mode Usage
```
1. Press Mode button to enter TRAINING mode
2. Take practice free throws
3. Feel haptic feedback for form correction:
   ├── Strong buzz → Elbow angle needs correction
   ├── Light buzz → Wrist angle needs correction  
   └── Double click → Good form!
4. Press Shot button to record make/miss
```

## Troubleshooting Common Issues

### ESP32 Upload Issues
```
Problem: "Failed to connect to ESP32"
Solutions:
├── Hold BOOT button while clicking Upload
├── Try different USB cable
├── Check COM port selection
├── Install CP210x or CH340 drivers
└── Reduce upload speed to 115200
```

### Sensor Not Detected
```
Problem: "BNO055 not detected" or similar
Solutions:
├── Check 3.3V power (should be exactly 3.3V)
├── Verify GND connections
├── Check SDA/SCL wiring
├── Try different jumper wires
└── Use I2C scanner to find device addresses
```

### No Haptic Feedback
```
Problem: LRA motors don't vibrate
Solutions:
├── Check DRV2605L power connections
├── Verify EN pin connection (GPIO 25)
├── Test motor with multimeter
├── Check motor polarity
└── Verify DRV2605L I2C address (0x5A)
```

### Battery Issues
```
Problem: System doesn't power on with battery
Solutions:
├── Check battery voltage (should be > 3.2V)
├── Verify TP4056 connections
├── Test voltage regulator output
├── Check for short circuits
└── Ensure proper charging setup
```

## Advanced Configuration

### 1. Sensitivity Adjustment
```cpp
// In firmware, adjust these values:
#define ELBOW_ANGLE_TOLERANCE 5.0    // Increase for less sensitivity
#define WRIST_ANGLE_TOLERANCE 3.0    // Decrease for more sensitivity
#define SHOT_DURATION_MIN 1.2        // Minimum shot time
#define SHOT_DURATION_MAX 1.8        // Maximum shot time
```

### 2. Haptic Pattern Customization
```cpp
// Different vibration patterns:
drv.setWaveform(0, 1);   // Sharp click
drv.setWaveform(0, 10);  // Double click  
drv.setWaveform(0, 47);  // Strong buzz
drv.setWaveform(0, 52);  // Pulsing
```

### 3. Data Logging Setup
```cpp
// Enable SD card logging (optional):
#define ENABLE_SD_LOGGING true
#define SD_CS_PIN 5

// Log format: timestamp, elbow_angle, wrist_angle, success
```

## System Modes Explained

### STANDBY Mode
- LED: Slow blink
- Function: Sensor monitoring, ready state
- Controls: Mode button cycles modes

### CALIBRATION Mode  
- LED: Solid on
- Function: Learning optimal form from 10 shots
- Duration: Until 10 shots recorded

### TRAINING Mode
- LED: Solid on
- Function: Real-time haptic feedback
- Controls: Shot button records make/miss

### DATA_REVIEW Mode
- LED: Fast blink
- Function: Display shot statistics
- Duration: 5 seconds, then back to STANDBY

## Performance Optimization

### 1. Battery Life
```
Expected battery life: 2-4 hours continuous use
Optimization tips:
├── Reduce sensor sampling rate if needed
├── Use lower haptic intensity
├── Enable sleep mode during inactivity
└── Monitor battery voltage regularly
```

### 2. Sensor Accuracy
```
Best practices:
├── Mount sensors securely to arm
├── Ensure consistent sensor orientation
├── Calibrate before each session
├── Keep I2C wires short and secure
└── Avoid interference from metal objects
```

### 3. Haptic Effectiveness
```
Optimal settings:
├── Use appropriate motor frequencies (235Hz/25Hz)
├── Adjust intensity based on user preference
├── Time feedback with motion phases
├── Provide clear different patterns for different errors
└── Test feedback patterns with user
```

## Ready to Build!

Your system should now be fully functional for basketball free throw training. The key is to start simple, test each component individually, then build up to the full system.

**Remember:** The goal is to prove the concept first, then optimize and add features!