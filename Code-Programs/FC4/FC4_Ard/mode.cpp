#include "mode.h"
#include "imu.h"
#include "bmp180.h"
#include "sdcard.h"
#include "lora.h"
#include "utils.h"
#include "config.h"

extern FlightMode currentMode;
extern SysStatus systemStatus;
extern unsigned long PAYTIME;
extern unsigned long PARATIME;


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
  sendStatus("Ready to launch");
  return imuOK && bmpOK;
  
}



void debugging() {
  sendStatus("Debugging Mode: Check sensors and reset.");
  delay(1000);
}

void launch() {
  logData();
  if (millis() - PAYTIME >= 16000) {
      
    // 12s passed → trigger payload
      deploypayload();
    }

  if (millis() - PARATIME >= 12000) {
      // 16s passed → trigger parachute
      deployparachute();
  }
  
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