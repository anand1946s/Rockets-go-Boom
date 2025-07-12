#include <Wire.h>
#include <MPU6050.h>
#include <Adafruit_BMP085.h>
#include <SD.h>

MPU6050 imu;
Adafruit_BMP085 bmp;

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

void setup() {
  Serial.begin(9600);
  delay(500);
  Serial.println("=== HW-290 Sensor + Angle Estimator ===");

  Wire.begin();

  imu.initialize();
  if (!imu.testConnection()) {
    Serial.println("MPU6050 connection failed!");
    while (1);
  }

  if (!bmp.begin()) {
    Serial.println("BMP180 init failed!");
    while (1);
  }

  if (!SD.begin(10)) {
    Serial.println("SD Card init failed!");
  } else {
    Serial.println("SD Card ready.");
  }

  calibrateSensors();
  lastAngleTime = millis();
}

void loop() {
  unsigned long currentTime = millis();

  readSensors();

  float deltaTime = (currentTime - lastAngleTime) / 1000.0;
  lastAngleTime = currentTime;

  // Integrate gyro to estimate angles
  angleX += gx_dps * deltaTime;
  angleY += gy_dps * deltaTime;
  angleZ += gz_dps * deltaTime;

  Serial.print("Acc (g): ");
  Serial.print(ax_g, 2); Serial.print(", ");
  Serial.print(ay_g, 2); Serial.print(", ");
  Serial.print(az_g, 2);

  Serial.print(" | Gyro (°/s): ");
  Serial.print(gx_dps, 2); Serial.print(", ");
  Serial.print(gy_dps, 2); Serial.print(", ");
  Serial.print(gz_dps, 2);

  Serial.print(" | Temp: ");
  Serial.print(temp, 2); Serial.print(" C");

  Serial.print(" | Pressure: ");
  Serial.print(pre, 2); Serial.print(" hPa");

  Serial.print(" | Altitude: ");
  Serial.print(alti, 2); Serial.print(" m");

  Serial.print(" | Angle (°): ");
  Serial.print(angleX, 2); Serial.print(", ");
  Serial.print(angleY, 2); Serial.print(", ");
  Serial.println(angleZ, 2);

  delay(500);
}

void readSensors() {
  // Get raw readings from MPU6050
  imu.getAcceleration(&axRaw, &ayRaw, &azRaw);
  imu.getRotation(&gxRaw, &gyRaw, &gzRaw);

  // Read barometric data (if BMP180 is used)
  temp = bmp.readTemperature();                // °C
  pre  = bmp.readPressure() / 100.0;           // hPa
  alti = bmp.readAltitude(101325);             // meters (sea level pressure as reference)

  // Convert raw acceleration to g (assuming ±2g range)
  ax_g = (axRaw - accX_offset) / 16384.0;
  ay_g = (ayRaw - accY_offset) / 16384.0;
  az_g = (azRaw - accZ_offset) / 16384.0;

  // Convert raw gyro data to degrees/sec (assuming ±250°/s range)
  gx_dps = (gxRaw - gyroX_offset) / 131.0;
  gy_dps = (gyRaw - gyroY_offset) / 131.0;
  gz_dps = (gzRaw - gyroZ_offset) / 131.0;
}


void calibrateSensors() {
  const int samples = 100;
  long accXSum = 0, accYSum = 0, accZSum = 0;
  long gyroXSum = 0, gyroYSum = 0, gyroZSum = 0;

  Serial.println("Calibrating... Keep the board still and flat!");

  for (int i = 0; i < samples; i++) {
    int16_t ax, ay, az, gx, gy, gz;
    imu.getAcceleration(&ax, &ay, &az);
    imu.getRotation(&gx, &gy, &gz);

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
  accZ_offset = (accZSum / (float)samples) - 16384.0; // assuming 1g up

  gyroX_offset = gyroXSum / (float)samples;
  gyroY_offset = gyroYSum / (float)samples;
  gyroZ_offset = gyroZSum / (float)samples;

  Serial.println("Calibration complete!");
  Serial.print("Accel Offsets: ");
  Serial.print(accX_offset); Serial.print(", ");
  Serial.print(accY_offset); Serial.print(", ");
  Serial.println(accZ_offset);

  Serial.print("Gyro Offsets: ");
  Serial.print(gyroX_offset); Serial.print(", ");
  Serial.print(gyroY_offset); Serial.print(", ");
  Serial.println(gyroZ_offset);
}
