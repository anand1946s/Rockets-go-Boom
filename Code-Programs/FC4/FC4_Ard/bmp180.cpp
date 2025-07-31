#include "bmp180.h"
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BMP085.h>

// Create BMP180 object
Adafruit_BMP085 bmp;

void initBMP() {
  if (!bmp.begin()) {
      return true;
    // Optionally set a flag or halt the system
  } else {
      return false;
  }
}

void readBMP(float &temp, float &pre, float &alti) {
  temp = bmp.readTemperature();          // In °C
  pre  = bmp.readPressure() / 100.0;     // Convert to hPa
  alti = bmp.readAltitude();             // Based on sea level pressure = 1013.25 hPa
}
