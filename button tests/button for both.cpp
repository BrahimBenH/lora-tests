#include <SPI.h>
#include <LoRa.h>

// LoRa pins
#define LORA_SCK 5
#define LORA_MISO 19
#define LORA_MOSI 27
#define LORA_SS 18
#define LORA_RST 14
#define LORA_DIO0 26

#define BUTTON_PIN 34     // Your tactile button pin
#define LORA_FREQ 868E6
#define LISTEN_TIME 2000  // 2 seconds listening after sending

bool buttonState = false;
bool lastButtonRead = HIGH;
unsigned long lastDebounceTime = 0;
const unsigned long debounceDelay = 50;

unsigned long lastCommTime = 0;
unsigned long nextCommDelay = 0;

void setup() {
  Serial.begin(115200);
  pinMode(BUTTON_PIN, INPUT);

  SPI.begin(LORA_SCK, LORA_MISO, LORA_MOSI, LORA_SS);
  LoRa.setPins(LORA_SS, LORA_RST, LORA_DIO0);

  if (!LoRa.begin(LORA_FREQ)) {
    Serial.println("LoRa init failed!");
    while (true);
  }

  randomSeed(analogRead(0));
  nextCommDelay = random(3000, 8000);  // 3–8 seconds interval

  Serial.println("Bi-directional Button LoRa Ready");
}

void loop() {
  // Handle button toggle with debounce
  bool reading = digitalRead(BUTTON_PIN);
  if (reading != lastButtonRead) {
    lastDebounceTime = millis();
  }
  if ((millis() - lastDebounceTime) > debounceDelay) {
    if (lastButtonRead == HIGH && reading == LOW) {
      buttonState = !buttonState;
      Serial.print("Button toggled locally: ");
      Serial.println(buttonState ? 1 : 0);
    }
  }
  lastButtonRead = reading;

  // Always listen for incoming packets
  int packetSize = LoRa.parsePacket();
  if (packetSize) {
    String received = LoRa.readString();
    Serial.print("Received while idle: ");
    Serial.println(received);
  }

  unsigned long now = millis();
  if (now - lastCommTime >= nextCommDelay) {
    lastCommTime = now;
    nextCommDelay = random(3000, 8000);  // new random interval

    // Send button state
    LoRa.beginPacket();
    LoRa.print(buttonState ? "1" : "0");
    LoRa.endPacket();
    Serial.print("Sent button state: ");
    Serial.println(buttonState ? 1 : 0);

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
