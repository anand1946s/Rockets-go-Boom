#ifndef GPS_H
#define GPS_H

#include <Arduino.h>

// Initialize GPS module
bool initGPS();

// Read GPS data
void readGPS();

// Optional: get last latitude/longitude
float getLatitude();
float getLongitude();

#endif
