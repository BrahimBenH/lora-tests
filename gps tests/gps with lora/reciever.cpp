#include <SPI.h>
#include <LoRa.h>

#define LORA_SCK 5
#define LORA_MISO 19
#define LORA_MOSI 27
#define LORA_SS 18
#define LORA_RST 14
#define LORA_DIO0 26

#define FREQ 868E6

void setup() {
  Serial.begin(115200);

  SPI.begin(LORA_SCK, LORA_MISO, LORA_MOSI, LORA_SS);
  LoRa.setPins(LORA_SS, LORA_RST, LORA_DIO0);

  if (!LoRa.begin(FREQ)) {
    Serial.println("LoRa init failed!");
    while (true);
  }

  Serial.println("GPS Receiver Ready");
}

void loop() {
  int packetSize = LoRa.parsePacket();
  if (packetSize) {
    String received = LoRa.readString();
    Serial.print("Received GPS: ");
    Serial.println(received);
  }
}
