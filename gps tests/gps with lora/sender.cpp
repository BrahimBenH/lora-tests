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

#define GPS_RX 15
#define GPS_TX 4
#define GPS_BAUD 9600

#define FREQ 868E6

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

  Serial.println("GPS Sender Ready");
}

void loop() {
  while (gpsSerial.available()) {
    gps.encode(gpsSerial.read());
  }

  if (gps.location.isUpdated()) {
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

    delay(3000);  // wait 3s before next send
  }
}
