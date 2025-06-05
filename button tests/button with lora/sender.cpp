#include <SPI.h>
#include <LoRa.h>

// LoRa pins
#define LORA_SCK 5
#define LORA_MISO 19
#define LORA_MOSI 27
#define LORA_SS 18
#define LORA_RST 14
#define LORA_DIO0 26

#define BUTTON_PIN 34
#define LORA_FREQ 868E6

bool buttonState = false;
bool lastButtonRead = HIGH;
unsigned long lastDebounceTime = 0;
const unsigned long debounceDelay = 50;

void setup() {
  Serial.begin(115200);
  pinMode(BUTTON_PIN, INPUT); // GPIO34 is input only

  SPI.begin(LORA_SCK, LORA_MISO, LORA_MOSI, LORA_SS);
  LoRa.setPins(LORA_SS, LORA_RST, LORA_DIO0);

  if (!LoRa.begin(LORA_FREQ)) {
    Serial.println("LoRa init failed!");
    while (true);
  }

  Serial.println("Button LoRa transmitter ready");
}

void loop() {
  bool reading = digitalRead(BUTTON_PIN);

  if (reading != lastButtonRead) {
    lastDebounceTime = millis();
  }

  if ((millis() - lastDebounceTime) > debounceDelay) {
    if (lastButtonRead == HIGH && reading == LOW) {
      buttonState = !buttonState;

      // Send state over LoRa
      LoRa.beginPacket();
      LoRa.print(buttonState ? "1" : "0");
      LoRa.endPacket();

      Serial.print("Sent state: ");
      Serial.println(buttonState ? "1" : "0");
    }
  }

  lastButtonRead = reading;
}
