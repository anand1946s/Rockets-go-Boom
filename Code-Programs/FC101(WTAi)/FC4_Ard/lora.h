#ifndef LORA_H
#define LORA_H

#include <Arduino.h>

bool initLoRa();
String checkCmd();
void sendStatus(String msg);
void sendData();

#endif
