# Basketball Free Throw Haptic Sleeve - Specific Wiring Guide

## Your Component Layout

### ESP32 Pin Assignments (Primary ESP32)

```
ESP32 Development Board (Primary)
├── 3.3V Power Supply
├── GND (Ground)
├── GPIO Pins:
│   ├── GPIO 21 → I2C SDA (BNO055)
│   ├── GPIO 22 → I2C SCL (BNO055)
│   ├── GPIO 18 → I2C SDA (MPU6050 #1 - Upper Arm)
│   ├── GPIO 19 → I2C SCL (MPU6050 #1 - Upper Arm)
│   ├── GPIO 25 → DRV2605L Enable (Haptic Controller)
│   ├── GPIO 26 → DRV2605L SDA
│   ├── GPIO 27 → DRV2605L SCL
│   ├── GPIO 34 → Battery Voltage Monitor
│   ├── GPIO 0  → Mode Selection Button
│   ├── GPIO 2  → Status LED
│   ├── GPIO 4  → Shot Success Button (Made/Missed)
│   └── GPIO 5  → Calibration Button
```

### Component-Specific Connections

#### 1. BNO055 (Main Motion Sensor - Upper Arm)
```
BNO055 (Adafruit)
├── VIN → 3.3V
├── GND → GND
├── SDA → GPIO 21
├── SCL → GPIO 22
└── ADR → GND (I2C Address 0x28)
```

#### 2. MPU6050 Sensors (3x - Detailed Arm Tracking)
```
MPU6050 #1 (Upper Arm - Shoulder to Elbow)
├── VCC → 3.3V
├── GND → GND
├── SDA → GPIO 18
├── SCL → GPIO 19
└── AD0 → GND (I2C Address 0x68)

MPU6050 #2 (Lower Arm - Elbow to Wrist)
├── VCC → 3.3V
├── GND → GND
├── SDA → GPIO 18 (same I2C bus)
├── SCL → GPIO 19 (same I2C bus)
└── AD0 → 3.3V (I2C Address 0x69)

MPU6050 #3 (Wrist/Hand)
├── VCC → 3.3V
├── GND → GND
├── SDA → GPIO 18 (same I2C bus)
├── SCL → GPIO 19 (same I2C bus)
└── AD0 → GPIO 23 (I2C Address 0x6A)
```

#### 3. DRV2605L Haptic Controller
```
DRV2605L (Adafruit)
├── VIN → 3.3V
├── GND → GND
├── SDA → GPIO 26
├── SCL → GPIO 27
├── EN → GPIO 25
└── OUT+/- → LRA Motors
```

#### 4. LRA Motor Connections
```
LRA Motor 1 (235Hz - Upper Arm Feedback)
├── Positive → DRV2605L OUT+
└── Negative → DRV2605L OUT-

LRA Motor 2 (25Hz - Lower Arm Feedback)
├── Positive → DRV2605L OUT+ (via multiplexer)
└── Negative → DRV2605L OUT- (via multiplexer)

LRA Motor 3 (235Hz - Wrist Feedback)
├── Positive → DRV2605L OUT+ (via multiplexer)
└── Negative → DRV2605L OUT- (via multiplexer)

LRA Motor 4 (25Hz - Hand Feedback)
├── Positive → DRV2605L OUT+ (via multiplexer)
└── Negative → DRV2605L OUT- (via multiplexer)
```

#### 5. Power System
```
3.7V Li-Po Battery
├── Positive → TP4056 IN+
└── Negative → TP4056 IN-

TP4056 Charger Module
├── IN+ → Battery Positive
├── IN- → Battery Negative
├── OUT+ → 3.3V Regulator Input
├── OUT- → GND
└── USB → Type-C Charging Port

3.3V Voltage Regulator
├── Input → TP4056 OUT+
├── GND → TP4056 OUT-
└── Output → ESP32 3.3V Rail
```

#### 6. User Interface Components
```
Mode Selection Button
├── One Terminal → GPIO 0
├── Other Terminal → GND
└── Pull-up Resistor → 10kΩ to 3.3V

Shot Success Button
├── One Terminal → GPIO 4
├── Other Terminal → GND
└── Pull-up Resistor → 10kΩ to 3.3V

Calibration Button
├── One Terminal → GPIO 5
├── Other Terminal → GND
└── Pull-up Resistor → 10kΩ to 3.3V

Status LED
├── Anode → GPIO 2 (via 220Ω resistor)
└── Cathode → GND

Battery Monitor
├── Battery+ → Voltage Divider → GPIO 34
└── Battery- → GND
```

