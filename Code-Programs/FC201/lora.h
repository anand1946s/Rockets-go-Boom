#ifndef LORA_H
#define LORA_H

#include <Arduino.h>

// Initialize LoRa radio
bool initLoRa();

// Send command/data string
void sendCommand(const String &msg);

bool sendData(const SensorData &data);

// Receive data (returns "" if none)
String receiveData();

// Global command string (last received)
extern String cmd;

#endif
