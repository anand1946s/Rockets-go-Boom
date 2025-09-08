#include "bmp180.h"
#include "config.h"
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BMP085.h>


Adafruit_BMP085 bmp;

// Reference altitude
float refAltitude = 0.0;

bool initBMP() {
  if (!bmp.begin()) {
    return true; 
  } else {
    
    refAltitude = bmp.readAltitude(1013.25);  
    return false; 
  }
}

void readBMP(float &temp, float &pre, float &alti) {
  temp = bmp.readTemperature();       // in °C
  pre  = bmp.readPressure() / 100.0;  // in hPa
  
  alti = bmp.readAltitude(1013.25) - refAltitude;  
}
