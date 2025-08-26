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
  
  

  switch (currentMode) {
    case IDLE:
      systemStatus = checkSystemStatus();
      if (systemStatus != OK) {
        currentMode = DEBUGGING;
        
      }

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

    case TRIG1: // para trigger
      deployparachute();    
      break;

    case TRIG2: //payload trigger
      deploypayload();
      break;
  }
}

bool initialize() {
  bool imuOK = initIMU();
  bool bmpOK = initBMP();
  sendStatus("Ready to launch");
  if( imuOK && bmpOK){
    currentMode = ARMING;
    return true;
  }
  else{
    currentMode = DEBUGGING;
    return false;
  }
  //return imuOK && bmpOK;
  
}



void debugging() {
  sendStatus("Critical ERROR occurred");
  delay(500);
}

void launch() {
  logData();
  if (millis() - PARATIME >= 12000) {
      // 12s passed → trigger parachute
      currentMode = TRIG1;

  }
  
  if (millis() - PAYTIME >= 16000) {
      
    // 16s passed → trigger payload
      currentMode = TRIG2;
    }

  
  
  
}

void deploypayload() {
  
  digitalWrite(PAYLOAD_PIN, HIGH);
  sendStatus("Payload deployed!");
  currentMode = LAUNCH;
}

void deployparachute() {
  
  digitalWrite(PARACHUTE_PIN, HIGH);
  sendStatus("Parachute deployed!");
  currentMode = LAUNCH;
}