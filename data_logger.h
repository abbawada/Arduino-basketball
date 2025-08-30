/*
 * Data Logger for Haptic Feedback Arm Sleeve
 * Handles data logging, storage, and analysis
 */

#ifndef DATA_LOGGER_H
#define DATA_LOGGER_H

#include <Arduino.h>
#include <SD.h>
#include <SPI.h>
#include "sensors.h"

// File Definitions
#define SHOT_DATA_FILE "/shot_data.csv"
#define CALIBRATION_FILE "/calibration.dat"
#define PERFORMANCE_FILE "/performance.csv"
#define CONFIG_FILE "/config.txt"

// Data Logging Configuration
#define MAX_SHOTS_PER_SESSION 100
#define MAX_TRAJECTORY_POINTS 100
#define LOG_BUFFER_SIZE 512

// Performance Metrics
struct PerformanceMetrics {
  int totalShots;
  float averageScore;
  float bestScore;
  float consistencyScore;
  unsigned long totalTrainingTime;
  int improvementTrend;
  float accuracyRate;
};

// Shot Statistics
struct ShotStatistics {
  unsigned long timestamp;
  float formScore;
  float peakAccel;
  unsigned long duration;
  bool wasSuccessful;
  char notes[50];
};

// Data Logging Modes
enum LoggingMode {
  LOG_NONE,
  LOG_SD_ONLY,
  LOG_CLOUD_ONLY,
  LOG_BOTH
};

// Global Variables
extern LoggingMode currentLoggingMode;
extern PerformanceMetrics performanceMetrics;
extern bool sdCardAvailable;

// Function Declarations
void initDataLogger();
bool initSDCard();
void logShotData(const ShotData* shot);
void logCalibrationData(const CalibrationData* data);
void logPerformanceMetrics(const PerformanceMetrics* metrics);
void saveShotToSD(const ShotData* shot);
void saveCalibrationToSD(const CalibrationData* data);
void loadCalibrationFromSD(CalibrationData* data);
void exportDataToCSV();
void clearAllData();

// Performance Analysis Functions
void calculatePerformanceMetrics();
float getAverageScore();
float getBestScore();
float getConsistencyScore();
int getImprovementTrend();
void updatePerformanceMetrics(const ShotData* shot);

// Data Export Functions
void exportShotDataToCSV();
void exportPerformanceToCSV();
void sendDataToCloud();
void backupDataToCloud();

// Utility Functions
bool isSDCardMounted();
unsigned long getFreeSDSpace();
void formatSDCard();
void listFilesOnSD();
void deleteOldData(int daysOld);

// Cloud Integration Functions
void initCloudConnection();
bool uploadToCloud(const char* filename);
bool downloadFromCloud(const char* filename);
void syncDataWithCloud();

// Data Validation Functions
bool validateShotData(const ShotData* shot);
bool validateCalibrationData(const CalibrationData* data);
void repairCorruptedData();

// Memory Management
void optimizeStorage();
void compressOldData();
void cleanupTempFiles();

#endif // DATA_LOGGER_H