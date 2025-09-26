#include <Arduino.h>
#include "imu.h"
#include "pre.h"       // BMP3XX module
#include "lora.h"
#include "sd.h"
#include "gps.h"
#include "mode.h"
#include "config.h"
    // shared SensorData struct

QueueHandle_t loggerQueue;
QueueHandle_t telemetryQueue;

//Todos
// Add buzzer pin to work at recovery(HIGH during RECOVERY)
// send system health via LoRa
// LED test for different states with either 5 leds or utility board 
// Utility board leds for LoRa indication
// Correct decision for "How to detect apogee"
//





// ------------------- TASKS ----------------------

// Task: Read sensors and enqueue data

//currentMode= LAUNCH
void sensorTask(void *pvParameters) {
    SensorData data;
    const TickType_t delayTicks = 20 / portTICK_PERIOD_MS; // 50 Hz

    for (;;) {
        data.time = millis();
        data.lat = NAN;
        data.lon = NAN;
        data.gpsAlt = NAN;

        readIMU(&data.ax, &data.ay, &data.az, &data.gx, &data.gy, &data.gz);
        readPressure(&data.pre, &data.temp);
        data.alti = calculateAltitude(data.pre);

        // Send to both queues
        xQueueSend(loggerQueue, &data, 0);     // non-blocking, drops if full
        xQueueSend(telemetryQueue, &data, 0); // non-blocking, drops if full

        vTaskDelay(delayTicks);
    }
}


// Task: Log sensor data to SD (and optionally LoRa)
// Task: Log only the latest sensor data from queue (non-blocking)
//currentMode= LAUNCH
void loggerTask(void *pvParameters) {
    SensorData data;
    SensorData tmp;

    for (;;) {
        // Try to read one element without blocking
        if (xQueueReceive(loggerQueue, &data, 0) == pdTRUE) {
            // Drain the rest of the queue to free space
            while (xQueueReceive(loggerQueue, &tmp, 0) == pdTRUE) {
                data = tmp; // keep the most recent sample
            }

            // Log the latest data
            logData(data); // write to SD
            // sendCommand("DATA"); // optional LoRa transmission
        }

        // Short delay to yield to other tasks (non-blocking)
        vTaskDelay(1 / portTICK_PERIOD_MS);
    }
}


// Task: Read GPS periodically also
//Add cmand recieving part here
void telemetryTask(void *pvParameters) {
    const TickType_t delayTicks = 1000 / portTICK_PERIOD_MS; // 1 Hz
    SensorData data;

    for (;;) {
        // Try to get latest sample
        if (xQueueReceive(telemetryQueue, &data, 0) == pdTRUE) {
            SensorData tmp;
            while (xQueueReceive(telemetryQueue, &tmp, 0) == pdTRUE) {
                data = tmp; // keep most recent
            }

            // GPS only in recovery
            if (currentMode == RECOVERY) {
                double lat, lon;
                float gpsAlt;
                if (readGPS(&lat, &lon, &gpsAlt)) {
                    data.lat = lat;
                    data.lon = lon;
                    data.gpsAlt = gpsAlt;
                } else {
                    data.lat = NAN;
                    data.lon = NAN;
                    data.gpsAlt = NAN;
                }
            } else {
                data.lat = NAN;
                data.lon = NAN;
                data.gpsAlt = NAN;
            }

            // Send out telemetry
            sendData(data);
        }

        vTaskDelay(delayTicks);
    }
}


// Task: Check and handle commands

//Change to modemanaging+event detection(Change name)
void commandTask(void *pvParameters) {
    const TickType_t delayTicks = 100 / portTICK_PERIOD_MS;

    for (;;) {
        String c = checkCmd();
        modeManager();
        vTaskDelay(delayTicks);
        //mid flight events
    }
}

// ------------------- SETUP ----------------------

void setup() {
    //Serial.begin(115200);

    // Initialize all modules
    

    // Create queue for sensor data
    sensorQueue = xQueueCreate(10, sizeof(SensorData));

    // Create FreeRTOS tasks

    // Core 1: high-priority sensor + logging
    xTaskCreatePinnedToCore(sensorTask, "Sensor", 4096, NULL, 2, NULL, 1);
    xTaskCreatePinnedToCore(loggerTask, "Logger", 4096, NULL, 1, NULL, 1);

    // Core 0: GPS + commands
    xTaskCreatePinnedToCore(gpsTask, "GPS", 4096, NULL, 1, NULL, 0);
    xTaskCreatePinnedToCore(commandTask, "Command", 4096, NULL, 1, NULL, 0);

    // Optional: mode manager as a separate task
    xTaskCreatePinnedToCore(modeManager, "Mode", 4096, NULL, 2, NULL, 0);
}

// ------------------- LOOP ----------------------

void loop() {
    // Nothing here, all tasks run independently
}
