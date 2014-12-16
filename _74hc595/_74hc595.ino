int latchPin = 8;
int clockPin = 12;
int dataPin = 11;

byte changeValue[8];
byte data[6];
int registerCount = 6;

void _595_out()
{ 
  for (int i = 0; i < registerCount; i++) {

    int reverseIndex = registerCount - i - 1;
  
    for (int pinIndex = 0; pinIndex < 8; pinIndex++ ) {
      data[reverseIndex] = changeValue[pinIndex];
      
      digitalWrite(latchPin, LOW);
      
      for (int k = 0; k < registerCount; k++) {
        shiftOut(dataPin, clockPin, LSBFIRST, data[k]);
      }
      
      digitalWrite(latchPin, HIGH);   // 74hc595 출력핀으로 출력 시키기
      
      delay(30);
    }
    
    data[reverseIndex] = 0b00000000;
  }
}

void setup() {
  pinMode(latchPin, OUTPUT);       // 핀들 출력설정
  pinMode(clockPin, OUTPUT);
  pinMode(dataPin, OUTPUT);

  for (int i = 0; i < registerCount; i++) {
      data[i] = 0b00000000;
  }
  
  changeValue[0] = 0b10000000;
  changeValue[1] = 0b01000000;
  changeValue[2] = 0b00100000;
  changeValue[3] = 0b00010000;
  changeValue[4] = 0b00001000;
  changeValue[5] = 0b00000100;
  changeValue[6] = 0b00000010;
  changeValue[7] = 0b00000001;
  
}

void loop() {
  _595_out();
}

