#define MAX_KEYS 10

#include <avr/sleep.h>

int currentSize = 0;

struct KeyFrequencyMap {
  char key;
  float frequency;
};

KeyFrequencyMap keyFrequencyMap[MAX_KEYS];

void addKeyValue(char key, int value) {
  if (currentSize < MAX_KEYS) {
    keyFrequencyMap[currentSize].key = key;
    keyFrequencyMap[currentSize].frequency = value;
    currentSize++;
  }
}

int sn = PB0;
int sw = PB3;
int t = 1;
long WDPI = 1500000;

void setup() {
  pinMode(sw, INPUT_PULLUP);
  pinMode(sn, OUTPUT); // set a pin for buzzer output

  GIMSK = 0b00100000;
  PCMSK = 0b00001000;
}

//C, 4
float getFrequency(char key, int octave){
  float frequency = 0;

  for(int i = 0; i < MAX_KEYS; i++) {
    if(keyFrequencyMap[i].key == key){
      frequency =  keyFrequencyMap[i].frequency;
      break;
    }
  }

  float octaveFrequency = frequency * pow(2, octave - 1);

  return octaveFrequency;
}

void loop() {
  sleep();
  waitInput();
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
    if (digitalRead(sw) == 1) {
      delayMicroseconds(300);
    } else {
      playSound();
    }
  }
}

void playSound() {

  if (t % 10 == 1) {
    HAPPY_BIRTHDAY_TO_YOU();
  }   else if (t % 10 == 2) {
    HAPPY_BIRTHDAY_TO_YOU();
  }   else if (t % 10 == 3) {
    HAPPY_BIRTHDAY_TO_YOU();
  }   else if (t % 10 == 4) {
    HAPPY_BIRTHDAY_TO_YOU();
  }   else if (t % 10 == 5) {
    HAPPY_BIRTHDAY_TO_YOU();
  }   else if (t % 10 == 6) {
    HAPPY_BIRTHDAY_TO_YOU();
  }   else if (t % 10 == 7) {
    HAPPY_BIRTHDAY_TO_YOU();
  }   else if (t % 10 == 8) {
    HAPPY_BIRTHDAY_TO_YOU();
  }   else if (t % 10 == 9) {
    HAPPY_BIRTHDAY_TO_YOU();
  }   else if (t % 10 == 0) {
    HAPPY_BIRTHDAY_TO_YOU();
  }
  delay(200);
  t++;
}

//happy birthday to u
void HAPPY_BIRTHDAY_TO_YOU() {
      buzz('G',6,125);
      buzz('G',6,125);
      buzz('A',6,250);
      buzz('G',6,250);
      buzz('C',7,250);
      buzz('B',6,500);
      buzz('G',6,125);
      buzz('G',6,125);
      buzz('A',6,250);
      buzz('G',6,250);
      buzz('D',7,250);
      buzz('C',7,500);
      buzz('G',6,125);
      buzz('G',6,125);
      buzz('G',7,250);
      buzz('E',7,250);
      buzz('C',7,250);
      buzz('B',6,250);
      buzz('A',6,250);
      buzz('F',7,125);
      buzz('F',7,125);
      buzz('E',7,250);
      buzz('C',7,250);
      buzz('D',7,250);
      buzz('C',7,500);
}

void buzz(char key, int octave , long length) {
  float frequency = getFrequency(key, octave);

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
