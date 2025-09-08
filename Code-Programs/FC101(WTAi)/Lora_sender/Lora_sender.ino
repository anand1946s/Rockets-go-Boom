#include <Wire.h>
#include <LoRa.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BMP085.h>

// ----------- HW290 (MPU6050 type) -----------
const int MPU_ADDR = 0x68;
int16_t accX, accY, accZ;
float gX, gY, gZ;
float accelScale = 16384.0; // default ±2g
float accX_off = 0, accY_off = 0, accZ_off = 0;

// ----------- BMP180 -----------
Adafruit_BMP085 bmp;
float tempC, pressure_hPa, altitude_m;
float refAltitude = 0;  // calibration baseline

// ----------- LoRa -----------
#define SS 10
#define RST 9
#define DIO0 2

void setup() {
  Serial.begin(9600);
  Wire.begin();

  // Init MPU
  Wire.beginTransmission(MPU_ADDR);
  Wire.write(0x6B); 
  Wire.write(0); // wake up sensor
  Wire.endTransmission(true);

  // Init BMP180
  if (!bmp.begin()) {
    Serial.println("BMP180 not found!");
    while (1);
  }

  // Init LoRa
  LoRa.setPins(SS, RST, DIO0);
  if (!LoRa.begin(433E6)) { // 433 MHz
    Serial.println("LoRa init failed!");
    while (1);
  }
  Serial.println("LoRa Sender ready");

  calibrateSensors();
}

void loop() {
  readMPU();
  readBMP();

  // Build CSV string
  String packet = String(gX, 3) + "," + String(gY, 3) + "," + String(gZ, 3) + "," +
                  String(tempC, 2) + "," + String(pressure_hPa, 2) + "," + String(altitude_m, 2);

  // Send packet
  LoRa.beginPacket();
  LoRa.print(packet);
  LoRa.endPacket();

  Serial.println("Sent: " + packet);
  delay(1000); // ~1 Hz
}

// ------------ Functions ------------
void calibrateSensors() {
  Serial.println("Calibrating...");
  long x=0, y=0, z=0;
  for (int i=0; i<200; i++) {
    Wire.beginTransmission(MPU_ADDR);
    Wire.write(0x3B);
    Wire.endTransmission(false);
    Wire.requestFrom(MPU_ADDR, 6, true);

    x += (Wire.read() << 8 | Wire.read());
    y += (Wire.read() << 8 | Wire.read());
    z += (Wire.read() << 8 | Wire.read());
    delay(5);
  }
  accX_off = x/200.0;
  accY_off = y/200.0;
  accZ_off = z/200.0;

  refAltitude = bmp.readAltitude();
  Serial.println("Calibration done. Reference altitude set to 0.");
}

void readMPU() {
  Wire.beginTransmission(MPU_ADDR);
  Wire.write(0x3B);
  Wire.endTransmission(false);
  Wire.requestFrom(MPU_ADDR, 6, true);

  accX = Wire.read() << 8 | Wire.read();
  accY = Wire.read() << 8 | Wire.read();
  accZ = Wire.read() << 8 | Wire.read();

  gX = (accX - accX_off) / accelScale;
  gY = (accY - accY_off) / accelScale;
  gZ = (accZ - accZ_off) / accelScale;
}

void readBMP() {
  tempC = bmp.readTemperature();
  pressure_hPa = bmp.readPressure() / 100.0;
  altitude_m = bmp.readAltitude() - refAltitude;
}
