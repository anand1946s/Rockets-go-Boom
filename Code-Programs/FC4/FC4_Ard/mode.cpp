#include "mode.h"
#include "imu.h"
#include "bmp180.h"
#include "sdcard.h"
#include "lora.h"
#include "utils.h"
#include "config.h"

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

bool initialize() {
  bool imuOK = initIMU();
  bool bmpOK = initBMP();

  return imuOK && bmpOK;
  currentMode = ARMING;
}


void countdown() {
  sendStatus("countdown started...");
  delay(7000);  // Just a placeholder
  currentMode = LAUNCH;
}

void debugging() {
  sendStatus("Debugging Mode: Check sensors and reset.");
  delay(1000);
}

void launch() {
  logData();
  pinMode(IGNITE_PIN,OUTPUT);
  digitalWrite(IGNITE_PIN,HIGH);
  

}

void deploypayload() {
  pinMode(PAYLOAD_PIN,OUTPUT);
  digitalWrite(PAYLOAD_PIN, HIGH);
  sendStatus("Payload deployed!");
}

void deployparachute() {
  pinMode(PARACHUTE_PIN,OUTPUT);
  digitalWrite(PARACHUTE_PIN, HIGH);
  sendStatus("Parachute deployed!");
}