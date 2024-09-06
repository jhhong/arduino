#include <tiny_IRremote.h>
#include <avr/sleep.h>

// IRsend 는 반드시 PB4 에만 연결 해야 정상 동작함
// PB4 는 별도로 pinMode 해줄 필요없음
// CR2032 정도의 전압에 연결시 적외선 LED와 피드백용 LED(PB0) 에 저항을 연결하지 않아도 정상동작
// PC의 USB로 전원 연결시 적외선 LED와 피드백용 LED(PB0) 에 저항(테스트:330옴)을 반드시 연결해야 정상 동작함.
// 적외선 송신 LED는 수신 LED의 방향에 일직선으로 일치해야 수신됨. 30-40cm의 가까운 거리라도 방향 불일치시 수신 안됨.
// 위로 송신하거나 손에 가리면 전송이 안됨(최대 길이 테스트 필요)
// 신호 증폭가능한 방법 확인 필요

int LED_PIN = PB0;
int SWITCH_PIN = PB3;

bool pushBlock = false;
unsigned long pushBlockTime = 0; //push 버튼 여러번 눌리지 않도록 체크
int pushBlockTimeout = 200; //0.3 sec

IRsend irsend;

void setup() {
  pinMode(LED_PIN, OUTPUT);
  pinMode(SWITCH_PIN, INPUT_PULLUP);

  //must need set
  GIMSK = 0b00100000; //외부인터럽트 0b01000000  핀체인지 인터럽트 0b00100000
  PCMSK = 0b00001000; //Pin Change Mask Register. 어떤핀을 사용할지 선택. (폴링엣지에서 ISR호출됨)
}

void loop() {
  sleep();
  waitInput();

  if (canExecutable(pushBlockTime, pushBlockTimeout)) {
    pushBlock = false;
  }
}

bool canExecutable(unsigned long startTime, int limit) {
  unsigned long diff = millis() - startTime;
  return diff > limit ? true : false;
}

void sendSignal() {
  irsend.sendSony(0xa90, 12);
  blink();
}

void blink() {
  digitalWrite(LED_PIN, HIGH);
  delay(200);
  digitalWrite(LED_PIN, LOW);
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
  if (digitalRead(SWITCH_PIN) == 0 && !pushBlock) {
    pushBlock = true;
    pushBlockTime = millis();
    sendSignal();
  }
}

//  void sendNEC(unsigned long data, int nbits);
//  void sendSony(unsigned long data, int nbits);
//  void sendRaw(unsigned int buf[], int len, int hz);
//  void sendRC5(unsigned long data, int nbits);
//  void sendRC6(unsigned long data, int nbits);
