#include <avr/sleep.h>
#include <SoftwareSerial.h>

int wakePin = 2;                 // pin used for waking up
long startTime;

SoftwareSerial dbgSerial(0, 1); // RX, TX

void setup() {
  pinMode(wakePin, INPUT);  
  startTime = millis();  
  dbgSerial.begin(9600); //can't be faster than 19200 for softserial  
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
  
  if ((millis() - startTime)/1000 > 10) {
    startTime = millis();
    sleepNow();
  }
}
