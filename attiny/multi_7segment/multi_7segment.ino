int dataPin = PB0;   // 74HC595의 data(DS) 핀을 연결
int latchPin = PB1;  // 74HC595의 latch(ST_CP) 핀을 연결
int clockPin = PB2;  // 74HC595의 clock(SH_CP) 핀을 연결

byte dec_digits[] = {
  0b10000000,
  0b10000000,
  0b11000000,
  0b10100000,
  0b10010000,
  0b10001000,
  0b10000100,
  0b10000010,
  0b10000001
};

void setup() {
  //set pins to output so you can control the shift register
  pinMode(latchPin, OUTPUT);
  pinMode(clockPin, OUTPUT);
  pinMode(dataPin, OUTPUT);
}

void loop() {

  int count = sizeof(dec_digits)/sizeof(dec_digits[0]);
  
  for (int i = 0; i < 9; i++) {
    digitalWrite(latchPin, LOW); // shift out the bits  :
    shiftOut(dataPin, clockPin, MSBFIRST, i); //take the latch pin high so the LEDs will light up:
    digitalWrite(latchPin, HIGH); // pause before next value:
    delay(500);
  }
}
