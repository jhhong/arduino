const int ledpin = 13;
const int sensor = 0;

void setup() {
  pinMode(ledpin , OUTPUT);
  Serial.begin(9600);
}

void loop() {
  
  int rate = analogRead(sensor);
  
  digitalWrite(ledpin , HIGH);
  Serial.println(rate);
  delay(rate);
  digitalWrite(ledpin , LOW);
  delay(rate);
//  Serial.println("LOW=%d",rate);  
  
}
