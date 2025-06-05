#include <SPI.h>
#include <LoRa.h>

#define LORA_SCK 5
#define LORA_MISO 19
#define LORA_MOSI 27
#define LORA_SS 18
#define LORA_RST 14
#define LORA_DIO0 26

#define BUTTON_PIN 16  // GPIO16 supports internal pull-up
#define LORA_FREQ 868E6

bool buttonState = false;
bool lastButtonReading = HIGH;
unsigned long lastDebounceTime = 0;
const unsigned long debounceDelay = 50;

void setup() {
  Serial.begin(115200);
  pinMode(BUTTON_PIN, INPUT_PULLUP);

  SPI.begin(LORA_SCK, LORA_MISO, LORA_MOSI, LORA_SS);
  LoRa.setPins(LORA_SS, LORA_RST, LORA_DIO0);
  if (!LoRa.begin(LORA_FREQ)) {
    Serial.println("LoRa init failed");
    while (1);
  }

  Serial.println("LoRa Button Sender Ready");
}

void loop() {
  bool reading = digitalRead(BUTTON_PIN);

  if (lastButtonReading == 0 && reading == 1) {
      buttonState = !buttonState;
      Serial.print("Button toggled to: ");
      Serial.println(buttonState ? "1" : "0");

      // Send via LoRa
      LoRa.beginPacket();
      LoRa.print(buttonState ? "1" : "0");
      LoRa.endPacket();
    }
  

  lastButtonReading = reading;
}
