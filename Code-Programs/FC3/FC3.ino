#include <Wire.h>
#include <MPU6050.h>
#include <SPI.h>
#include <SD.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BMP085.h>
#include <LoRa.h>

//define variables
const int CS = 10;  //Cs pin
unsigned long lastTime = 0;
unsigned long Interval = 200; //chnage this to change time interval
unsigned long lastSendTime = 0;

float temp;
float pre;
float alti;
float logtime;

void recieveCommand();


//define objects and pins
MPU6050 IMU;
Adafruit_BMP085 BMP;
File datafile;

//define enums
enum FlightMode { IDLE,INIT,ARMING, DEBUGGING, LAUNCH, TRIG1, TRIG2 };
enum SysStatus { OK, HALT };

FlightMode currentMode = IDLE;
SysStatus systemStatus = OK;

void setup() {
  Serial.begin(115200);
  LoRa.begin(433E6);
  

  pinMode(payload, OUTPUT);
  pinMode(parachute, OUTPUT);
  //Add stuff
}

// === Loop ===
void loop() {
  recieveCommand();
  if(currentMode==INIT){
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
    senddata();
  }
}

}

void modeManager() {
  // Check system health before doing anything
  systemStatus = checkSystemStatus();
  if (systemStatus != OK) {
    currentMode = DEBUGGING;
    return;
  }

 
  switch (currentMode) {
    case IDLE:
      //wait for lora;
      break;
    case INIT:
      initialize();
      break;

    case ARMING:
      break;

    case DEBUGGING:
      debugging();
      break;

    case LAUNCH:
      launch();
      break;

    case TRIG1:
      payload();
      break;

    case TRIG2:
      parachute();
      break;
  }
}

SysStatus checkSystemStatus() {
  if (!systemCheck()) {
    return HALT;
  }
  return OK;
}


bool systemCheck() {
  bool ok = true;

  
  if (!IMU.begin(MPU6050_SCALE_2000DPS, MPU6050_RANGE_8G)) {
    sendStatus("IMU check failed");
    ok = false;
  }

  if (!BMP.begin()) {
    sendStatus("BMP check failed");
    ok = false;
  }

  if (!SD.begin(CS)) {
    sendStatus("SD check failed");
    ok = false;
  } 
  else {
    File test = SD.open("test.txt", FILE_WRITE);
    if (!test) {
      sendStatus("SD write test failed");
      ok = false;
    } 
    else {
      test.println("TEST");
      test.flush();
      test.close();
    }
  }
  return ok;
}



void initialize() {
  datafile = SD.open("data.csv", FILE_WRITE);
  sendStatus("=== Starting INIT - SELF TEST ===");
  int flag = 1;

  
  if (!IMU.begin(MPU6050_SCALE_2000DPS, MPU6050_RANGE_8G)) {
    sendStatus("IMU init failed");
    flag = 0;
  }

 
  if (!BMP.begin()) {
    sendStatus("BMP180 init failed");
    flag = 0;
  }

  // Initialize SD
  if (!SD.begin(CS)) {
    sendStatus("SD card init failed");
    flag = 0;
  }
  else {
    datafile = SD.open("data.csv", FILE_WRITE);
    if (!datafile) {
      sendStatus("Failed to open data.csv");
      flag = 0;
    }
  }

  // Do a test sensor read
  if (flag) {
    
    temp = BMP.readTemperature();
    pre = BMP.readPressure() / 100.0;
    alti = BMP.readAltitude(101325);
    datafile.println("Time,Ax,Ay,Az,Gx,Gy,Gz,Temp,Pre(hPa),Alti");
    datafile.print(normAcc.XAxis, 4); datafile.print(",");
    datafile.print(normAcc.YAxis, 4); datafile.print(",");
    datafile.print(normAcc.ZAxis, 4); datafile.print(",");

    datafile.print(normGyro.XAxis, 2); datafile.print(",");
    datafile.print(normGyro.YAxis, 2); datafile.print(",");
    datafile.println(normGyro.ZAxis, 2);

    sendStatus("INIT and TEST PASSED");
    currentMode = ARMING;
  } 
  else {
    sendStatus("TEST Failed... Entering DEBUGGING");
    currentMode = DEBUGGING;
  }
}



