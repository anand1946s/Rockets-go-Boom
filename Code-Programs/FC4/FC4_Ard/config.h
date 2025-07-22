#include <SD.h>

#ifndef CONFIG_H
#define CONFIG_H

enum FlightMode { IDLE, INIT, ARMING, DEBUGGING, LAUNCH, TRIG1, TRIG2 };
enum SysStatus { OK, HALT };


extern FlightMode currentMode;
extern SysStatus systemStatus;
extern File dataFile;

// Timers
extern unsigned long lastTime;
extern unsigned long lastSendTime;
extern const unsigned long Interval;

// Other global objects
extern File dataFile;

#endif