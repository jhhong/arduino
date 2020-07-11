int dataPin = PB0;   // 74HC595의 data(DS) 핀을 연결
int latchPin = PB1;  // 74HC595의 latch(ST_CP) 핀을 연결
int clockPin = PB2;  // 74HC595의 clock(SH_CP) 핀을 연결

byte dec_digits[] = {
  0b11001111,  //1
  0b10010010,  //2
  0b10000110,  //3
  0b11001100,  //4
  0b10100100,  //5
  0b10100000,  //6
  0b10001101,  //7
  0b11111111   //8
};

void setup() {
  //set pins to output so you can control the shift register
  pinMode(latchPin, OUTPUT);
  pinMode(clockPin, OUTPUT);
  pinMode(dataPin, OUTPUT);
}

void loop() {

  //  int count = sizeof(dec_digits) / sizeof(dec_digits[0]);

  for (int i = 0; i < 7; i++) {
    digitalWrite(latchPin, LOW); // shift out the bits  :
    shiftOut(dataPin, clockPin, MSBFIRST, 0b11000000); //take the latch pin high so the LEDs will light up:
    digitalWrite(latchPin, HIGH); // pause before next value:
    delay(500);
  }
}
