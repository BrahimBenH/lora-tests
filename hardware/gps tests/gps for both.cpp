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

// GPS pins
#define GPS_RX 15
#define GPS_TX 4
#define GPS_BAUD 9600

#define LORA_FREQ 868E6
#define LISTEN_TIME 2000  // 2s listen after sending

TinyGPSPlus gps;
HardwareSerial gpsSerial(1);

unsigned long lastCommTime = 0;
unsigned long nextCommDelay = 0;

void setup() {
  Serial.begin(115200);
  gpsSerial.begin(GPS_BAUD, SERIAL_8N1, GPS_RX, GPS_TX);

  SPI.begin(LORA_SCK, LORA_MISO, LORA_MOSI, LORA_SS);
  LoRa.setPins(LORA_SS, LORA_RST, LORA_DIO0);

  if (!LoRa.begin(LORA_FREQ)) {
    Serial.println("LoRa init failed!");
    while (true);
  }

  randomSeed(analogRead(0));
  nextCommDelay = random(3000, 8000); // 3–8 seconds

  Serial.println("Bi-directional GPS LoRa Ready");
}

void loop() {
  // Always update GPS
  while (gpsSerial.available()) {
    gps.encode(gpsSerial.read());
  }

  // Always listen for incoming packets
  int packetSize = LoRa.parsePacket();
  if (packetSize) {
    String received = LoRa.readString();
    Serial.print("Received while idle: ");
    Serial.println(received);
  }

  unsigned long now = millis();
  if (now - lastCommTime >= nextCommDelay && gps.location.isUpdated()) {
    lastCommTime = now;
    nextCommDelay = random(3000, 8000); // reset delay

    float lat = gps.location.lat();
    float lng = gps.location.lng();

    String msg = String(lat, 6) + "," + String(lng, 6);
    LoRa.beginPacket();
    LoRa.print(msg);
    LoRa.endPacket();
    Serial.print("Sent GPS: ");
    Serial.println(msg);

    // Post-send listening window
    Serial.println("Listening after send...");
    unsigned long listenStart = millis();
    while (millis() - listenStart < LISTEN_TIME) {
      int packetSize = LoRa.parsePacket();
      if (packetSize) {
        String received = LoRa.readString();
        Serial.print("Received after send: ");
        Serial.println(received);
        break;
      }
    }
    Serial.println("Post-send listen window closed\n");
  }
}
