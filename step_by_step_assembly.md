# Step-by-Step Assembly Guide for Basketball Free Throw Sleeve

## Your Components Checklist ✅

### ✅ **What You Have (Perfect Setup!)**
- [x] Adafruit BNO055 (better than MPU6050!)
- [x] 3x HiLetgo GY-521 MPU6050
- [x] 2x LRA motors 235Hz 1g
- [x] 2x LRA motors 25Hz 1g
- [x] Adafruit DRV2605L haptic controller
- [x] 3.7V Li-Po battery
- [x] 3x TP4056 Type-C charger modules
- [x] 3x ESP32 boards
- [x] Aegend arm sleeve
- [x] Dupont wires
- [x] Freenove starter kit

## Phase 1: Basic Setup (Start Here!)

### Step 1: Test Your Components (30 minutes)

**Why?** Better to find problems now than after assembly!

1. **Test ESP32**
   ```bash
   # Connect ESP32 to computer via USB
   # Open Arduino IDE
   # Upload basic blink sketch
   # Verify LED blinks
   ```

2. **Test BNO055**
   ```bash
   # Connect BNO055 to ESP32:
   # VIN → 3.3V
   # GND → GND  
   # SDA → GPIO 21
   # SCL → GPIO 22
   # Upload BNO055 test sketch
   # Check Serial Monitor for sensor readings
   ```

3. **Test One MPU6050**
   ```bash
   # Connect MPU6050 to ESP32:
   # VCC → 3.3V
   # GND → GND
   # SDA → GPIO 18
   # SCL → GPIO 19
   # Upload MPU6050 test sketch
   # Verify accelerometer/gyroscope data
   ```

4. **Test DRV2605L + One LRA Motor**
   ```bash
   # Connect DRV2605L:
   # VIN → 3.3V
   # GND → GND
   # SDA → GPIO 26
   # SCL → GPIO 27
   # Connect LRA motor to OUT+/OUT-
   # Upload haptic test sketch
   # Feel vibration!
   ```

### Step 2: Simple Basketball Motion Detection (1 hour)

**Goal:** Get basic motion detection working before adding complexity.

1. **Wire Basic Setup**
   ```
   ESP32 + BNO055 + One MPU6050 + DRV2605L + One LRA Motor
   ```

2. **Upload Basketball Firmware**
   - Use `firmware/basketball_free_throw.ino`
   - This is simplified version for testing

3. **Test Motion Detection**
   - Move your arm in shooting motion
   - Check Serial Monitor for motion detection
   - Verify haptic feedback triggers

## Phase 2: Full System Assembly (2-3 hours)

### Step 3: Wire All Sensors

**BNO055 (Main Sensor - Upper Arm)**
```
BNO055 → ESP32
├── VIN → 3.3V
├── GND → GND
├── SDA → GPIO 21
└── SCL → GPIO 22
```

**MPU6050 #1 (Upper Arm)**
```
MPU6050 #1 → ESP32
├── VCC → 3.3V
├── GND → GND
├── SDA → GPIO 18
├── SCL → GPIO 19
└── AD0 → GND (Address 0x68)
```

**MPU6050 #2 (Lower Arm)**
```
MPU6050 #2 → ESP32
├── VCC → 3.3V
├── GND → GND
├── SDA → GPIO 18
├── SCL → GPIO 19
└── AD0 → 3.3V (Address 0x69)
```

**MPU6050 #3 (Wrist)**
```
MPU6050 #3 → ESP32
├── VCC → 3.3V
├── GND → GND
├── SDA → GPIO 18
├── SCL → GPIO 19
└── AD0 → GPIO 23 (Address 0x6A)
```

### Step 4: Wire Haptic System

**DRV2605L Controller**
```
DRV2605L → ESP32
├── VIN → 3.3V
├── GND → GND
├── SDA → GPIO 26
├── SCL → GPIO 27
└── EN → GPIO 25
```

**LRA Motors**
```
LRA Motor 1 (235Hz - Upper Arm) → DRV2605L OUT+/OUT-
LRA Motor 2 (25Hz - Lower Arm) → DRV2605L OUT+/OUT-
LRA Motor 3 (235Hz - Wrist) → DRV2605L OUT+/OUT-
LRA Motor 4 (25Hz - Hand) → DRV2605L OUT+/OUT-
```

