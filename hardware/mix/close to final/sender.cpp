
#include <TinyGPS++.h>
#include <HardwareSerial.h>
#include <SPI.h>
#include <LoRa.h>

#define LORA_SCK 5
#define LORA_MISO 19
#define LORA_MOSI 27
#define LORA_SS 18
#define LORA_RST 14
#define LORA_DIO0 26

#define GPS_RX 4
#define GPS_TX 15
#define GPS_BAUD 9600

#define BUTTON_PIN 16
#define FREQ 868E6
#define LISTEN_TIME 3000

TinyGPSPlus gps;
HardwareSerial gpsSerial(1);
bool buttonState = false;
bool lastButtonReading = HIGH;

void setup() {
  Serial.begin(115200);
  gpsSerial.begin(GPS_BAUD, SERIAL_8N1, GPS_RX, GPS_TX);
  pinMode(BUTTON_PIN, INPUT_PULLUP);

  SPI.begin(LORA_SCK, LORA_MISO, LORA_MOSI, LORA_SS);
  LoRa.setPins(LORA_SS, LORA_RST, LORA_DIO0);
  if (!LoRa.begin(FREQ)) {
    Serial.println("LoRa init failed!");
    while (true);
  }
  Serial.println("GPS Sender Ready with Listen Mode");
}

void loop() {
  while (gpsSerial.available()) {
    gps.encode(gpsSerial.read());
  }

  float lat = gps.location.lat();
  float lng = gps.location.lng();

  String packet = String("red:") + (buttonState ? "1" : "0");
  packet += ",GPS:" + String(lat, 6) + "," + String(lng, 6);

  LoRa.beginPacket();
  LoRa.print(packet);
  LoRa.endPacket();

  Serial.print("Sent: "); Serial.println(packet);

  unsigned long listenStart = millis();
  while (millis() - listenStart < LISTEN_TIME) {
    
  bool reading = digitalRead(BUTTON_PIN);
  if (lastButtonReading == HIGH && reading == LOW) {
    buttonState = !buttonState;
    Serial.print("Red button toggled: ");
    Serial.println(buttonState ? "1" : "0");
  }
  lastButtonReading = reading;

    int packetSize = LoRa.parsePacket();
    if (packetSize) {
      String received = LoRa.readString();
      Serial.print("Received (from blue): ");
      Serial.println(received);
    }
  }

  Serial.println("Done listening\n");
}

