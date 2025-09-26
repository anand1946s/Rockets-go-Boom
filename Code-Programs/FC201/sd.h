#ifndef SD_H
#define SD_H

#include <Arduino.h>

// --- Sensor Data Structure (shared everywhere) ---
typedef struct {
  unsigned long time;
  float ax, ay, az;
  float gx, gy, gz;
  float pre;
  float temp;
  float alti;

  double lat;
  double lon;
  float gpsAlt;

} SensorData;

// --- Function Prototypes ---
bool initSD(uint8_t csPin);
void logData(const SensorData &data);

#endif
