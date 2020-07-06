#define C6  1047
#define D6  1175
#define E6  1319
#define F6  1397
#define G6  1568
#define A6  1760
#define B6  1976
#define C7  2093
#define D7  2349
#define E7  2637
#define F7  2794
#define G7  3136
#define A7  3520
#define B7  3951
#define C8  4186
#include <avr/sleep.h>


int sn = PB0;
int sw = PB3;
int i = 0;
int l = 0;
int b = 0;
int t = 0;
int WDC = 3;
long WDPI = 1500000;

void setup() {
  pinMode(sw, INPUT_PULLUP);
  pinMode(sn, OUTPUT); // set a pin for buzzer output

  GIMSK = 0b00100000;
  PCMSK = 0b00001000;
}

void loop() {
  sleep();
  check();
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

void check() {
  for (i = 0; i <= 30000; i++) {
    if (digitalRead(sw) == 1) {
      delayMicroseconds(300);
    } else {
      PUSH();
    }
  }
}


void PUSH() {
  for (i = 0; i <= 5000; i++) {
    if (digitalRead(sw) == 0) {
      b = 1;
      delayMicroseconds(100);
    } else {
      if (b == 1) {
        playSound();
      }
      b = 0;
    }
  }
  scream();
}

void scream() {
  for (i = 0; i <= 9; i++) {
    if (digitalRead(sw) == 1) {
      delayMicroseconds(100);
    }
  }
}

void playSound() {

  if (t % 1 == 0) {
    WDSO1();
  }   else if (t % 2 == 0) {
    WDSO2();
  }   else if (t % 3 == 0) {
    WDSO3();
  }   else if (t % 4 == 0) {
    WDSO4();
  }   else if (t % 5 == 0) {
    WDSO5();
  }   else if (t % 6 == 0) {
    WDSO6();
  }   else if (t % 7 == 0) {
    WDSO7();
  }   else if (t % 8 == 0) {
    WDSO8();
  }   else if (t % 9 == 0) {
    WDSO9();
  }   else if (t % 10 == 0) {
    WDS10();
  }
  delay(200);
  l++;
  t++;
}

void WDSO1() {
  buzz(C6, 125);
  buzz(C6, 125);
  buzz(E6, 125);
  buzz(E6, 125);
  buzz(G6, 125);
  buzz(C7, 125);
  buzz(G6, 250);
  buzz(A6, 125);
  buzz(A6, 125);
  buzz(C7, 125);
  buzz(C7, 125);
  buzz(G6, 500);
  buzz(C7, 125);
  buzz(B6, 125);
  buzz(A6, 125);
  buzz(B6, 125);
  buzz(C7, 125);
  buzz(E7, 125);
  buzz(C7, 250);
  buzz(E7, 125);
  buzz(D7, 125);
  buzz(A6, 125);
  buzz(B6, 125);
  buzz(C7, 500);
}

void WDSO2() {
  buzz(C7, 250);
  buzz(C7, 250);
  buzz(C7, 250);
  buzz(G6, 250);
  buzz(A6, 250);
  buzz(A6, 250);
  buzz(G6, 500);
  buzz(E7, 250);
  buzz(E7, 250);
  buzz(D7, 250);
  buzz(D7, 250);
  buzz(C7, 500);
}

void WDSO3() {
  buzz(E7, 125);
  buzz(F7, 125);
  buzz(G7, 250);
  buzz(G7, 500);
  buzz(A7, 125);
  buzz(B7, 125);
  buzz(C8, 250);
  buzz(C8, 500);
  buzz(E7, 125);
  buzz(F7, 125);
  buzz(G7, 250);
  buzz(G7, 500);
  buzz(A7, 125);
  buzz(G7, 125);
  buzz(F7, 250);
  buzz(F7, 500);
  buzz(E7, 250);
  buzz(G7, 250);
  buzz(C7, 250);
  buzz(E7, 250);
  buzz(D7, 250);
  buzz(F7, 500);
  buzz(B6, 250);
  buzz(C7, 500);
}

void WDSO4() {
  buzz(C6, 125);
  buzz(C6, 125);
  buzz(G6, 125);
  buzz(G6, 125);
  buzz(A6, 125);
  buzz(A6, 125);
  buzz(G6, 250);
  buzz(F6, 125);
  buzz(F6, 125);
  buzz(E6, 125);
  buzz(E6, 125);
  buzz(D6, 125);
  buzz(D6, 125);
  buzz(C6, 250);
}

void WDSO5() {
  buzz(C6, 250);
  buzz(D6, 250);
  buzz(E6, 250);
  buzz(C6, 250);
  buzz(C6, 250);
  buzz(D6, 250);
  buzz(E6, 250);
  buzz(C6, 250);
  buzz(E6, 250);
  buzz(F6, 250);
  buzz(G6, 500);
  buzz(E6, 250);
  buzz(F6, 250);
  buzz(G6, 500);
}

void WDSO6() {
  buzz(G6, 125);
  buzz(G6, 125);
  buzz(A6, 250);
  buzz(G6, 250);
  buzz(C7, 250);
  buzz(B6, 500);
  buzz(G6, 125);
  buzz(G6, 125);
  buzz(A6, 250);
  buzz(G6, 250);
  buzz(D7, 250);
  buzz(C7, 500);
  buzz(G6, 125);
  buzz(G6, 125);
  buzz(G7, 250);
  buzz(E7, 250);
  buzz(C7, 250);
  buzz(B6, 250);
  buzz(A6, 250);
  buzz(F7, 125);
  buzz(F7, 125);
  buzz(E7, 250);
  buzz(C7, 250);
  buzz(D7, 250);
  buzz(C7, 500);
}

void WDSO7() {
  buzz(C7, 250);
  buzz(C7, 250);
  buzz(C7, 125);
  buzz(D7, 125);
  buzz(E7, 250);
  buzz(E7, 125);
  buzz(D7, 125);
  buzz(E7, 125);
  buzz(F7, 125);
  buzz(G7, 500);
  buzz(C8, 125);
  buzz(C8, 125);
  buzz(G7, 125);
  buzz(G7, 125);
  buzz(E7, 125);
  buzz(E7, 125);
  buzz(C7, 125);
  buzz(C7, 125);
  buzz(G7, 125);
  buzz(F7, 125);
  buzz(E7, 125);
  buzz(D7, 125);
  buzz(C7, 500);
}

void WDSO8() {
  buzz(G6, 250);
  buzz(A6, 250);
  buzz(G6, 250);
  buzz(F6, 250);
  buzz(E6, 250);
  buzz(F6, 250);
  buzz(G6, 500);
  buzz(D6, 250);
  buzz(E6, 250);
  buzz(F6, 500);
  buzz(E6, 250);
  buzz(F6, 250);
  buzz(G6, 500);
}

void WDSO9() {
  buzz(G6, 125);
  buzz(C7, 125);
  buzz(C7, 125);
  buzz(G6, 125);
  buzz(G6, 125);
  buzz(A6, 125);
  buzz(A6, 125);
  buzz(G6, 125);
  buzz(G6, 125);
  buzz(C7, 125);
  buzz(C7, 125);
  buzz(D7, 125);
  buzz(D7, 125);
  buzz(E7, 250);
  buzz(C7, 250);
  buzz(E7, 250);
  buzz(E7, 250);
  buzz(F7, 125);
  buzz(F7, 125);
  buzz(F7, 250);
  buzz(D7, 250);
  buzz(D7, 250);
  buzz(E7, 125);
  buzz(E7, 125);
  buzz(E7, 250);
  buzz(C7, 250);
  buzz(C7, 250);
  buzz(D7, 125);
  buzz(D7, 125);
  buzz(D7, 125);
  buzz(C7, 125);
  buzz(B6, 125);
  buzz(G6, 125);
  buzz(A6, 125);
  buzz(B6, 125);
  buzz(C7, 500);
}

void WDS10() {
  buzz(C7, 500);
  buzz(C8, 500);
  buzz(B7, 250);
  buzz(G7, 125);
  buzz(A7, 125);
  buzz(B7, 250);
  buzz(C8, 250);
  buzz(C7, 250);
  buzz(C7, 125);
  buzz(C7, 125);
  buzz(A7, 500);
  buzz(G7, 500);
}

void buzz(long frequency, long length) {
  long delayValue = WDPI / frequency / 2; // calculate the delay value between transitions
  //// 1 second's worth of microseconds, divided by the frequency, then split in half since
  //// there are two phases to each cycle
  long numCycles = frequency * length / 1300; // calculate the number of cycles for proper timing
  //// multiply frequency, which is really cycles per second, by the number of seconds to
  //// get the total number of cycles to produce
  for (long i = 0; i < numCycles; i++) { // for the calculated length of time...
    digitalWrite(sn, HIGH); // write the buzzer pin high to push out the diaphram
    delayMicroseconds(delayValue); // wait for the calculated delay value
    digitalWrite(sn, LOW); // write the buzzer pin low to pull back the diaphram
    delayMicroseconds(delayValue - 1); // wait againf or the calculated delay value
  }
  delay(20);
}
