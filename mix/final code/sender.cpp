
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
#define BUTTON_COOLDOWN 1000
#define CAR_ID 1

#define RED_LED 33  // GPIO red led toggled
#define BLUE_LED 23  // GPIO red led toggled



TinyGPSPlus gps;
HardwareSerial gpsSerial(1);
bool buttonState = false;
bool lastButtonReading = HIGH;
unsigned long lastToggleTime = 0;

void setup() {
    pinMode(RED_LED, OUTPUT); // Set pin as output
    pinMode(BLUE_LED, OUTPUT); // Set pin as output

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

  // Create JSON packet
  String json = "{";
  json += "\"i\":" + String(CAR_ID) + ",";
  json += "\"la\":" + String(lat) + ",";
  json += "\"lo\":" + String(lng) + ",";
  json += "\"w\":" + String(buttonState? "1" : "0");
  json += "}";
        digitalWrite(RED_LED,buttonState);


  LoRa.beginPacket();
  LoRa.print(json);
  LoRa.endPacket();

  Serial.print("Sent: "); Serial.println(json);

 
    
  
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
      Serial.print("Received (from blue): ");
      Serial.println(received);
      int value = received.toInt(); // Converts "1" to 1, "0" to 0
digitalWrite(BLUE_LED, value);
    }
  }

  Serial.println("Done listening\n");
}

