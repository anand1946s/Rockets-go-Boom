#include <Wire.h>
#include <MPU6050.h>
#include <SPI.h>
#include <SD.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BMP085.h>
#include <LoRa.h>



//define enums
enum FlightMode { IDLE,INIT,ARMING, DEBUGGING, LAUNCH, TRIG1, TRIG2 };
enum SysStatus { OK, HALT };

SysStatus checkSystemStatus();



void receiveCommand();
void countdown();
void calibrateSensors(); 
void deploypayload();
void deploypayload();
void senddata();
void sendStatus();
void writeSensor();
void readSensor();
void launch();
void debugging();
void initialize();
bool systemCheck();
void modeManager();


int payload=2;
int parachute=3;
const int CS = 10;

bool SDFailed() {
  return !SD.begin(CS);  // Re-attempt to initialize SD card
}

//define variables
  //Cs pin
unsigned long lastTime = 0;
unsigned long Interval = 200; //chnage this to change time interval
unsigned long lastSendTime = 0;

// Raw sensor data
int16_t axRaw, ayRaw, azRaw;
int16_t gxRaw, gyRaw, gzRaw;

// Calibrated data in physical units
float ax_g, ay_g, az_g;
float gx_dps, gy_dps, gz_dps;
float temp, pre, alti;

// Offsets
float accX_offset = 0, accY_offset = 0, accZ_offset = 0;
float gyroX_offset = 0, gyroY_offset = 0, gyroZ_offset = 0;

// Angles
float angleX = 0, angleY = 0, angleZ = 0;
unsigned long lastAngleTime = 0;




//define objects and pins
MPU6050 IMU;
Adafruit_BMP085 BMP;
File datafile;



FlightMode currentMode = IDLE;
SysStatus systemStatus = OK;

void setup() {
  Serial.begin(9600);  
  LoRa.begin(433E6);
  calibrateSensors();
  

  pinMode(payload, OUTPUT);
  pinMode(parachute, OUTPUT);
  //Add stuff
}

// === Loop ===
void loop() {
  receiveCommand();
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
  
 
  switch (currentMode) {
    case IDLE:
      systemStatus = checkSystemStatus();
      if (systemStatus != OK) {
        currentMode = DEBUGGING;
        return;
      }

      //wait for lora;
      break;
    case INIT:
      systemStatus = checkSystemStatus();
      if (systemStatus != OK) {
        currentMode = DEBUGGING;
        return;
      }

      initialize();
      break;

    case ARMING:
      systemStatus = checkSystemStatus();
      if (systemStatus != OK) {
        currentMode = DEBUGGING;
        return;
      }

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

SysStatus checkSystemStatus() {
  if (!systemCheck()) {
    return HALT;
  }
  return OK;
}


bool systemCheck() {
  bool ok = true;

  IMU.initialize();
  if (!IMU.testConnection()) {
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

  
  IMU.initialize();
  if (!IMU.testConnection()) {
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
    datafile.print(ax_g, 4); datafile.print(",");
    datafile.print(ay_g, 4); datafile.print(",");
    datafile.print(az_g, 4); datafile.print(",");

    datafile.print(gx_dps, 2); datafile.print(",");
    datafile.print(gy_dps, 2); datafile.print(",");
    datafile.println(gz_dps, 2);

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

  // if (sensorFailed()) {
  //   sendStatus("RESULT: Sensor Failed");
  // }
  // if (ignitorFailed()) {
  //   sendStatus("RESULT: Ignitor Failed");
  // }

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
  
   // Read raw accelerometer and gyroscope values
  IMU.getAcceleration(&axRaw, &ayRaw, &azRaw);
  IMU.getRotation(&gxRaw, &gyRaw, &gzRaw);

  // Read barometer values
  temp = BMP.readTemperature();                // °C
  pre  = BMP.readPressure() / 100.0;           // hPa
  alti = BMP.readAltitude(101325);             // meters

  // Convert accelerometer to g
  ax_g = (axRaw - accX_offset) / 16384.0;
  ay_g = (ayRaw - accY_offset) / 16384.0;
  az_g = (azRaw - accZ_offset) / 16384.0;

  // Convert gyroscope to degrees per second
  gx_dps = (gxRaw - gyroX_offset) / 131.0;
  gy_dps = (gyRaw - gyroY_offset) / 131.0;
  gz_dps = (gzRaw - gyroZ_offset) / 131.0;       
}
void writeSensor() {
  if (datafile) {
  datafile.print(ax_g, 4); datafile.print(",");
  datafile.print(ay_g, 4); datafile.print(",");
  datafile.print(az_g, 4); datafile.print(",");

  datafile.print(gx_dps, 2); datafile.print(",");
  datafile.print(gy_dps, 2); datafile.print(",");
  datafile.print(gz_dps, 2); datafile.print(",");

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
  dataMsg += String(ax_g, 2) + ",";
  dataMsg += String(ay_g, 2) + ",";
  dataMsg += String(az_g, 2) + ",";
  dataMsg += String(gx_dps, 2) + ",";
  dataMsg += String(gy_dps, 2) + ",";
  dataMsg += String(gz_dps, 2) + ",";
  dataMsg += String(temp, 2) + ",";
  dataMsg += String(pre, 2) + ",";
  dataMsg += String(alti, 2);

  sendStatus(dataMsg.c_str());
  }


void deploypayload(){
    digitalWrite(payload, HIGH);
    delay(50);
    digitalWrite(payload , LOW);
    sendStatus("Payload deployed");
    currentMode = LAUNCH;
}

void deployparachute(){
    digitalWrite(parachute, HIGH);
    delay(50);
    digitalWrite(parachute , LOW);
    sendStatus("parachutes deployed");
    currentMode = LAUNCH;
}

void calibrateSensors() {
  const int samples = 100;
  long accXSum = 0, accYSum = 0, accZSum = 0;
  long gyroXSum = 0, gyroYSum = 0, gyroZSum = 0;

  Serial.println("Calibrating sensors... Please keep the rocket still.");

  for (int i = 0; i < samples; i++) {
    int16_t ax, ay, az, gx, gy, gz;
    IMU.getAcceleration(&ax, &ay, &az);
    IMU.getRotation(&gx, &gy, &gz);

    accXSum += ax;
    accYSum += ay;
    accZSum += az;

    gyroXSum += gx;
    gyroYSum += gy;
    gyroZSum += gz;

    delay(5);
  }

  accX_offset = accXSum / (float)samples;
  accY_offset = accYSum / (float)samples;
  accZ_offset = (accZSum / (float)samples) - 16384.0; // Assuming upright

  gyroX_offset = gyroXSum / (float)samples;
  gyroY_offset = gyroYSum / (float)samples;
  gyroZ_offset = gyroZSum / (float)samples;

  Serial.println("Calibration done.");
}

void countdown() {
  sendStatus("===================================");
  sendStatus("[ARMING] Countdown Initiated");
  sendStatus("===================================");

  for (int i = 5; i >= 1; i--) {
    String msg = ">>> T - " + String(i) + " seconds";
    sendStatus(msg.c_str());
    delay(1000);
  }

  sendStatus("===================================");
  sendStatus(">>> Countdown Complete");
  sendStatus(">>> Awaiting LIFTOFF...");
  sendStatus("===================================");
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
