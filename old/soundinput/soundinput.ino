int ledPin = 13;

void setup() {
  pinMode(ledPin, OUTPUT);       // 핀들 출력설정  
  Serial.begin(9600);  
}

void loop() {
    
  int v = analogRead(A0);
  
  if (v > 400 ) {
    Serial.println(v);
    Serial.println("\n");    
    digitalWrite(ledPin, HIGH);
    delay(200);
    digitalWrite(ledPin, LOW);    
  }
}

