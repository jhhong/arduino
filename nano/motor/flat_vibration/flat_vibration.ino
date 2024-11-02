void setup() {
  Serial.begin(115200);
  pinMode(11, OUTPUT);
}

void loop() {
  for(int i=100;i<255;i+=50) {
    Serial.println(i);
    analogWrite(11, i);
    delay(3000);
  }
}
