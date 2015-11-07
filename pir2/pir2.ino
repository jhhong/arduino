#include <avr/sleep.h>
#include <SoftwareSerial.h>

int ledPin1 = 3;       // ledPin을 13번에 추가
int ledPin2 = 4;       // ledPin을 13번에 추가
int ledPin3 = 5;       // ledPin을 13번에 추가
int pirPin = 2;     // inputPin을 2번에 추가
long startTime = 0;
bool needLedOn = false;

//SoftwareSerial dbgSerial(0, 1); // RX, TX
  
void setup() {
  pinMode(pirPin, INPUT);    //inputPin을 입력모드로
  pinMode(ledPin1, OUTPUT);    //ledPin을 출력모드로
  pinMode(ledPin2, OUTPUT);    //ledPin을 출력모드로
  pinMode(ledPin3, OUTPUT);    //ledPin을 출력모드로  
//  dbgSerial.begin(9600);           //시리얼 통신을, 9600속도로 받습니다. ( 숫자 조정은 자유)
}

void loop(){

  if (needLedOn) {
    ledOn();    
    delay(10000);    
    needLedOn = false;
  } else {
    ledOff();    
    sleepNow();
  }
} 

void ledOn(){
  digitalWrite(ledPin1,HIGH);
  digitalWrite(ledPin2,HIGH);
  digitalWrite(ledPin3,HIGH);  
}

void ledOff(){
  digitalWrite(ledPin1,LOW);
  digitalWrite(ledPin2,LOW);
  digitalWrite(ledPin3,LOW);  
}

void wakeUpNow()        // here the interrupt is handled after wakeup
{
//    sleep_disable();         // first thing after waking from sleep:  
//    detachInterrupt(0);      // disables interrupt 0 on pin 2 so the     
//    dbgSerial.println("wake up");
    needLedOn = true;
}

void sleepNow()         // here we put the arduino to sleep
{
//    sleep_enable();
//    attachInterrupt(0,wakeUpNow, HIGH); // use interrupt 0 (pin 2) and run function
//    set_sleep_mode(SLEEP_MODE_PWR_DOWN);   // sleep mode is set here
//    sleep_cpu();

//    dbgSerial.println("Entering sleep mode");  
    set_sleep_mode(SLEEP_MODE_PWR_DOWN);   // sleep mode is set here
    sleep_enable();          // enables the sleep bit in the mcucr register
                             // so sleep is possible. just a safety pin 

    attachInterrupt(0,wakeUpNow, HIGH); // use interrupt 0 (pin 2) and run function
                                       // wakeUpNow when pin 2 gets LOW 

    sleep_mode();            // here the device is actually put to sleep!!
                             // THE PROGRAM CONTINUES FROM HERE AFTER WAKING UP

    sleep_disable();         // first thing after waking from sleep:
                             // disable sleep...
    detachInterrupt(0);      // disables interrupt 0 on pin 2 so the 
                             // wakeUpNow code will not be executed 
                             // during normal running time.
}
