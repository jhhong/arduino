void setup() {
Serial.begin(9600);
}


void loop() {
  
  int aa = analogRead(A0);

  Serial.print(aa);  
}
