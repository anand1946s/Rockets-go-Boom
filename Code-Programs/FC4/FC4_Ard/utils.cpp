#include "utils.h"
#include "config.h"
#include "imu.h"
#include "bmp180.h"
#include "sdcard.h"
#include "lora.h"
#include <Arduino.h>

// TEMP system check: always OK for now
SysStatus systemCheck() {
    
    return OK;
}


void logData() {
    float ax, ay, az, gx, gy, gz;
    readIMU(ax, ay, az, gx, gy, gz);

    float temp, pressure, altitude;
    readBMP(temp, pressure, altitude);

    String data = String(millis()) + "," +
                  String(ax) + "," + String(ay) + "," + String(az) + "," +
                  String(gx) + "," + String(gy) + "," + String(gz) + "," +
                  String(temp) + "," + String(pressure) + "," + String(altitude);
    datafile.println(data);

    
}
