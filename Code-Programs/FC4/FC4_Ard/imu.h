#ifndef IMU_H
#define IMU_H

#include <Arduino.h>

// Public API - these are what other files will call
bool initIMU();
void calibrateIMU();
void readIMU(float &ax, float &ay, float &az, float &gx, float &gy, float &gz);
//void printIMU();  // Optional: for serial debug

#endif