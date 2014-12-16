int latchPin = 1;
int clockPin = 0;
int dataPin = 2;
int micPin = A2;
int mode = 0;

byte changeValue[8];
byte data[6];
int registerCount = 1;

void blinkOn(int duration)
{
  changeValue[0] = 0b11111111;
  for(int i = 1; i <= 7; i ++) {
    changeValue[i] = changeValue[i - 1]; 
  }
  
  for (int i = 0; i < registerCount; i++) {

    int reverseIndex = registerCount - i - 1;
         
    for (int pinIndex = 0; pinIndex < 8; pinIndex++ ) {
      data[reverseIndex] = changeValue[pinIndex];
      
      digitalWrite(latchPin, LOW);
      
      for (int k = 0; k < registerCount; k++) {
        shiftOut(dataPin, clockPin, LSBFIRST, data[k]);
      }
      
      digitalWrite(latchPin, HIGH);   // 74hc595 출력핀으로 출력 시키기
    }
    
    data[reverseIndex] = 0b00000000;
  }
  
    delay(duration);
}

void turnOff(int duration)
{ 
  for (int i = 0; i < registerCount; i++) {

    int reverseIndex = registerCount - i - 1;
         
    for (int pinIndex = 0; pinIndex < 8; pinIndex++ ) {
      data[reverseIndex] = changeValue[pinIndex];
      
      digitalWrite(latchPin, LOW);
      
      for (int k = 0; k < registerCount; k++) {
        shiftOut(dataPin, clockPin, LSBFIRST, 0b00000000);
      }
      
      digitalWrite(latchPin, HIGH);   // 74hc595 출력핀으로 출력 시키기
    }
    
    data[reverseIndex] = 0b00000000;
  }
  
  delay(duration);  
}

void rolling(int duration)
{ 
  changeValue[0] = 0b10000000;
  for(int i = 1; i <= 7; i ++) {
    changeValue[i] = changeValue[i - 1] >> 1; 
  }
  
  for (int i = 0; i < registerCount; i++) {

    int reverseIndex = registerCount - i - 1;
         
    for (int pinIndex = 0; pinIndex < 8; pinIndex++ ) {
      data[reverseIndex] = changeValue[pinIndex];
      
      digitalWrite(latchPin, LOW);
      
      for (int k = 0; k < registerCount; k++) {
        shiftOut(dataPin, clockPin, LSBFIRST, data[k]);
      }
      
      digitalWrite(latchPin, HIGH);   // 74hc595 출력핀으로 출력 시키기
      
      delay(duration);
    }
    
    data[reverseIndex] = 0b00000000;
  }
  
    delay(duration);
}

void setup() {
  pinMode(latchPin, OUTPUT);       // 핀들 출력설정
  pinMode(clockPin, OUTPUT);
  pinMode(dataPin, OUTPUT);
  pinMode(micPin, INPUT);
  
  for (int i = 0; i < registerCount; i++) {
      data[i] = 0b00000000;
  }
}

void play()
{
  if (mode % 3 == 0) {
    for(int i=0;i<20;i++) {    
      blinkOn(100);
      turnOff(100);
    }
  } else if (mode %3 == 1) {
    for(int i=0;i<10;i++) {    
      blinkOn(500);
      turnOff(500);    
    }
  } else {
    for(int i=0;i<20;i++) {    
      rolling(30);
    }
    
    turnOff(30);    
  }
}

void loop() {
  int soundValue = analogRead(micPin);
  if (soundValue > 150) {    
      play();
      mode++;
  }
}

