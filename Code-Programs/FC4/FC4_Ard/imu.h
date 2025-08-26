#ifndef IMU_H
#define IMU_H

#include <Wire.h>

// MPU-6050 I2C address
const int MPU_ADDR = 0x68;

// Raw variables
extern int16_t accX, accY, accZ;
extern int16_t gyroX, gyroY, gyroZ;

// Offsets
extern float accX_offset, accY_offset, accZ_offset;
extern float gyroX_offset, gyroY_offset, gyroZ_offset;

// Scale factors
extern float ACCEL_SCALE;
extern float GYRO_SCALE;

// Current ranges
extern int accelRange; // 0=±2g,1=±4g,2=±8g,3=±16g
extern int gyroRange;  // 0=±250,1=±500,2=±1000,3=±2000 °/s

bool initIMU();
void calibrateIMU();
void readIMU(float &ax, float &ay, float &az, float &gx, float &gy, float &gz);

// Range setters
void setAccelRange(int range);
void setGyroRange(int range);

#endif
