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

File datafile;

void setup() {
  Serial.begin(9600);

  initIMU();
  calibrateIMU();

  initBMP();
  initLoRa();
  initSD();

  pinMode(2, OUTPUT);  // Payload
  pinMode(3, OUTPUT);  // Parachute

  sendStatus("System Ready. Waiting for ARM command...");
}

void loop() {
  checkCmd();

  if (currentMode == INIT) {
    modeManager();
  }

  if (currentMode == LAUNCH) {
    if (millis() - lastTime >= Interval) {
      lastTime = millis();
      modeManager();
      datafile.flush();
    }

    if (millis() - lastSendTime >= 2000) {
      lastSendTime = millis();
      sendData();
    }
  }
}
