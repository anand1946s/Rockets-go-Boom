#include <SPI.h>
#include <LoRa.h>

#define LORA_SS    10
#define LORA_RST   9
#define LORA_DIO0  2
#define LORA_FREQ  433E6

void setup() {
  Serial.begin(9600);


  Serial.println("LoRa Custom Sender");
  LoRa.setPins(LORA_SS, LORA_RST, LORA_DIO0);

  if (!LoRa.begin(LORA_FREQ)) {
    Serial.println("Starting LoRa failed!");
    while (1);
  }

  Serial.println("LoRa Initialized! Type your message:");
}

void loop() {
  // Check if user typed something
  if (Serial.available()) {
    String message = Serial.readStringUntil('\n');  // Read until newline
    message.trim(); // remove any trailing newline or spaces

    if (message.length() > 0) {
      LoRa.beginPacket();
      LoRa.print(message);
      
      LoRa.endPacket();

      Serial.print("Sent: ");
      Serial.println(message);
    }
  }
}
