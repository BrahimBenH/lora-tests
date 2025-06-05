#define BUTTON_PIN 34

bool buttonState = false;
bool lastButtonRead = HIGH;
unsigned long lastDebounceTime = 0;
const unsigned long debounceDelay = 50;

void setup() {
  Serial.begin(115200);
  pinMode(BUTTON_PIN, INPUT);
}

void loop() {
  bool reading = digitalRead(BUTTON_PIN);

  if (reading != lastButtonRead) {
    lastDebounceTime = millis();
  }

  if ((millis() - lastDebounceTime) > debounceDelay) {
    if (lastButtonRead == HIGH && reading == LOW) {
      buttonState = !buttonState;
      Serial.print("Button state toggled: ");
      Serial.println(buttonState ? 1 : 0);
    }
  }

  lastButtonRead = reading;
}
