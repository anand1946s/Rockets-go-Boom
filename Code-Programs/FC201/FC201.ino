//Guys we write the main logic in this file.The current one is a sample and have to be changed.
// We are using FreeRTOS for ESP32.its basically an efficient way of assigning functions as tasks with priorities.
//more priority task is executed first. refer yt 
//Do check using freertos
// also the remaining files will store the functions for modules.represented by names.
//we simply include them in mode.cpp and use them here.thats the logic 

// every module has 2 files one with .h and other with .cpp. we define the function names in .h(header file) and actual definition in cpp file

#include <Arduino.h>
#include "imu.h"
#include "bmp180.h"
#include "lora.h"
#include "sdcard.h"
#include "gps.h"
#include "mode.h"
#include "config.h"

QueueHandle_t sensorQueue;       

typedef struct {
  unsigned long time;
  float ax, ay, az;
  float gx, gy, gz;
  float pressure;
  float temperature;
} SensorData;

// ------------------- TASKS ----------------------

void sensorTask(void *pvParameters) {
  SensorData data;
  for (;;) {
    data.time = millis();
    readIMU(&data.ax, &data.ay, &data.az, &data.gx, &data.gy, &data.gz);
    readBMP(&data.pressure, &data.temperature);

    xQueueSend(sensorQueue, &data, portMAX_DELAY);

    vTaskDelay(20 / portTICK_PERIOD_MS); // 50Hz
  }
}

void loggerTask(void *pvParameters) {
  SensorData data;
  for (;;) {
    if (xQueueReceive(sensorQueue, &data, portMAX_DELAY) == pdTRUE) {
      logData(data);   // write to SD + LoRa
    }
  }
}

void gpsTask(void *pvParameters) {
  for (;;) {
    if (currentMode == RECOVERY) {
      readGPS();
    }
    vTaskDelay(1000 / portTICK_PERIOD_MS); // 1Hz
  }
}

void commandTask(void *pvParameters) {
  for (;;) {
    String cmd = checkCmd();
    if (cmd.length() > 0) {
      handleCommand(cmd);
    }
    vTaskDelay(100 / portTICK_PERIOD_MS);
  }
}

// ------------------- SETUP ----------------------

void setup() {
  Serial.begin(115200);

  initIMU();
  initBMP();
  initLoRa();
  initSD();
  initGPS();

  // Create queue for sensor data
  sensorQueue = xQueueCreate(10, sizeof(SensorData));

  // Core 1 (sensor + logging)
  xTaskCreatePinnedToCore(sensorTask, "Sensor", 4096, NULL, 2, NULL, 1);
  xTaskCreatePinnedToCore(loggerTask, "Logger", 4096, NULL, 1, NULL, 1);

  // Core 0 (comms + gps)
  xTaskCreatePinnedToCore(gpsTask, "GPS", 4096, NULL, 1, NULL, 0);
  xTaskCreatePinnedToCore(commandTask, "Command", 4096, NULL, 1, NULL, 0);
}

void loop() {
  // empty, tasks run independently
}
