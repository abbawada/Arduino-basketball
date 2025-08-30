/*
 * Haptic Feedback Control for Arm Sleeve
 * Controls vibration motors for haptic feedback
 */

#ifndef HAPTIC_H
#define HAPTIC_H

#include <Arduino.h>

// Pin Definitions
#define HAPTIC_1_PIN 25  // Upper arm
#define HAPTIC_2_PIN 26  // Lower arm  
#define HAPTIC_3_PIN 27  // Wrist

// Haptic Feedback Patterns
enum HapticPattern {
  NONE,
  SINGLE_PULSE,
  DOUBLE_PULSE,
  TRIPLE_PULSE,
  CONTINUOUS,
  INCREASING,
  DECREASING,
  ALTERNATING,
  WAVE
};

// Feedback Intensity Levels
enum FeedbackIntensity {
  LIGHT = 64,
  MEDIUM = 128,
  STRONG = 255
};

// Feedback Zones
enum FeedbackZone {
  UPPER_ARM = 1,
  LOWER_ARM = 2,
  WRIST = 3,
  ALL_ZONES = 0
};

// Haptic Motor Control Structure
struct HapticMotor {
  int pin;
  int currentIntensity;
  bool isActive;
  unsigned long startTime;
  unsigned long duration;
  HapticPattern pattern;
};

// Global Variables
extern HapticMotor motors[3];
extern bool hapticSystemEnabled;

// Function Declarations
void initHapticSystem();
void triggerHapticFeedback(int pin, int intensity, unsigned long duration);
void triggerPatternFeedback(FeedbackZone zone, HapticPattern pattern, FeedbackIntensity intensity);
void updateHapticFeedback();
void stopAllHapticFeedback();
void setHapticIntensity(int pin, int intensity);
void enableHapticSystem(bool enable);

// Pattern Functions
void executeSinglePulse(HapticMotor* motor);
void executeDoublePulse(HapticMotor* motor);
void executeTriplePulse(HapticMotor* motor);
void executeContinuous(HapticMotor* motor);
void executeIncreasing(HapticMotor* motor);
void executeDecreasing(HapticMotor* motor);
void executeAlternating();
void executeWave();

// Form-Specific Feedback Functions
void feedbackPoorForm();
void feedbackModerateForm();
void feedbackGoodForm();
void feedbackTooSlow();
void feedbackTooFast();
void feedbackOffTrajectory();
void feedbackCorrectForm();

// Utility Functions
int getMotorIndex(int pin);
void setMotorPattern(int pin, HapticPattern pattern, int intensity, unsigned long duration);
bool isMotorActive(int pin);
void cleanupInactiveMotors();

// Safety Functions
void checkMotorTemperature();
void emergencyStop();
bool isSystemOverheating();

#endif // HAPTIC_H