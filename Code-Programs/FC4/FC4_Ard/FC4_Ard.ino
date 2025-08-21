#include "config.h"
#include "imu.h"
#include "bmp180.h"
#include "lora.h"
#include "sdcard.h"
#include "mode.h"
#include "utils.h"

#include <SPI.h>




FlightMode currentMode = IDLE;
SysStatus systemStatus = OK;

unsigned long lastTime = 0;
unsigned long lastSendTime = 0;
const unsigned long Interval = 1000;
const unsigned long sendInterval =2000;

unsigned long PAYTIME = 0;
unsigned long PARATIME = 0;



File datafile;

void setup() {

  if(initIMU() && initBMP() && initLoRa() && initSD()){
    Serial.println("sensors ok!!!");
    systemStatus = OK;
    calibrateIMU();

    datafile = SD.open("samples.csv",FILE_WRITE);
    if(!datafile){
      systemStatus = HALT;
      Serial.println("sd card failed");
      return ;
    } 
    else{
      datafile.println(F("T(ms),Ax,Ay,Az,Gx,Gy,Gz,Alti,Pre,Temp"));
      datafile.flush();

    }

    pinMode(PAYLOAD_PIN, OUTPUT);
    pinMode(PARACHUTE_PIN, OUTPUT);
    
    if(systemStatus == OK) sendStatus("All good");
    
  }
  else{
    systemStatus = HALT;
    sendStatus("sensor error");
  }
    
}

void loop() {
 String cmd = checkCmd();  // Get command from LoRa

  if (cmd == "ARM") {
    currentMode = INIT;
  } 
  else if (cmd == "LAUNCH") {
    PARATIME = millis();
    PAYTIME = millis();

      if (checkSystemStatus() == OK) {
        currentMode = LAUNCH;
      } 
      else {
      currentMode = DEBUGGING;  
          }
  }
  else if(cmd == "ABORT"){
    currentMode = IDLE;
  }

  if (currentMode == INIT) {
    modeManager();
  }

  if (currentMode == LAUNCH) {

    if (millis() - lastTime >= Interval) {
      lastTime = millis();
      
      modeManager();
      datafile.flush();
    }

    if (millis() - lastSendTime >= sendInterval) {
      lastSendTime = millis();
      sendData();
    }
  }
}
