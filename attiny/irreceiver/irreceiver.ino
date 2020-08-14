/*
 * IRremote: IRrecvDemo - demonstrates receiving IR codes with IRrecv
 * An IR detector/demodulator must be connected to the input RECV_PIN.
 * Version 0.1 July, 2009
 * Copyright 2009 Ken Shirriff
 * http://arcfn.com
 */

#include <tiny_IRremote.h>

int RECV_PIN = PB0;
int LED_PIN = PB4;
bool flag = true;

IRrecv irrecv(RECV_PIN);

decode_results results;

void setup()
{
  
//  Serial.begin(9600);
  // In case the interrupt driver crashes on setup, give a clue
  // to the user what's going on.
//  Serial.println("Enabling IRin");
  pinMode(LED_PIN,OUTPUT);
  irrecv.enableIRIn(); // Start the receiver
//  Serial.println("Enabled IRin");
}

void loop() {

if(flag){
   ledOn();
} else {
  ledOff();
}

  if (irrecv.decode(&results)) {
    flag = !flag;
    irrecv.resume(); // Receive the next value
  }
  delay(100);
}

void ledOn(){
  digitalWrite(LED_PIN,HIGH);
}

void ledOff(){
  digitalWrite(LED_PIN,LOW);
}
