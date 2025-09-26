#include "sd.h"
#include <SPI.h>
#include <SD.h>
#include "config.h"

File logFile;

// Initialize SD card
bool initSD(uint8_t SD_CS) {
    if (!SD.begin(SD_CS)) {
        //Serial.println("SD init failed!");
        return false;
    }

    // Create file with header if not exists
    if (!SD.exists("/log.csv")) {
        logFile = SD.open("/log.csv", FILE_WRITE);
        if (logFile) {
            logFile.println("time,ax,ay,az,gx,gy,gz,pressure,temperature,alti");
            logFile.close();
        }
    }
    return true;
}

// Log data in CSV format
void logData(const SensorData &data) {
    logFile = SD.open("/log.csv", FILE_APPEND);
    if (logFile) {
        logFile.print(data.time);
        logFile.print(",");
        logFile.print(data.ax);
        logFile.print(",");
        logFile.print(data.ay);
        logFile.print(",");
        logFile.print(data.az);
        logFile.print(",");
        logFile.print(data.gx);
        logFile.print(",");
        logFile.print(data.gy);
        logFile.print(",");
        logFile.print(data.gz);
        logFile.print(",");
        logFile.print(data.pre);
        logFile.print(",");
        logFile.print(data.temp);
        logFile.print(",");
        logFile.println(data.alti);

        logFile.close();
    }
}
