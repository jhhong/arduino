#include <avr/sleep.h>

const int dataPin = PB0;   // 74HC595의 data(DS) 핀을 연결
const int latchPin = PB1;  // 74HC595의 latch(ST_CP) 핀을 연결
const int clockPin = PB2;  // 74HC595의 clock(SH_CP) 핀을 연결
const int switchPin = PB3; // switch
const int tensPlacePin = PB4; // 10의 자리. position. 19 -> 1. 35 ->3

int counter = 1;
bool pushBlock = false;
bool sleepMode = false;
unsigned long pushBlockTime = 0; //push 버튼 여러번 눌리지 않도록 체크
int displayTimeout = 30000; //30 sec
int pushBlockTimeout = 300; //0.3 sec

//1의 자리
byte digits_one[] = {
  0b10000001,  //0
  0b11001111,  //1
  0b10010010,  //2
  0b10000110,  //3
  0b11001100,  //4
  0b10100100,  //5
  0b10100000,  //6
  0b10001101,  //7
  0b10000000,  //8
  0b10001100,  //9
};

//10의 자리
byte digits_ten[] = {
  0b00000001,  //0
  0b01001111,  //1
  0b00010010,  //2
  0b00000110,  //3
  0b01001100,  //4
  0b00100100,  //5
  0b00100000,  //6
  0b00001101,  //7
  0b00000000,  //8
  0b00001100,  //9
};

void setup() {
  //set pins to output so you can control the shift register
  pinMode(latchPin, OUTPUT);
  pinMode(clockPin, OUTPUT);
  pinMode(dataPin, OUTPUT);
  pinMode(switchPin, INPUT_PULLUP);
  pinMode(tensPlacePin, OUTPUT);

  //interrupt setting
  GIMSK = 0b00100000;
  PCMSK = 0b00001000;
}

void loop() {
  waitInput();

  if (!sleepMode) {
    showNumber();
  }

  if (canExecutable(pushBlockTime, pushBlockTimeout)) {
    pushBlock = false;
  }

  if (canExecutable(pushBlockTime, displayTimeout)) {
    displayOff();
    sleepMode = true;
    sleep();
  }
}

bool canExecutable(unsigned long startTime, int limit) {
  unsigned long diff = millis() - startTime;
  return diff > limit ? true : false;
}

void waitInput() {
  if (digitalRead(switchPin) == 0 && !pushBlock) {
    pushBlock = true;
    pushBlockTime = millis();
    counter++;
  }
}

void showNumber() {

  if (counter >= 100) {
    counter = 1;
  }

  int tensPlace = counter / 10;  // 1~9
  int unitsPlace = counter % 10; // 0~9

  showNumberOfTen(tensPlace);
  showNumberOfOne(unitsPlace);
}

//1의 자리
void showNumberOfOne(int n) {
  digitalWrite(tensPlacePin, LOW);
  digitalWrite(latchPin, LOW); // shift out the bits  :
  shiftOut(dataPin, clockPin, LSBFIRST, digits_one[n]); //take the latch pin high so the LEDs will light up:
  digitalWrite(latchPin, HIGH); // pause before next value:
  delay(10);
}

//10의 자리
void showNumberOfTen(int n) {

  if ( n > 0) {
    digitalWrite(tensPlacePin, HIGH);
    digitalWrite(latchPin, LOW); // shift out the bits  :
    shiftOut(dataPin, clockPin, LSBFIRST, digits_ten[n]); //take the latch pin high so the LEDs will light up:
    digitalWrite(latchPin, HIGH); // pause before next value:
    delay(10);
  }
}

void displayOff() {

  digitalWrite(latchPin, LOW); // shift out the bits  :
  shiftOut(dataPin, clockPin, LSBFIRST, 0x11111111); //take the latch pin high so the LEDs will light up:
  digitalWrite(latchPin, HIGH); // pause before next value:

  digitalWrite(tensPlacePin, LOW);
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
  sleepMode = false;
}
