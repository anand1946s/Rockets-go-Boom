#include "gps.h"
#include <HardwareSerial.h>
#include <TinyGPS++.h>

// GPS setup
HardwareSerial gpsSerial(1); // Use UART1 (pins defined in config)
TinyGPSPlus gps;

// Globals for last known location
float latitude = 0.0;
float longitude = 0.0;

// Initialize GPS
bool initGPS() {
    gpsSerial.begin(9600, SERIAL_8N1, 16, 17); // RX=16, TX=17 (example pins)
    delay(1000); // let GPS stabilize
    return true; // assume success
}

// Read GPS data (call in FreeRTOS task)
void readGPS() {
    while (gpsSerial.available() > 0) {
        char c = gpsSerial.read();
        gps.encode(c);
    }

    if (gps.location.isUpdated()) {
        latitude = gps.location.lat();
        longitude = gps.location.lng();
    }
}

// Get last known coordinates
float getLatitude() { return latitude; }
float getLongitude() { return longitude; }
