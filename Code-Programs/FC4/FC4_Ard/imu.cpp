#include "imu.h"

// Globals
int16_t accX, accY, accZ;
int16_t gyroX, gyroY, gyroZ;

float accX_offset = 0, accY_offset = 0, accZ_offset = 0;
float gyroX_offset = 0, gyroY_offset = 0, gyroZ_offset = 0;

float ACCEL_SCALE = 16384.0; // default ±2g
float GYRO_SCALE  = 131.0;   // default ±250°/s

int accelRange = 0;
int gyroRange  = 0;

bool initIMU() {
  Wire.begin();

  // Wake up MPU6050
  Wire.beginTransmission(MPU_ADDR);
  Wire.write(0x6B);
  Wire.write(0);
  Wire.endTransmission(true);

  // Default ranges
  setAccelRange(3); // ±2g
  setGyroRange(3);  // ±250°/s

  // Test connection
  Wire.beginTransmission(MPU_ADDR);
  Wire.write(0x75); // WHO_AM_I
  Wire.endTransmission(false);
  Wire.requestFrom(MPU_ADDR, 1, true);
  return (Wire.read() == 0x68);
}

void calibrateIMU() {
  const int samples = 500;
  long accX_sum = 0, accY_sum = 0, accZ_sum = 0;
  long gyroX_sum = 0, gyroY_sum = 0, gyroZ_sum = 0;

  for (int i = 0; i < samples; i++) {
    // Read accelerometer
    Wire.beginTransmission(MPU_ADDR);
    Wire.write(0x3B);
    Wire.endTransmission(false);
    Wire.requestFrom(MPU_ADDR, 6, true);
    accX = Wire.read() << 8 | Wire.read();
    accY = Wire.read() << 8 | Wire.read();
    accZ = Wire.read() << 8 | Wire.read();

    // Read gyro
    Wire.beginTransmission(MPU_ADDR);
    Wire.write(0x43);
    Wire.endTransmission(false);
    Wire.requestFrom(MPU_ADDR, 6, true);
    gyroX = Wire.read() << 8 | Wire.read();
    gyroY = Wire.read() << 8 | Wire.read();
    gyroZ = Wire.read() << 8 | Wire.read();

    accX_sum += accX;
    accY_sum += accY;
    accZ_sum += accZ;
    gyroX_sum += gyroX;
    gyroY_sum += gyroY;
    gyroZ_sum += gyroZ;

    
  }

  accX_offset = accX_sum / (float)samples;
  accY_offset = accY_sum / (float)samples;
  accZ_offset = (accZ_sum / (float)samples) - ACCEL_SCALE; // remove 1g
  gyroX_offset = gyroX_sum / (float)samples;
  gyroY_offset = gyroY_sum / (float)samples;
  gyroZ_offset = gyroZ_sum / (float)samples;
}

void readIMU(float &ax, float &ay, float &az, float &gx, float &gy, float &gz) {
  // Accel
  Wire.beginTransmission(MPU_ADDR);
  Wire.write(0x3B);
  Wire.endTransmission(false);
  Wire.requestFrom(MPU_ADDR, 6, true);
  accX = Wire.read() << 8 | Wire.read();
  accY = Wire.read() << 8 | Wire.read();
  accZ = Wire.read() << 8 | Wire.read();

  // Gyro
  Wire.beginTransmission(MPU_ADDR);
  Wire.write(0x43);
  Wire.endTransmission(false);
  Wire.requestFrom(MPU_ADDR, 6, true);
  gyroX = Wire.read() << 8 | Wire.read();
  gyroY = Wire.read() << 8 | Wire.read();
  gyroZ = Wire.read() << 8 | Wire.read();

  // Convert with offset & scale
  ax = (accX - accX_offset) / ACCEL_SCALE;
  ay = (accY - accY_offset) / ACCEL_SCALE;
  az = (accZ - accZ_offset) / ACCEL_SCALE;
  gx = (gyroX - gyroX_offset) / GYRO_SCALE;
  gy = (gyroY - gyroY_offset) / GYRO_SCALE;
  gz = (gyroZ - gyroZ_offset) / GYRO_SCALE;
}

void setAccelRange(int range) {
  accelRange = range;
  Wire.beginTransmission(MPU_ADDR);
  Wire.write(0x1C);
  Wire.write(range << 3);
  Wire.endTransmission(true);

  switch (range) {
    case 0: ACCEL_SCALE = 16384.0; break; // ±2g
    case 1: ACCEL_SCALE = 8192.0;  break; // ±4g
    case 2: ACCEL_SCALE = 4096.0;  break; // ±8g
    case 3: ACCEL_SCALE = 2048.0;  break; // ±16g
  }
}

void setGyroRange(int range) {
  gyroRange = range;
  Wire.beginTransmission(MPU_ADDR);
  Wire.write(0x1B);
  Wire.write(range << 3);
  Wire.endTransmission(true);

  switch (range) {
    case 0: GYRO_SCALE = 131.0; break;   // ±250 °/s
    case 1: GYRO_SCALE = 65.5; break;    // ±500 °/s
    case 2: GYRO_SCALE = 32.8; break;    // ±1000 °/s
    case 3: GYRO_SCALE = 16.4; break;    // ±2000 °/s
  }
}
