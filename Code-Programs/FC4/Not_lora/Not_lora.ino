#include <Wire.h>
#include <MPU6050.h>
#include <Adafruit_BMP085.h>
#include <SPI.h>
#include <SD.h>
#include <LoRa.h>

MPU6050 mpu;

Adafruit_BMP085 bmp;

const int chipSelect = 10;

// LoRa pins (change as needed)
#define LORA_SS 7
#define LORA_RST 6
#define LORA_DIO0 2

void setup() {
  Serial.begin(115200);
  while (!Serial);

  Serial.println("Starting HW290, SD card, and LoRa test...");

  Wire.begin();

  // MPU6050 initialization
  Serial.println("Initializing MPU6050...");
  mpu.initialize();

  // Check if MPU6050 is connected
  if (mpu.testConnection()) {
    Serial.println("MPU6050 connection successful!");
  } else {
    Serial.println("MPU6050 connection failed!");
  }

  // BMP180 initialization
  Serial.println("Initializing BMP180...");
  if (bmp.begin()) {
    Serial.println("BMP180 initialized successfully!");
  } else {
    Serial.println("BMP180 initialization failed!");
  }

  // SD card initialization
  Serial.print("Initializing SD card...");
  if (!SD.begin(chipSelect)) {
    Serial.println("Failed to initialize SD card!");
  } else {
    Serial.println("SD card initialized successfully!");
    File testFile = SD.open("test.txt", FILE_WRITE);
    if (testFile) {
      testFile.println("SD card write test successful.");
      testFile.close();
      Serial.println("Wrote test file to SD card.");
    } else {
      Serial.println("Failed to open test file on SD card.");
    }
  }

  // LoRa initialization
  Serial.println("Initializing LoRa...");
  LoRa.setPins(LORA_SS, LORA_RST, LORA_DIO0);

  if (!LoRa.begin(915E6)) {  // Adjust frequency for your region
    Serial.println("LoRa initialization failed!");
  } else {
    Serial.println("LoRa initialized successfully!");
  }

  Serial.println("Setup complete!");
}

void loop() {
  // Read raw accelerometer and gyroscope data
  int16_t ax, ay, az;
  int16_t gx, gy, gz;

  mpu.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);

  Serial.print("Accel (raw): ");
  Serial.print(ax); Serial.print(", ");
  Serial.print(ay); Serial.print(", ");
  Serial.println(az);

  Serial.print("Gyro (raw): ");
  Serial.print(gx); Serial.print(", ");
  Serial.print(gy); Serial.print(", ");
  Serial.println(gz);

  // Read temperature and pressure from BMP180
  Serial.print("BMP180 temp: ");
  Serial.print(bmp.readTemperature());
  Serial.print(" C, pressure: ");
  Serial.print(bmp.readPressure());
  Serial.println(" Pa");

  
}
