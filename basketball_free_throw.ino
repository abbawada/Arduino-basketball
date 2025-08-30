/*
 * Basketball Free Throw Haptic Training System
 * ESP32-based motion tracking and haptic feedback for free throw improvement
 * 
 * Components:
 * - Adafruit BNO055 (main motion sensor)
 * - 3x MPU6050 (detailed arm tracking)
 * - DRV2605L (haptic motor controller)
 * - 4x LRA motors (235Hz and 25Hz)
 * - TP4056 charger module
 * - Li-Po battery
 */

#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BNO055.h>
#include <MPU6050.h>
#include <Adafruit_DRV2605.h>
#include <WiFi.h>
#include <BluetoothSerial.h>
#include <SPI.h>
#include <SD.h>

// Pin Definitions
#define BNO055_SDA 21
#define BNO055_SCL 22
#define MPU1_SDA 18
#define MPU1_SCL 19
#define DRV_ENABLE 25
#define DRV_SDA 26
#define DRV_SCL 27
#define BATTERY_PIN 34
#define MODE_BUTTON 0
#define SHOT_BUTTON 4
#define CALIB_BUTTON 5
#define LED_PIN 2

// System States
enum SystemState {
  STANDBY,
  CALIBRATION,
  TRAINING,
  DATA_REVIEW
};

// Basketball-specific data structures
struct FreeThrowData {
  float elbowAngle;
  float wristAngle;
  float releaseTiming;
  float followThrough;
  float bodyBalance;
  bool wasSuccessful;
  unsigned long shotDuration;
  float peakAcceleration;
};

struct CalibrationData {
  float avgElbowAngle;
  float avgWristAngle;
  float avgReleaseTiming;
  float avgFollowThrough;
  float avgBodyBalance;
  float stdDevElbow;
  float stdDevWrist;
  float stdDevTiming;
  bool isValid;
};

// Global Variables
SystemState currentState = STANDBY;
Adafruit_BNO055 bno = Adafruit_BNO055(55);
MPU6050 mpu1, mpu2, mpu3;
Adafruit_DRV2605 drv;
BluetoothSerial SerialBT;

FreeThrowData currentShot;
CalibrationData calibrationData;
bool isCalibrated = false;
int shotCount = 0;
unsigned long lastShotTime = 0;

// Basketball-specific thresholds
#define ELBOW_ANGLE_TOLERANCE 5.0  // degrees
#define WRIST_ANGLE_TOLERANCE 3.0  // degrees
#define RELEASE_TIMING_TOLERANCE 0.1  // seconds
#define SHOT_DURATION_MIN 1.2  // seconds
#define SHOT_DURATION_MAX 1.8  // seconds

void setup() {
  Serial.begin(115200);
  Serial.println("Basketball Free Throw Haptic Training System");
  
  // Initialize pins
  pinMode(MODE_BUTTON, INPUT_PULLUP);
  pinMode(SHOT_BUTTON, INPUT_PULLUP);
  pinMode(CALIB_BUTTON, INPUT_PULLUP);
  pinMode(LED_PIN, OUTPUT);
  pinMode(DRV_ENABLE, OUTPUT);
  
  // Initialize I2C for BNO055
  Wire.begin(BNO055_SDA, BNO055_SCL);
  
  // Initialize I2C for MPU6050s
  Wire1.begin(MPU1_SDA, MPU1_SCL);
  
  // Initialize I2C for DRV2605L
  Wire2.begin(DRV_SDA, DRV_SCL);
  
  // Initialize BNO055
  if (!bno.begin()) {
    Serial.println("BNO055 not detected. Check wiring!");
    while (1);
  }
  
  // Initialize MPU6050s with different addresses
  mpu1.initialize();
  mpu2.initialize();
  mpu3.initialize();
  
  // Set MPU6050 addresses
  mpu1.setDeviceID(0x68);  // AD0 = GND
  mpu2.setDeviceID(0x69);  // AD0 = 3.3V
  mpu3.setDeviceID(0x6A);  // AD0 = GPIO 23
  
  // Initialize DRV2605L
  digitalWrite(DRV_ENABLE, HIGH);
  if (!drv.begin()) {
    Serial.println("DRV2605L not found. Check wiring!");
    while (1);
  }
  
  drv.selectLibrary(1);  // ERM library
  drv.setMode(DRV2605_MODE_INTTRIG);
  
  // Initialize Bluetooth
  SerialBT.begin("Basketball_Trainer");
  
  Serial.println("System initialized successfully!");
  Serial.println("Ready for basketball training!");
  
  // Flash LED to indicate ready
  for (int i = 0; i < 3; i++) {
    digitalWrite(LED_PIN, HIGH);
    delay(200);
    digitalWrite(LED_PIN, LOW);
    delay(200);
  }
}

