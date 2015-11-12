#include <avr/sleep.h>
#include <SoftwareSerial.h>

 int pirPin = 2;     // inputPin을 2번에 추가
 
SoftwareSerial dbgSerial(0, 1); // RX, TX
  
void setup() {
  pinMode(pirPin, INPUT);    //inputPin을 입력모드로  
  dbgSerial.begin(9600);           //시리얼 통신을, 9600속도로 받습니다. ( 숫자 조정은 자유)
  dbgSerial.println("setup");  
}

void loop(){

  dbgSerial.println("test1");
  delay(1000);
  
  int result = digitalRead(pirPin);
  
  if (result == HIGH) {
    dbgSerial.println("HIGH");      
  } else {
    dbgSerial.println("LOW");      
  }
  
    sleepNow();  
} 

void wakeUpNow()        // here the interrupt is handled after wakeup
{
    dbgSerial.println("wake up");
}

void sleepNow()         // here we put the arduino to sleep
{
    dbgSerial.println("Entering sleep mode");  
    set_sleep_mode(SLEEP_MODE_IDLE);   // sleep mode is set here
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
