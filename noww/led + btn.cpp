#define BUTTON_PIN 16
#define LED1 13
#define LED2 12

bool enabled = false;
bool lastButtonState = HIGH;
unsigned long lastDebounceTime = 0;
const unsigned long debounceDelay = 50;

void setup() {
  pinMode(LED1, OUTPUT);
  pinMode(LED2, OUTPUT);
  pinMode(BUTTON_PIN, INPUT_PULLUP); // Button connected between pin 16 and GND
}

void loop() {
  bool currentButtonState = digitalRead(BUTTON_PIN);

  if (lastButtonState == HIGH && currentButtonState == LOW) {
    if (millis() - lastDebounceTime > debounceDelay) {
      enabled = !enabled;
      lastDebounceTime = millis();
    }
  }
  lastButtonState = currentButtonState;

  if (enabled) {
    digitalWrite(LED1, HIGH);
    digitalWrite(LED2, LOW);
    delay(500);
    digitalWrite(LED1, LOW);
    digitalWrite(LED2, HIGH);
    delay(500);
  } else {
    digitalWrite(LED1, LOW);
    digitalWrite(LED2, LOW);
  }
}
