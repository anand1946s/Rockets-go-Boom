#ifndef SDCARD_H
#define SDCARD_H


#include <Arduino.h>

bool initSD();
bool logData(const String& data);

#endif