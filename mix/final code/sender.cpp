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
#define LED_BUTTON 33     // LED toggled by button
#define LED_RECEIVE 23    // LED toggled by received warn
#define SEND_FREQ 868E6
#define RECEIVE_FREQ 866E6
#define LISTEN_TIME 3000
#define BUTTON_COOLDOWN 1000
#define CAR_ID 1

TinyGPSPlus gps;
HardwareSerial gpsSerial(1);
bool buttonState = false;
bool lastButtonReading = HIGH;
unsigned long lastToggleTime = 0;

void setup() {
  Serial.begin(115200);
  gpsSerial.begin(GPS_BAUD, SERIAL_8N1, GPS_RX, GPS_TX);
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  pinMode(LED_BUTTON, OUTPUT);
  pinMode(LED_RECEIVE, OUTPUT);

  SPI.begin(LORA_SCK, LORA_MISO, LORA_MOSI, LORA_SS);
  LoRa.setPins(LORA_SS, LORA_RST, LORA_DIO0);
  if (!LoRa.begin(SEND_FREQ)) {
    Serial.println("LoRa send init failed!");
    while (true);
  }
  Serial.println("GPS Sender Ready with Listen Mode");
}

void loop() {
  // Read GPS
  while (gpsSerial.available()) {
    gps.encode(gpsSerial.read());
  }

  float lat = gps.location.lat() * 1e6;
  float lng = gps.location.lng() * 1e6;

  // Create JSON packet
  String json = "{";
  json += "\"id\":" + String(CAR_ID) + ",";
  json += "\"lat\":" + String((int32_t)lat) + ",";
  json += "\"long\":" + String((int32_t)lng) + ",";
  json += "\"warn\":" + String(buttonState ? "true" : "false");
  json += "}";

  // Send on SEND_FREQ
  LoRa.end();
  LoRa.begin(SEND_FREQ);
  LoRa.beginPacket();
  LoRa.print(json);
  LoRa.endPacket();

  Serial.print("Sent: ");
  Serial.println(json);
  digitalWrite(LED_BUTTON, buttonState);

  // Listen on RECEIVE_FREQ
  LoRa.end();
  LoRa.begin(RECEIVE_FREQ);
  unsigned long listenStart = millis();
  while (millis() - listenStart < LISTEN_TIME) {
    bool reading = digitalRead(BUTTON_PIN);
    unsigned long now = millis();
    if (lastButtonReading == HIGH && reading == LOW && (now - lastToggleTime > BUTTON_COOLDOWN)) {
      buttonState = !buttonState;
      lastToggleTime = now;
      Serial.print("Red button toggled: ");
      Serial.println(buttonState ? "1" : "0");
    }
    lastButtonReading = reading;

    int packetSize = LoRa.parsePacket();
    if (packetSize) {
      String received = LoRa.readString();
      Serial.print("Received: ");
      Serial.println(received);
      if (received == "1") {
        digitalWrite(LED_RECEIVE, HIGH);
      } else {
        digitalWrite(LED_RECEIVE, LOW);
      }
    }
  }

  Serial.println("Done listening\n");
}
