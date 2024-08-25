/*
   IRremote: IRrecvDemo - demonstrates receiving IR codes with IRrecv
   An IR detector/demodulator must be connected to the input RECV_PIN.
   Version 0.1 July, 2009
   Copyright 2009 Ken Shirriff
   http://arcfn.com
*/

#include <tiny_IRremote.h>

int RECV_PIN = PB0;
int LED_PIN = PB4;
bool flag = true;
bool shortTermBlock = false;
unsigned long shortTermBlockTime = 0; //push 버튼 여러번 눌리지 않도록 체크
int shortTermBlockTimeout = 200; //0.2 sec

IRrecv irrecv(RECV_PIN);

decode_results results;

void setup()
{

  //  Serial.begin(9600);
  // In case the interrupt driver crashes on setup, give a clue
  // to the user what's going on.
  //  Serial.println("Enabling IRin");
  pinMode(LED_PIN, OUTPUT);
  irrecv.enableIRIn(); // Start the receiver
  //  Serial.println("Enabled IRin");
}

void loop() {

  if (canExecutable(shortTermBlockTime, shortTermBlockTimeout)) {
    shortTermBlock = false;
  }

  if (irrecv.decode(&results) && !shortTermBlock) {
    flag = !flag;
    shortTermBlock = true;
    shortTermBlockTime = millis();
    toggleLed();
    irrecv.resume(); // Receive the next value
  }
}

bool canExecutable(unsigned long startTime, int limit) {
  unsigned long diff = millis() - startTime;
  return diff > limit ? true : false;
}

void toggleLed() {
  if (flag) {
    ledOn();
  } else {
    ledOff();
  }
}

void ledOn() {
  digitalWrite(LED_PIN, HIGH);
}

void ledOff() {
  digitalWrite(LED_PIN, LOW);
}
