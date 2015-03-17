int latchPin = 8;
int clockPin = 12;
int dataPin = 11;
byte changeValue[8];
byte data[6];
int registerCount = 2;

void setup() {
  pinMode(latchPin, OUTPUT);
  pinMode(clockPin, OUTPUT);
  pinMode(dataPin, OUTPUT);
  
  for (int i = 0; i < registerCount; i++) {
      data[i] = 0b00000000;
  }
  
  changeValue[0] = 0b10000000;
  for(int i = 0; i <= 7; i ++)
  changeValue[i] = changeValue[i - 1] >> 1;
  
  Serial.begin(9600);  
}

void loop() {
   show();    
}

void show()
{
    showContinuous();    
//    showAll();
//    delay(1000);
//    hide();
//    delay(1000);
}

void showContinuous()
{
//  for (int i = 0; i < registerCount; i++) {

//    int reverseIndex = registerCount - i - 1;
//  
//    for (int pinIndex = 0; pinIndex < 8; pinIndex++ ) {
//      data[reverseIndex] = changeValue[pinIndex];
      
      digitalWrite(latchPin, LOW);
      
//      for (int k = 0; k < registerCount; k++) {
        shiftOut(dataPin, clockPin, LSBFIRST, 0b10111111);
        shiftOut(dataPin, clockPin, LSBFIRST, 0b10010000);        
//      }
      
      digitalWrite(latchPin, HIGH);   // 74hc595 출력핀으로 출력 시키기
      
      delay(1000);
      
      digitalWrite(latchPin, LOW);
      
//      for (int k = 0; k < registerCount; k++) {
        shiftOut(dataPin, clockPin, LSBFIRST, 0b0111111);
        shiftOut(dataPin, clockPin, LSBFIRST, 0b01001001);        
//      }
      
      digitalWrite(latchPin, HIGH);   // 74hc595 출력핀으로 출력 시키기
      
      delay(1000);
      
//    }
    
//    data[reverseIndex] = 0b11111111;
//  }
}

void showAll()
{
  digitalWrite(latchPin, LOW);  
  
  for (int i = 0; i < registerCount; i++) {
    shiftOut(dataPin, clockPin, LSBFIRST, 0b11111111);
  }
  
  digitalWrite(latchPin, HIGH);  
}

void hide()
{
  digitalWrite(latchPin, LOW);  
  
  for (int i = 0; i < registerCount; i++) {
    shiftOut(dataPin, clockPin, LSBFIRST, 0b00000000);
  }
  
  digitalWrite(latchPin, HIGH);  
}
