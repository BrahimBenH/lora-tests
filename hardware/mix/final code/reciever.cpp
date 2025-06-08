
#include <SPI.h>
#include <LoRa.h>

#define LORA_SCK 5
#define LORA_MISO 19
#define LORA_MOSI 27
#define LORA_SS 18
#define LORA_RST 14
#define LORA_DIO0 26

#define BUTTON_PIN 16
#define FREQ 868E6
#define LISTEN_TIME 3000

#define BLUE_LED 13  // GPIO red led toggled

unsigned long lastToggleTime = 0;      // Timestamp of last button toggle
#define BUTTON_COOLDOWN 1000           // Cooldown time in milliseconds (1 second)


bool buttonState = false;
bool lastButtonReading = HIGH;

void setup() {
      pinMode(BLUE_LED, OUTPUT); // Set pin as output

  Serial.begin(115200);
  pinMode(BUTTON_PIN, INPUT_PULLUP);

  SPI.begin(LORA_SCK, LORA_MISO, LORA_MOSI, LORA_SS);
  LoRa.setPins(LORA_SS, LORA_RST, LORA_DIO0);
  if (!LoRa.begin(FREQ)) {
    while (1);
  }

}

void loop() {

  unsigned long now = millis();
  unsigned long startListening = now;
  
  while (millis() - startListening < LISTEN_TIME) {

    bool reading = digitalRead(BUTTON_PIN);

    if (lastButtonReading == HIGH && reading == LOW && (now - lastToggleTime > BUTTON_COOLDOWN)) {
      buttonState = !buttonState;
      lastToggleTime = now;
     
    }
    lastButtonReading = reading;
    int packetSize = LoRa.parsePacket();
    if (packetSize) {
      String received = LoRa.readString();
   
      Serial.println(received);
    }
  }

  String packet = (buttonState ? "1" : "0");
  LoRa.beginPacket();
  LoRa.print(packet);
  LoRa.endPacket();

        digitalWrite(BLUE_LED,buttonState);


}
