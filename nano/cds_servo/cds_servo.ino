#include <Servo.h>

Servo myservo;

int pos = 0;
int cdsVal;

void setup() {
  Serial.begin(9600);
  myservo.attach(8);
  myservo.write(0);
}

void loop() {
  cdsVal = analogRead(A0);

  if(cdsVal > 500) {
    Serial.println("***Hit***");
    myservo.write(90);
    delay(500);
    myservo.write(0);
  }

  // Serial.println(cdsVal);
  // delay(500);
}
//laser pointer
//330ohm normal 0-10 / 60
//10K ohm normal 100-230 / 600 