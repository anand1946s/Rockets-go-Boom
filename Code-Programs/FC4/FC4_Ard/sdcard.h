#ifndef SDCARD_H
#define SDCARD_H


#include <Arduino.h>

bool initSD();
bool loagData(const String& data);

#endif