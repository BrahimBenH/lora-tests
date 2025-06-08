#define BUTTON_PIN 16  // GPIO16 supports internal pull-up

bool buttonState = false;
bool lastButtonReading = HIGH;
unsigned long lastDebounceTime = 0;
const unsigned long debounceDelay = 50;

void setup() {
  Serial.begin(115200);
  pinMode(BUTTON_PIN, INPUT_PULLUP);  // Enable internal pull-up
  Serial.println("TTGO Button Toggle Ready (GPIO16)");
}

void loop() {
  bool reading = digitalRead(BUTTON_PIN);

  
    if (lastButtonReading == 1 && reading == 0) {
      buttonState = !buttonState;
      Serial.print("Button toggled to: ");
      Serial.println(buttonState);
    }
  
      Serial.println(buttonState);

  lastButtonReading = reading;
}
