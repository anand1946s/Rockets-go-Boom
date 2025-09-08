#include <SD.h>

#ifndef CONFIG_H
#define CONFIG_H

enum FlightMode { IDLE, INIT, ARMING, DEBUGGING, LAUNCH, TRIG1, TRIG2 };
enum SysStatus { OK, HALT };

//pins
const int PARACHUTE_PIN = 9;
const int PAYLOAD_PIN = 8;
const int SD_CS =10;

// Timers
extern unsigned long lastTime;
extern unsigned long lastSendTime;
extern const unsigned long Interval;

// Other global objects
extern File datafile;

#endif