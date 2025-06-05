#include <TinyGPS++.h>
#include <HardwareSerial.h>
#include <SPI.h>
#include <LoRa.h>

// LoRa pins
#define LORA_SCK 5
#define LORA_MISO 19
#define LORA_MOSI 27
#define LORA_SS 18
#define LORA_RST 14
#define LORA_DIO0 26

#define GPS_RX 4
#define GPS_TX 15
#define GPS_BAUD 9600

#define FREQ 868E6
#define LISTEN_TIME 3000  // ms to listen after sending

TinyGPSPlus gps;
HardwareSerial gpsSerial(1); // UART1 for GPS

void setup() {
  Serial.begin(115200);
  gpsSerial.begin(GPS_BAUD, SERIAL_8N1, GPS_RX, GPS_TX);
  
  SPI.begin(LORA_SCK, LORA_MISO, LORA_MOSI, LORA_SS);
  LoRa.setPins(LORA_SS, LORA_RST, LORA_DIO0);

  if (!LoRa.begin(FREQ)) {
    Serial.println("LoRa init failed!");
    while (true);
  }

  Serial.println("GPS Sender Ready with Listen Mode");
}

void loop() {
  // Read GPS continuously
  while (gpsSerial.available()) {
    gps.encode(gpsSerial.read());
  }

  // If GPS updated, send location
  if (1) {
    float lat = gps.location.lat();
    float lng = gps.location.lng();

    Serial.print("Sending: ");
    Serial.print(lat, 6);
    Serial.print(", ");
    Serial.println(lng, 6);

    LoRa.beginPacket();
    LoRa.print(lat, 6);
    LoRa.print(",");
    LoRa.print(lng, 6);
    LoRa.endPacket();

  }
  
    // Now listen instead of delay
    unsigned long listenStart = millis();
    while (millis() - listenStart < LISTEN_TIME) {
      int packetSize = LoRa.parsePacket();
      if (packetSize) {
        String received = LoRa.readString();
        Serial.print("Received while listening: ");
        Serial.println(received);
      }

    }

    Serial.println("Done listening\n");
}
