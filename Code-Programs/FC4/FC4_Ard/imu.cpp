#include "imu.h"
#include "lora.h"
#include "config.h"

#include <Wire.h>
#include <MPU6050.h>


MPU6050 imu;

float accX_offset = 0, accY_offset = 0, accZ_offset = 0;
float gyroX_offset = 0, gyroY_offset = 0, gyroZ_offset = 0;

bool initIMU() {
  Wire.begin();
  imu.initialize();

  if (!imu.testConnection()) {
    return false;
    // Optionally, set a status flag or halt
  } else {
    return true;
  }
}

void calibrateIMU() {
  const int numSamples = 100;
  long accX_sum = 0, accY_sum = 0, accZ_sum = 0;
  long gyroX_sum = 0, gyroY_sum = 0, gyroZ_sum = 0;

  for (int i = 0; i < numSamples; i++) {
    int16_t ax, ay, az, gx, gy, gz;
    imu.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);

    accX_sum += ax;
    accY_sum += ay;
    accZ_sum += az;
    gyroX_sum += gx;
    gyroY_sum += gy;
    gyroZ_sum += gz;

    delay(5); // slight pause to get stable readings
  }
  accX_offset = accX_sum / (float)numSamples;
  accY_offset = accY_sum / (float)numSamples;
  accZ_offset = (accZ_sum / (float)numSamples) - 16384.0; // assuming 1g offset on Z
  gyroX_offset = gyroX_sum / (float)numSamples;
  gyroY_offset = gyroY_sum / (float)numSamples;
  gyroZ_offset = gyroZ_sum / (float)numSamples;

  sendStatus("Calb Done!!");
}

void readIMU(float &ax, float &ay, float &az, float &gx, float &gy, float &gz) {
  int16_t raw_ax, raw_ay, raw_az;
  int16_t raw_gx, raw_gy, raw_gz;

  imu.getMotion6(&raw_ax, &raw_ay, &raw_az, &raw_gx, &raw_gy, &raw_gz);

  ax = (raw_ax - accX_offset) / 16384.0; // Convert to g
  ay = (raw_ay - accY_offset) / 16384.0;
  az = (raw_az - accZ_offset) / 16384.0;

  gx = (raw_gx - gyroX_offset) / 131.0;  // Convert to °/s
  gy = (raw_gy - gyroY_offset) / 131.0;
  gz = (raw_gz - gyroZ_offset) / 131.0;
}


  