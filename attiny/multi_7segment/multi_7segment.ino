#include <avr/sleep.h>#include <avr/sleep.h>

const int dataPin = PB0;   // 74HC595의 data(DS) 핀을 연결
const int latchPin = PB1;  // 74HC595의 latch(ST_CP) 핀을 연결
const int clockPin = PB2;  // 74HC595의 clock(SH_CP) 핀을 연결
const int switchPin = PB3; // switch

int counter = 0;

byte dec_digits[] = {
  0b11001111,  //1
  0b10010010,  //2
  0b10000110,  //3
  0b11001100,  //4
  0b10100100,  //5
  0b10100000,  //6
  0b10001101,  //7
  0b11111111,  //8
  0b11110011   //9
};

void setup() {
  //set pins to output so you can control the shift register
  pinMode(latchPin, OUTPUT);
  pinMode(clockPin, OUTPUT);
  pinMode(dataPin, OUTPUT);
  pinMode(switchPin, INPUT_PULLUP);

  //need ?
  GIMSK = 0b00100000;
  PCMSK = 0b00001000;  
}

void loop() {

  //  int count = sizeof(dec_digits) / sizeof(dec_digits[0]);

  sleep();
  waitInput();
}

void waitInput() {
  for (int i = 0; i <= 30000; i++) {
    if (digitalRead(switchPin) == 1) {
      delayMicroseconds(300);
    } else {
      showNumber();
    }
  }
}

void showNumber() {

  for (int i = 0; i < 9; i++) {
    digitalWrite(latchPin, LOW); // shift out the bits  :
    shiftOut(dataPin, clockPin, MSBFIRST, 0b11000000); //take the latch pin high so the LEDs will light up:
    digitalWrite(latchPin, HIGH); // pause before next value:
    delay(500);
  }  
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
