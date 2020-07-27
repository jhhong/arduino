int LED_PIN = PB0;

void setup() {
  pinMode(LED_PIN, OUTPUT);
}

void loop() {
  blink();
}

void blink() {
  digitalWrite(LED_PIN, HIGH);
  delay(500);
  digitalWrite(LED_PIN, LOW);
  delay(500);
}
