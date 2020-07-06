/*
 * IRremote: IRrecvDemo - demonstrates receiving IR codes with IRrecv
 * An IR detector/demodulator must be connected to the input RECV_PIN.
 * Version 0.1 July, 2009
 * Copyright 2009 Ken Shirriff
 * http://arcfn.com
 */

#include <IRremote.h>

int RECV_PIN = PB0;
int LED_PIN = PB4;

IRrecv irrecv(RECV_PIN);

decode_results results;

void setup()
{
  ledOn();
//  Serial.begin(9600);
  // In case the interrupt driver crashes on setup, give a clue
  // to the user what's going on.
//  Serial.println("Enabling IRin");
  irrecv.enableIRIn(); // Start the receiver
//  Serial.println("Enabled IRin");
}

void loop() {
  if (irrecv.decode(&results)) {
//    Serial.println(results.value, HEX);
ledOn();
    irrecv.resume(); // Receive the next value
  }
  delay(100);
}

void ledOn(){
  digitalWrite(LED_PIN,HIGH);
  delay(1000);
  digitalWrite(LED_PIN,LOW);
}
