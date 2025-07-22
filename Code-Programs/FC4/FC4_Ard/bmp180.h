#ifndef BMP_H
#define BMP_H

#include <Arduino.h>

// Public functions for the barometer module
void initBMP();                             // Initialize the BMP180
void readBMP(float &temp, float &pre, float &alti);  // Read all values

#endif