void debugging() {
  sendStatus("===Debugging Mode===");

  if (SDFailed()) {
    sendStatus("RESULT: SD Failed");
  }

  if (sensorFailed()) {
    sendStatus("RESULT: Sensor Failed");
  }
  if (ignitorFailed()) {
    sendStatus("RESULT: Ignitor Failed");
  }

  if (checkSystemStatus() == OK) {
    sendStatus("Sys recovered . Entering INIT");
    currentMode = INIT;
  }
}

void launch() {
  
  //Ignitor powers
  readSensor();
  writeSensor();
  delay(20);
}
void readSensor() {
  
  temp = BMP.readTemperature();
  pre = BMP.readPressure() / 100.0;     // in hPa
  alti = BMP.readAltitude(101325);          
}
void writeSensor() {
  if (datafile) {
    datafile.print(normAcc.XAxis, 4); datafile.print(",");
    datafile.print(normAcc.YAxis, 4); datafile.print(",");
    datafile.print(normAcc.ZAxis, 4); datafile.print(",");

    datafile.print(normGyro.XAxis, 2); datafile.print(",");
    datafile.print(normGyro.YAxis, 2); datafile.print(",");
    datafile.print(normGyro.ZAxis, 2); datafile.print(",");

    datafile.print(temp, 2); datafile.print(",");
    datafile.print(pre, 2); datafile.print(",");
    datafile.print(alti, 2);
    datafile.println();
  }
}



void sendStatus(const char* message) {
  LoRa.beginPacket();
  LoRa.print(message);
  LoRa.endPacket();

}

void senddata() {
  String dataMsg = "DATA,";
  dataMsg += String(normAcc.XAxis, 2) + ",";
  dataMsg += String(normAcc.YAxis, 2) + ",";
  dataMsg += String(normAcc.ZAxis, 2) + ",";
  dataMsg += String(normGyro.XAxis, 2) + ",";
  dataMsg += String(normGyro.YAxis, 2) + ",";
  dataMsg += String(normGyro.ZAxis, 2) + ",";
  dataMsg += String(temp, 2) + ",";
  dataMsg += String(pre, 2) + ",";
  dataMsg += String(alti, 2);

  sendStatus(dataMsg.c_str());
}


void payload(){
    digitalWrite(payload, HIGH);
    delay(50);
    digitalWrite(payload , LOW);
    sendStatus("Payload deployed");
    currentMode = LAUNCH;
}

void parachute(){
    digitalWrite(parachute, HIGH);
    delay(50);
    digitalWrite(parachute , LOW);
    sendStatus("parachutes deployed");
    currentMode = LAUNCH;
}

void receiveCommand() {
  if (LoRa.parsePacket()) {
    String command = "";
    while (LoRa.available()) {
      command += (char)LoRa.read();  // Read each byte into the string
    }
    command.trim();  // Remove whitespace or newlines

    if (command == "ARM" && currentMode == IDLE) {
      currentMode = INIT;
      delay(50);
      sendStatus("ARM received → INIT");
    }
    if (command == "LIFTOFF" && currentMode == ARMING) {
      currentMode = LAUNCH;
      delay(50);
      sendStatus("LIFTOFF received → LAUNCH");
    } 
    if(command == "ABORT"){
      sendStatus("Switching to IDLE...");
      currentMode = IDLE;
      delay(50);
    }
    
    if(command == "PAYLOAD" && currentMode == LAUNCH){
      sendStatus("Deploying payload");
      currentMode= TRIG1;
      delay(50);
    }

    if(command == "PARACHUTE" && currentMode == LAUNCH){
      sendStatus("Deploying parachutes");
      currentMode = TRIG2;
      delay(50);
    }
  }
}
