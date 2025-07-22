#include "mode.h"
#include "imu.h"
#include "bmp180.h"
#include "sdcard.h"
#include "lora.h"
#include "utils.h"

extern FlightMode currentMode;
extern SysStatus systemStatus;

SysStatus checkSystemStatus() {
  if (!systemCheck()) {
    return HALT;
  }
  return OK;
}

void modeManager() {
  // Recheck system health at every phase
  systemStatus = checkSystemStatus();
  if (systemStatus != OK) {
    currentMode = DEBUGGING;
    return;
  }

  switch (currentMode) {
    case IDLE:
      break;

    case INIT:
      initialize();
      break;

    case ARMING:
      countdown();
      break;

    case DEBUGGING:
      debugging();
      break;

    case LAUNCH:
      launch();
      break;

    case TRIG1:
      deploypayload();
      break;

    case TRIG2:
      deployparachute();
      break;
  }
}

void initialize() {
  sendStatus("Initializing...");
  // Init steps if needed
  currentMode = ARMING;
}

void countdown() {
  sendStatus("Arming countdown started...");
  delay(5000);  // Just a placeholder
  currentMode = LAUNCH;
}

void debugging() {
  sendStatus("Debugging Mode: Check sensors and reset.");
  delay(1000);
}

void launch() {
  logData();
  sendStatus("Logging and transmitting...");

}

void deploypayload() {
  digitalWrite(2, HIGH);
  sendStatus("Payload deployed!");
}

void deployparachute() {
  digitalWrite(3, HIGH);
  sendStatus("Parachute deployed!");
}