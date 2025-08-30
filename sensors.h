/*
 * Sensor Definitions for Haptic Feedback Arm Sleeve
 * Defines data structures and functions for motion tracking
 */

#ifndef SENSORS_H
#define SENSORS_H

#include <Arduino.h>

// Data Structures
struct Vector3D {
  float x, y, z;
};

struct MotionData {
  Vector3D accel;      // Acceleration in g
  Vector3D gyro;       // Angular velocity in degrees/sec
  float magnitude;     // Acceleration magnitude
  unsigned long timestamp;
};

struct ShotData {
  unsigned long timestamp;
  float peakAccel;
  unsigned long duration;
  float formScore;
  Vector3D startPosition;
  Vector3D endPosition;
  Vector3D trajectory[100]; // Store trajectory points
  int trajectoryLength;
};

struct CalibrationData {
  float avgPeakAccel;
  float avgDuration;
  float stdDevAccel;
  float stdDevDuration;
  Vector3D optimalTrajectory[50];
  int trajectoryLength;
  bool isValid;
};

// Sensor Configuration
#define MPU6050_ADDRESS 0x68
#define SAMPLE_RATE 100  // Hz
#define CALIBRATION_SAMPLES 10

// Motion Detection Thresholds
#define MOTION_THRESHOLD 5000
#define SHOT_DETECTION_THRESHOLD 15000
#define MOTION_TIMEOUT 1000  // ms

// Global Variables
extern CalibrationData calibrationData;
extern MotionData lastMotionData;

// Function Declarations
void initSensors();
bool calibrateSensors();
void readMotionData(MotionData* data);
bool detectShotStart(const MotionData* data);
bool detectShotEnd(const MotionData* data);
float calculateFormScore(const MotionData* data, const CalibrationData* cal);
void recordTrajectory(ShotData* shot);
void analyzeTrajectory(ShotData* shot);
void saveCalibrationData(const CalibrationData* data);
void loadCalibrationData(CalibrationData* data);
float calculateTrajectorySimilarity(const Vector3D* traj1, int len1, 
                                   const Vector3D* traj2, int len2);

// Utility Functions
float vectorMagnitude(const Vector3D* vec);
float vectorDistance(const Vector3D* vec1, const Vector3D* vec2);
void normalizeVector(Vector3D* vec);
float dotProduct(const Vector3D* vec1, const Vector3D* vec2);
void crossProduct(const Vector3D* vec1, const Vector3D* vec2, Vector3D* result);

// Filtering Functions
void applyLowPassFilter(MotionData* data, float alpha);
void applyKalmanFilter(MotionData* data);
void removeGravity(MotionData* data);

// Calibration Functions
void startCalibration();
void addCalibrationSample(const ShotData* shot);
void finishCalibration();
bool validateCalibration();

#endif // SENSORS_H