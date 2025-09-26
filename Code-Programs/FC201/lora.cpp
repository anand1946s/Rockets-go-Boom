#include "lora.h"
#include <LoRa.h>

// Pin configuration (adjust to your wiring)
#define LORA_SS   18
#define LORA_RST  14
#define LORA_DIO0 26

// Frequency (region-specific: 433E6, 866E6, 915E6)
#define LORA_FREQUENCY  433E6

// Global command string
String cmd = "";

bool initLoRa() {
    LoRa.setPins(LORA_SS, LORA_RST, LORA_DIO0);
    if (!LoRa.begin(LORA_FREQUENCY)) {
        return false;
    }

    // Optional tuning for stability
    LoRa.setSpreadingFactor(7);       // 6–12
    LoRa.setSignalBandwidth(125E3);   // 7.8E3 – 500E3
    LoRa.setCodingRate4(5);           // 5–8
    LoRa.enableCrc();

    return true;
}

void sendCommand(const String &msg) {
    LoRa.beginPacket();
    LoRa.print(msg);
    LoRa.endPacket();
}

String receiveData() {
    int packetSize = LoRa.parsePacket();
    if (packetSize) {
        cmd = "";
        while (LoRa.available()) {
            cmd += (char)LoRa.read();
        }
        return cmd;
    }
    return "";
}

bool sendData(const SensorData &data) {
    // Serialize the data into a string or byte array
    String payload = String(data.time) + "," +
                     String(data.ax, 2) + "," + String(data.ay, 2) + "," + String(data.az, 2) + "," +
                     String(data.gx, 2) + "," + String(data.gy, 2) + "," + String(data.gz, 2) + "," +
                     String(data.alti, 2) + "," +
                     String(data.temp, 2) + "," + String(data.pre, 2);

    LoRa.beginPacket();
    LoRa.print(payload);
    int res = LoRa.endPacket();

    return (res == 1); // returns true if transmission succeeded
}
