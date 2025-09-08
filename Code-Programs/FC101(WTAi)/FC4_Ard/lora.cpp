
#include <LoRa.h>
#include "lora.h"
#include "utils.h"
#include <SPI.h>

#include "imu.h"
#include "bmp180.h"
#include "config.h"

#define LORA_SS 7
#define LORA_RST 6
#define LORA_DIO0 2
#define LORA_FREQ 433E6

bool initLoRa() {
    LoRa.setPins(LORA_SS, LORA_RST, LORA_DIO0);

    if (!LoRa.begin(LORA_FREQ)) {
        LoRa.setTxPower(10); // 10 dBm = 10 mW

        
        return false;
    } else {
        
        return true;
    }
}

String checkCmd() {
    int packetSize = LoRa.parsePacket();
    if (packetSize) {
        String incoming = "";
        while (LoRa.available()) {
            incoming += (char)LoRa.read();
        }
        return incoming;
    }
    return "";  // No message
}


void sendStatus(String msg) {
    LoRa.beginPacket();
    LoRa.print(msg);
    LoRa.endPacket();
}


void sendData() {
    float ax, ay, az, gx, gy, gz;
    readIMU(ax, ay, az, gx, gy, gz);

    float temp, pressure, altitude;
    readBMP(temp, pressure, altitude);

    String data = String(millis()) + "," +
                  String(ax) + "," + String(ay) + "," + String(az) + "," +
                  String(gx) + "," + String(gy) + "," + String(gz) + "," +
                  String(temp) + "," + String(pressure) + "," + String(altitude);

    sendStatus(data);
}