void loop() {
  // Check button states
  checkButtons();
  
  // Update system based on current state
  switch (currentState) {
    case STANDBY:
      handleStandby();
      break;
    case CALIBRATION:
      handleCalibration();
      break;
    case TRAINING:
      handleTraining();
      break;
    case DATA_REVIEW:
      handleDataReview();
      break;
  }
  
  // Monitor battery
  monitorBattery();
  
  delay(10);  // Small delay for stability
}

void checkButtons() {
  static unsigned long lastButtonCheck = 0;
  if (millis() - lastButtonCheck < 50) return;  // Debounce
  
  if (digitalRead(MODE_BUTTON) == LOW) {
    cycleSystemState();
    delay(300);  // Prevent multiple triggers
  }
  
  if (digitalRead(CALIB_BUTTON) == LOW) {
    if (currentState == STANDBY) {
      currentState = CALIBRATION;
      Serial.println("Starting calibration mode...");
    }
    delay(300);
  }
  
  if (digitalRead(SHOT_BUTTON) == LOW) {
    if (currentState == TRAINING) {
      recordShotOutcome();
    }
    delay(300);
  }
  
  lastButtonCheck = millis();
}

void handleStandby() {
  // Slow blink LED to indicate standby
  static unsigned long lastBlink = 0;
  if (millis() - lastBlink > 1000) {
    digitalWrite(LED_PIN, !digitalRead(LED_PIN));
    lastBlink = millis();
  }
  
  // Read sensors for monitoring
  readAllSensors();
  
  // Print sensor data every 2 seconds
  static unsigned long lastPrint = 0;
  if (millis() - lastPrint > 2000) {
    printSensorData();
    lastPrint = millis();
  }
}

void handleCalibration() {
  Serial.println("Calibration Mode: Take 10 successful free throws");
  digitalWrite(LED_PIN, HIGH);  // Solid LED during calibration
  
  // Collect calibration data
  static int calibrationShots = 0;
  static float elbowSum = 0, wristSum = 0, timingSum = 0;
  
  if (detectShotMotion()) {
    calibrationShots++;
    
    // Collect data for this shot
    FreeThrowData shotData = analyzeShotForm();
    elbowSum += shotData.elbowAngle;
    wristSum += shotData.wristAngle;
    timingSum += shotData.releaseTiming;
    
    Serial.print("Calibration shot ");
    Serial.print(calibrationShots);
    Serial.println(" recorded");
    
    // Provide haptic feedback
    drv.setWaveform(0, 1);  // Strong click
    drv.go();
    
    if (calibrationShots >= 10) {
      // Calculate averages
      calibrationData.avgElbowAngle = elbowSum / 10.0;
      calibrationData.avgWristAngle = wristSum / 10.0;
      calibrationData.avgReleaseTiming = timingSum / 10.0;
      calibrationData.isValid = true;
      isCalibrated = true;
      
      Serial.println("Calibration complete!");
      Serial.print("Average elbow angle: ");
      Serial.println(calibrationData.avgElbowAngle);
      
      currentState = STANDBY;
      calibrationShots = 0;
      elbowSum = wristSum = timingSum = 0;
    }
    
    delay(2000);  // Wait between shots
  }
}

void handleTraining() {
  digitalWrite(LED_PIN, HIGH);  // Solid LED during training
  
  if (!isCalibrated) {
    Serial.println("Please calibrate first!");
    currentState = STANDBY;
    return;
  }
  
  if (detectShotMotion()) {
    FreeThrowData shotData = analyzeShotForm();
    provideHapticFeedback(shotData);
    
    shotCount++;
    lastShotTime = millis();
    
    Serial.println("Shot analyzed - check form feedback");
  }
}

void handleDataReview() {
  Serial.println("Data Review Mode");
  // Print recent shot data
  Serial.print("Total shots: ");
  Serial.println(shotCount);
  
  if (isCalibrated) {
    Serial.print("Calibrated elbow angle: ");
    Serial.println(calibrationData.avgElbowAngle);
  }
  
  delay(5000);
  currentState = STANDBY;
}

bool detectShotMotion() {
  // Read BNO055 for main motion detection
  sensors_event_t event;
  bno.getEvent(&event);
  
  // Simple motion detection based on acceleration
  static float lastAccel = 0;
  float currentAccel = sqrt(event.acceleration.x * event.acceleration.x + 
                           event.acceleration.y * event.acceleration.y + 
                           event.acceleration.z * event.acceleration.z);
  
  bool motionDetected = abs(currentAccel - lastAccel) > 5.0;  // Threshold for shot motion
  lastAccel = currentAccel;
  
  return motionDetected;
}

