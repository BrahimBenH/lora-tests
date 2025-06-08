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
#define LISTEN_TIME 3000  // milliseconds

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

  Serial.println("LoRa Button Sender + GPS Listener Ready");
}

void loop() {
  
    // Listen for GPS packets instead of delay
    unsigned long startListening = millis();
    while (millis() - startListening < LISTEN_TIME) {
      bool reading = digitalRead(BUTTON_PIN);

  // Check for button toggle
  if (lastButtonReading == HIGH && reading == LOW) {
    buttonState = !buttonState;
    Serial.print("Button toggled to: ");
    Serial.println(buttonState ? "1" : "0");

  }
    lastButtonReading = reading;

      int packetSize = LoRa.parsePacket();
      if (packetSize) {
        String received = LoRa.readString();
        Serial.print("Received GPS: ");
        Serial.println(received);
      }
    }
    
    // Send via LoRa
     Serial.print("current state: ");
    Serial.println(buttonState ? "1" : "0");

    LoRa.beginPacket();
    LoRa.print(buttonState ? "1" : "0");
    LoRa.endPacket();

}