## Breadboard Wiring Layout

### Power Rails Setup
```
Left Side Power Rails:
├── Red Rail → 5V (for charger input)
└── Blue Rail → GND

Right Side Power Rails:
├── Red Rail → 3.3V (MAIN POWER DISTRIBUTION)
└── Blue Rail → GND

CRITICAL CONNECTION:
ESP32 3.3V pin → Right Red Rail (creates 3.3V distribution)
ESP32 GND pin → Both Blue Rails (common ground)
```

### Component Placement & Wiring
```
Rows 35-40: DRV2605L Haptic Controller
├── VIN → Right Red Rail (3.3V)
├── GND → Right Blue Rail
├── SDA → ESP32 GPIO 26 (row ~25)
├── SCL → ESP32 GPIO 27 (row ~26)
└── EN → ESP32 GPIO 25 (row ~24)

Rows 41-46: MPU6050 #2 (Lower Arm)
├── VCC → Right Red Rail (3.3V)
├── GND → Right Blue Rail
├── SDA → ESP32 GPIO 18 (row ~20)
├── SCL → ESP32 GPIO 19 (row ~21)
└── AD0 → Right Red Rail (3.3V for address 0x69)

Rows 48-53: MPU6050 #1 (Upper Arm)
├── VCC → Right Red Rail (3.3V)
├── GND → Right Blue Rail
├── SDA → ESP32 GPIO 18 (row ~20)
├── SCL → ESP32 GPIO 19 (row ~21)
└── AD0 → Right Blue Rail (GND for address 0x68)

Rows 55-60: BNO055 (Main Sensor)
├── VIN → Right Red Rail (3.3V)
├── GND → Right Blue Rail
├── SDA → ESP32 GPIO 21 (row ~16)
├── SCL → ESP32 GPIO 22 (row ~17)
└── ADR → Right Blue Rail (GND for address 0x28)

Rows 55-63: Battery & Charger System
├── Battery+ → TP4056 IN+
├── Battery- → TP4056 IN-
├── TP4056 OUT+ → Left Red Rail (5V)
├── TP4056 OUT- → Left Blue Rail (GND)
└── Voltage Monitor → ESP32 GPIO 34 (row ~28)
```

## Wire Color Coding (Recommended)
```
Power Connections:
├── Red Wires → 3.3V connections
├── Black Wires → GND connections
├── Orange Wires → 5V connections (charger)

Data Connections:
├── Yellow Wires → SDA (I2C data)
├── Green Wires → SCL (I2C clock)
├── Blue Wires → GPIO connections
├── Purple Wires → Enable/Control pins

Special Connections:
├── White Wires → Analog inputs
└── Gray Wires → Button connections
```

## I2C Bus Organization
```
I2C Bus 0 (Hardware) - BNO055 Only
├── SDA → GPIO 21
├── SCL → GPIO 22
└── Device: BNO055 (Address 0x28)

I2C Bus 1 (Hardware) - All MPU6050s
├── SDA → GPIO 18
├── SCL → GPIO 19
├── Device 1: MPU6050 #1 (Address 0x68)
├── Device 2: MPU6050 #2 (Address 0x69)
└── Device 3: MPU6050 #3 (Address 0x6A)

I2C Bus 2 (Hardware) - DRV2605L Only
├── SDA → GPIO 26
├── SCL → GPIO 27
└── Device: DRV2605L (Address 0x5A)
```

## Critical Wiring Notes

### ⚠️ **POWER WARNINGS**
- **NEVER connect 5V to sensors** - they are 3.3V only!
- **Always use ESP32 3.3V pin** for sensor power
- **Connect ESP32 3.3V to breadboard power rail** first
- **Double-check polarity** on battery connections

### ✅ **Best Practices**
- **Keep I2C wires short** (< 6 inches ideal)
- **Use different I2C buses** to prevent conflicts
- **Test each component individually** before connecting all
- **Use multimeter** to verify power rail voltages
- **Label your wires** with tape for debugging

### 🔧 **Troubleshooting Quick Checks**
1. **3.3V on power rail?** Should measure 3.3V exactly
2. **GND continuity?** All GND points should be 0Ω resistance
3. **I2C pullups?** Most breakout boards have them built-in
4. **Correct addresses?** Use I2C scanner to verify
5. **Secure connections?** Wiggle test all jumper wires

This wiring guide will get your basketball haptic training system working perfectly!