FreeThrowData analyzeShotForm() {
  FreeThrowData shotData;
  
  // Read all sensors
  sensors_event_t bnoEvent;
  bno.getEvent(&bnoEvent);
  
  int16_t ax1, ay1, az1, gx1, gy1, gz1;
  mpu1.getMotion6(&ax1, &ay1, &az1, &gx1, &gy1, &gz1);
  
  // Calculate elbow angle (simplified)
  shotData.elbowAngle = atan2(ay1, ax1) * 180 / PI;
  
  // Calculate wrist angle (simplified)
  shotData.wristAngle = atan2(bnoEvent.acceleration.y, bnoEvent.acceleration.x) * 180 / PI;
  
  // Estimate release timing
  shotData.releaseTiming = millis() / 1000.0;
  
  // Calculate follow through
  shotData.followThrough = abs(gx1) / 1000.0;
  
  return shotData;
}

void provideHapticFeedback(FreeThrowData shotData) {
  // Compare with calibration data
  float elbowError = abs(shotData.elbowAngle - calibrationData.avgElbowAngle);
  float wristError = abs(shotData.wristAngle - calibrationData.avgWristAngle);
  
  // Provide feedback based on errors
  if (elbowError > ELBOW_ANGLE_TOLERANCE) {
    // Strong buzz for elbow correction
    drv.setWaveform(0, 47);  // Strong buzz
    drv.go();
    Serial.println("Haptic: Elbow angle correction needed");
  } else if (wristError > WRIST_ANGLE_TOLERANCE) {
    // Light buzz for wrist correction
    drv.setWaveform(0, 1);   // Light click
    drv.go();
    Serial.println("Haptic: Wrist angle correction needed");
  } else {
    // Success pattern
    drv.setWaveform(0, 10);  // Double click
    drv.go();
    Serial.println("Haptic: Good form!");
  }
}

void readAllSensors() {
  // Read BNO055
  sensors_event_t event;
  bno.getEvent(&event);
  
  // Read MPU6050s
  int16_t ax1, ay1, az1, gx1, gy1, gz1;
  mpu1.getMotion6(&ax1, &ay1, &az1, &gx1, &gy1, &gz1);
}

void printSensorData() {
  sensors_event_t event;
  bno.getEvent(&event);
  
  Serial.print("BNO055 - X: ");
  Serial.print(event.orientation.x);
  Serial.print(" Y: ");
  Serial.print(event.orientation.y);
  Serial.print(" Z: ");
  Serial.println(event.orientation.z);
  
  int16_t ax1, ay1, az1, gx1, gy1, gz1;
  mpu1.getMotion6(&ax1, &ay1, &az1, &gx1, &gy1, &gz1);
  
  Serial.print("MPU6050 - AX: ");
  Serial.print(ax1);
  Serial.print(" AY: ");
  Serial.print(ay1);
  Serial.print(" AZ: ");
  Serial.println(az1);
}

void cycleSystemState() {
  switch (currentState) {
    case STANDBY:
      currentState = TRAINING;
      Serial.println("Switched to Training Mode");
      break;
    case TRAINING:
      currentState = DATA_REVIEW;
      Serial.println("Switched to Data Review Mode");
      break;
    case DATA_REVIEW:
      currentState = STANDBY;
      Serial.println("Switched to Standby Mode");
      break;
    case CALIBRATION:
      // Can't switch out of calibration mode with mode button
      break;
  }
}

void recordShotOutcome() {
  Serial.println("Shot outcome recorded");
  // Simple feedback
  drv.setWaveform(0, 1);
  drv.go();
}

void monitorBattery() {
  static unsigned long lastBatteryCheck = 0;
  if (millis() - lastBatteryCheck > 30000) {  // Check every 30 seconds
    int batteryReading = analogRead(BATTERY_PIN);
    float voltage = batteryReading * (3.3 / 4095.0) * 2;  // Voltage divider
    
    Serial.print("Battery voltage: ");
    Serial.println(voltage);
    
    if (voltage < 3.2) {
      Serial.println("Low battery warning!");
      // Flash LED rapidly
      for (int i = 0; i < 5; i++) {
        digitalWrite(LED_PIN, HIGH);
        delay(100);
        digitalWrite(LED_PIN, LOW);
        delay(100);
      }
    }
    
    lastBatteryCheck = millis();
  }
}
