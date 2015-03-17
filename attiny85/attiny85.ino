#include <avr/sleep.h>
#include <SoftwareSerial.h>

int latchPin = 1;
int clockPin = 0;
int dataPin = 2;
int micPin = A2;
int wakePin = 2;                 // pin used for waking up
int mode = 0;
long startTime;

byte changeValue[8];
byte data[6];
int registerCount = 1;

SoftwareSerial dbgSerial(0, 1); // RX, TX
   
void blinkOn(int duration)
{
  changeValue[0] = 0b11111111;
  for(int i = 1; i <= 7; i ++) {
    changeValue[i] = changeValue[i - 1]; 
  }

  for (int i = 0; i < registerCount; i++) {

    int reverseIndex = registerCount - i - 1;

    for (int pinIndex = 0; pinIndex < 8; pinIndex++ ) {
      data[reverseIndex] = changeValue[pinIndex];

      digitalWrite(latchPin, LOW);

      for (int k = 0; k < registerCount; k++) {
        shiftOut(dataPin, clockPin, LSBFIRST, data[k]);
      }

      digitalWrite(latchPin, HIGH);   // 74hc595 출력핀으로 출력 시키기
    }

    data[reverseIndex] = 0b00000000;
  }

  delay(duration);
}

void turnOff(int duration)
{ 
  for (int i = 0; i < registerCount; i++) {

    int reverseIndex = registerCount - i - 1;

    for (int pinIndex = 0; pinIndex < 8; pinIndex++ ) {
      data[reverseIndex] = changeValue[pinIndex];

      digitalWrite(latchPin, LOW);

      for (int k = 0; k < registerCount; k++) {
        shiftOut(dataPin, clockPin, LSBFIRST, 0b00000000);
      }

      digitalWrite(latchPin, HIGH);   // 74hc595 출력핀으로 출력 시키기
    }

    data[reverseIndex] = 0b00000000;
  }

  delay(duration);  
}

void rolling(int duration)
{ 
  changeValue[0] = 0b10000000;
  for(int i = 1; i <= 7; i ++) {
    changeValue[i] = changeValue[i - 1] >> 1; 
  }

  for (int i = 0; i < registerCount; i++) {

    int reverseIndex = registerCount - i - 1;

    for (int pinIndex = 0; pinIndex < 8; pinIndex++ ) {
      data[reverseIndex] = changeValue[pinIndex];

      digitalWrite(latchPin, LOW);

      for (int k = 0; k < registerCount; k++) {
        shiftOut(dataPin, clockPin, LSBFIRST, data[k]);
      }

      digitalWrite(latchPin, HIGH);   // 74hc595 출력핀으로 출력 시키기

      delay(duration);
    }

    data[reverseIndex] = 0b00000000;
  }

  delay(duration);
}

void setup() {
  pinMode(latchPin, OUTPUT);       // 핀들 출력설정
  pinMode(clockPin, OUTPUT);
  pinMode(dataPin, OUTPUT);
  pinMode(micPin, INPUT);
  pinMode(wakePin, INPUT);
  
  for (int i = 0; i < registerCount; i++) {
    data[i] = 0b00000000;
  }
  
  startTime = millis();  
 dbgSerial.begin(9600); //can't be faster than 19200 for softserial  
}

void play()
{
  if (mode % 3 == 0) {
    for(int i=0;i<20;i++) {    
      blinkOn(100);
      turnOff(100);
    }
  } 
  else if (mode %3 == 1) {
    for(int i=0;i<10;i++) {    
      blinkOn(500);
      turnOff(500);    
    }
  } 
  else {
    for(int i=0;i<20;i++) {    
      rolling(30);
    }

    turnOff(30);    
  }
}

void wakeUpNow()        // here the interrupt is handled after wakeup
{
     dbgSerial.println("wake up");    
}

void sleepNow()         // here we put the arduino to sleep
{
     dbgSerial.println("Entering sleep mode");  
    set_sleep_mode(SLEEP_MODE_PWR_DOWN);   // sleep mode is set here
    sleep_enable();          // enables the sleep bit in the mcucr register
                             // so sleep is possible. just a safety pin 

    attachInterrupt(0,wakeUpNow, LOW); // use interrupt 0 (pin 2) and run function
                                       // wakeUpNow when pin 2 gets LOW 

    sleep_mode();            // here the device is actually put to sleep!!
                             // THE PROGRAM CONTINUES FROM HERE AFTER WAKING UP

    sleep_disable();         // first thing after waking from sleep:
                             // disable sleep...
    detachInterrupt(0);      // disables interrupt 0 on pin 2 so the 
                             // wakeUpNow code will not be executed 
                             // during normal running time.
}

void loop() {

  int soundValue = analogRead(micPin);
  if (soundValue > 200) {
    play();
    mode++;
    startTime = millis();
  }
  
  if ((millis() - startTime)/1000 > 60) {
    startTime = millis();
    sleepNow();
  }
}
