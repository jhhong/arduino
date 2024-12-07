int cdsVal;

void setup() {
  Serial.begin(9600);

}

void loop() {
  cdsVal = analogRead(A0);
  Serial.println(cdsVal);
  delay(500);
}
//laser pointer
//330ohm normal 0-10 / 60
//10K ohm normal 100-230 / 600 