#include <tiny_IRremote.h>
#include <avr/sleep.h>

// IMPORTANT: IRsend only works from PB4 ("pin 4" according to Arduino)
int LED_PIN = PB0;
int SWITCH_PIN = PB3;
IRsend irsend;

void setup() {
  pinMode(LED_PIN, OUTPUT);
  pinMode(SWITCH_PIN, INPUT_PULLUP);

  //must need set
  GIMSK = 0b00100000; //외부인터럽트 0b01000000  핀체인지 인터럽트 0b00100000
  PCMSK = 0b00001000; //Pin Change Mask Register. 어떤핀을 사용할지 선택. (폴링엣지에서 ISR호출됨)
}

void loop() {
  sleep();
  waitInput();
}

void sendSignal() {
  irsend.sendSony(0xa90, 12);
  blink();
}

void blink() {
  digitalWrite(LED_PIN, HIGH);
  delay(500);
  digitalWrite(LED_PIN, LOW);
  delay(500);
}

void sleep() {
  ADCSRA &= ~_BV(ADEN);                   // ADC off
  set_sleep_mode(SLEEP_MODE_PWR_DOWN);    // replaces above statement

  sleep_enable();                         // Sets the Sleep Enable bit in the MCUCR Register (SE BIT)
  sei();                                  // Enable interrupts
  sleep_cpu();                            // sleep

  cli();                                  // Disable interrupts
  sleep_disable();                        // Clear SE bit
  ADCSRA |= _BV(ADEN);                    // ADC on

  sei();                                  // Enable interrupts
}

ISR(PCINT0_vect)
{
}

void waitInput() {
  for (int i = 0; i <= 30000; i++) {
    if (digitalRead(SWITCH_PIN) == 1) {
      delayMicroseconds(300);
    } else {
      sendSignal();
    }
  }
}


//  void sendNEC(unsigned long data, int nbits);
//  void sendSony(unsigned long data, int nbits);
//  void sendRaw(unsigned int buf[], int len, int hz);
//  void sendRC5(unsigned long data, int nbits);
//  void sendRC6(unsigned long data, int nbits);
