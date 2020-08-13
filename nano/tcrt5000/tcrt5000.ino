const int analogInput = A3;
const int digitalInput = 5;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  pinMode(analogInput, INPUT);
  pinMode(digitalInput, INPUT);
}

void loop() {
  // put your main code here, to run repeatedly:

  if (digitalRead(digitalInput) == HIGH) {
    Serial.println("digital HIGH");
  } 
  
  if (digitalRead(digitalInput) == LOW) {
    Serial.println("digital LOW");
  }

  int input = analogRead(analogInput);
//  int p = map(input,0,700,0,100);
  Serial.println(input);

  delay(100);
}
