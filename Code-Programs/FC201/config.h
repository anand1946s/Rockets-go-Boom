#ifndef CONFIG_H
#define CONFIG_H

#include <Arduino.h>

// Flight modes
enum FlightMode {
  IDLE,
  INIT,
  LAUNCH,
  RECOVERY,
  RETRIEVAL
};

// System health states
enum SystemHealth {
  SYS_OK,
  SYS_HALT
};

// ------------------- Pin definitions -------------------
const int PARA_PIN = 25;   // Parachute trigger
const int PAY_PIN  = 26;   // Payload trigger
const int SD_CS    = 5;    // SD card CS

// ------------------- Global states -------------------
extern FlightMode currentMode ;
extern SystemHealth sysHealth ;

#endif
