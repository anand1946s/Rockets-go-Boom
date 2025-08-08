#include <SPI.h>
#include <LoRa.h>

#define LORA_SS    10
#define LORA_RST   9
#define LORA_DIO0  2
#define LORA_FREQ  433E6

#define LED_PIN    7  // LED connected to digital pin 7

void setup() {
  Serial.begin(9600);
  while (!Serial);

  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW); // Make sure LED is off initially

  Serial.println("LoRa Custom Receiver");
  LoRa.setPins(LORA_SS, LORA_RST, LORA_DIO0);

  if (!LoRa.begin(LORA_FREQ)) {
    Serial.println("Starting LoRa failed!");
    while (1);
  }

  Serial.println("LoRa Initialized! Listening for messages...");
}

void loop() {
  int packetSize = LoRa.parsePacket();

  if (packetSize > 0) {
    String incoming = "";

    while (LoRa.available()) {
      incoming += (char)LoRa.read();
    }

    incoming.trim();

    Serial.print("Received: ");
    Serial.print(incoming);
    Serial.print(" | RSSI: ");
    Serial.println(LoRa.packetRssi());

    // Blink LED on message receive
    digitalWrite(LED_PIN, HIGH);
    delay(200); // LED on for 200 ms
    digitalWrite(LED_PIN, LOW);
  }
}
