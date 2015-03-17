#include <avr/sleep.h>
//#include <SoftwareSerial.h>

int cdsPin = A0;
int pirPin = 8;
int ledPin = 9;
long startTime;
boolean ledOn;

//SoftwareSerial dbgSerial(0, 1); // RX, TX
   
void setup(){
  pinMode(pirPin, INPUT);
  pinMode(ledPin, OUTPUT);  
  ledOn = false;
  Serial.begin(9600);
}

void loop() {

  int cdsValue = analogRead(cdsPin);
  //> 660
  int human = digitalRead(pirPin);
  Serial.println(cdsValue);
  Serial.println(human);
  
  if (cdsValue > 1000 && human && !ledOn) {
    Serial.println("OOn");
    digitalWrite(ledPin,HIGH);
    startTime = millis();
    ledOn = true;
  }
  
  if (ledOn && ((millis() - startTime)/1000) > 10) {
    Serial.println("============Off");    
    digitalWrite(ledPin,LOW);
    ledOn = false;
  }
  
  delay(1000);
}