### Step 5: Power System

**Battery + Charger**
```
Li-Po Battery → TP4056
├── Battery + → TP4056 IN+
└── Battery - → TP4056 IN-

TP4056 → ESP32
├── OUT+ → 3.3V Regulator → ESP32 3.3V
└── OUT- → GND
```

### Step 6: User Interface

**Buttons**
```
Mode Button → ESP32
├── One side → GPIO 0
├── Other side → GND
└── 10kΩ resistor → 3.3V (pull-up)

Shot Button → ESP32
├── One side → GPIO 4
├── Other side → GND
└── 10kΩ resistor → 3.3V (pull-up)

Calibration Button → ESP32
├── One side → GPIO 5
├── Other side → GND
└── 10kΩ resistor → 3.3V (pull-up)
```

**Status LED**
```
LED → ESP32
├── Anode → GPIO 2 (via 220Ω resistor)
└── Cathode → GND
```

## 🔧 Critical Breadboard Layout

### **Your 63-Row Breadboard Layout**

**ESP32 Placement (Rows 1-33):**
- Rows 1-13: ESP32 board (blocked)
- Rows 14-33: Extension pins (3 holes free per side)

**Power Rails Setup:**
```
Left Side:  Red → 5V,     Blue → GND
Right Side: Red → 3.3V,   Blue → GND
CRITICAL: Connect ESP32 3.3V pin to Right Red Rail!
```

**Component Placement:**
```
Rows 35-40: DRV2605L (left side, near ESP32)
Rows 41-46: MPU6050 #2 (aligned horizontally)
Rows 48-53: MPU6050 #1 (left of BNO055)
Rows 55-60: BNO055 (bottom-right, clearly visible)
Rows 55-63: Battery + Charger (bottom-left)
```

**L-Shaped Header Pins:**
- **Point UPWARD** (not flat on breadboard)
- Better wire management
- Easier debugging
- Standard practice

## ⚡ Power Distribution (CRITICAL!)

**Step 1: Create 3.3V Power Rail**
```
ESP32 3.3V pin → Right-side Red Rail
ESP32 GND pin → Both Blue Rails
```

**Step 2: Connect All Components**
```
All VIN/VCC pins → Right-side Red Rail (3.3V)
All GND pins → Right-side Blue Rail (GND)
```

## 🎯 Testing Protocol

### **Phase 1 Success Criteria:**
- [ ] ESP32 connects to computer
- [ ] BNO055 provides sensor data
- [ ] MPU6050 provides motion data
- [ ] LRA motor vibrates when triggered

### **Phase 2 Success Criteria:**
- [ ] All sensors working simultaneously
- [ ] Haptic feedback responds to motion
- [ ] System detects basketball shot motion
- [ ] Battery powers system for 1+ hours

## 🏀 Basketball Calibration

1. **Setup**
   - Stand at free throw line
   - Wear sleeve properly
   - Ensure sensors aligned

2. **Calibration Shots**
   - Press calibration button
   - Take 10 successful free throws
   - System learns your form

3. **Test Training Mode**
   - Switch to training mode
   - Take practice shots
   - Feel haptic feedback

## 🔧 Troubleshooting

### **ESP32 Not Connecting**
- Try different USB cable
- Install ESP32 board support in Arduino IDE
- Check if driver needed for your OS

### **BNO055 Not Working**
- Check I2C connections (SDA/SCL)
- Verify power supply (3.3V)
- Check I2C address (should be 0x28)

### **MPU6050 Address Conflict**
- Set different AD0 pins for each MPU6050
- MPU6050 #1: AD0 → GND (Address 0x68)
- MPU6050 #2: AD0 → 3.3V (Address 0x69)
- MPU6050 #3: AD0 → GPIO 23 (Address 0x6A)

### **Haptic Motors Not Working**
- Check DRV2605L connections
- Verify motor polarity
- Test with multimeter
- Check DRV2605L enable pin

### **Battery Issues**
- Check battery voltage (should be 3.7V)
- Verify TP4056 connections
- Check voltage regulator output
- Ensure proper ground connections

## 🎯 Success Metrics

You have an excellent component selection and a focused use case. This system will definitely help improve basketball free throw form!

**Start with Phase 1, get basic motion detection working, then scale up!**