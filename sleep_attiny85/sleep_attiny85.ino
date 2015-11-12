#include <avr/sleep.h>

int pirPin = 2;
int ledPin1 = 3;
int ledPin2 = 4;
int ledPin3 = 0;

bool needLedOn = false;

void setup()
{
  pinMode(pirPin, INPUT);          //inputPin을 입력모드로  
  pinMode(ledPin1, OUTPUT);    //ledPin을 출력모드로
  pinMode(ledPin2, OUTPUT);    //ledPin을 출력모드로
  pinMode(ledPin3, OUTPUT);    //ledPin을 출력모드로  
}

void loop()
{
  if (needLedOn) {
    ledOn();    
    delay(10000);    
    needLedOn = false;
  } else {
    ledOff();    
    sleepNow();
  }  
}

void sleepNow()
{
  set_sleep_mode(SLEEP_MODE_PWR_DOWN); // 슬립모드를 파워 다운모드로 설정합니다.
  sleep_enable(); // 슬립모드를 활성화시킵니다. (안전핀 같은 기능 실제로 슬립모드에 진입하지는 않음)
  
  GIMSK = 0b00100000; // 핀체인지 인터럽트를 사용합니다.
  PCMSK = 0b00000010; // 핀체인지 인터럽트 핀을 2번으로 할당합니다.
  
  sleep_mode(); // 슬립모드에 진입합니다. (실제 슬립모드에 진입)
  
  sleep_disable(); // 슬립모드를 비활성화 시킵니다. ( 슬립모드에 진입하면 비활성화 시키지 못하는데, 인터럽트 신호가 발생하면 다시 슬립모드에서 나와 슬립모드를 비활성화 시킵니다.)
  
  GIMSK = 0b00000000; // 핀체인지 인터럽트를 사용하지않습니다.
  PCMSK = 0b00000000;
}

void ledOn(){
  digitalWrite(ledPin1,HIGH);
  digitalWrite(ledPin2,HIGH);
  digitalWrite(ledPin3,HIGH);  
}

void ledOff(){
  digitalWrite(ledPin1,LOW);
  digitalWrite(ledPin2,LOW);
  digitalWrite(ledPin3,LOW);
}

ISR(PCINT0_vect)
{
    needLedOn = true;  
}
