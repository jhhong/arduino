#define IR_SEND_PIN     PB4 //PB0,PB1, PB2, PB3,PB4 전부 전송 가능 확인. 디폴트 설정은 PB4
#define LED_BUILTIN     PB0 //FEEDBACK LED
#define SWITCH_PIN      PB3

#if !defined(ARDUINO_ESP32C3_DEV) // This is due to a bug in RISC-V compiler, which requires unused function sections :-(.
#define DISABLE_CODE_FOR_RECEIVER // Disables static receiver code like receive timer ISR handler and static IRReceiver and irparams data. Saves 450 bytes program memory and 269 bytes RAM if receiving functions are not required.
#endif
//#define SEND_PWM_BY_TIMER         // Disable carrier PWM generation in software and use (restricted) hardware PWM.
//#define USE_NO_SEND_PWM           // Use no carrier PWM, just simulate an active low receiver signal. Overrides SEND_PWM_BY_TIMER definition

bool pushed = false;
unsigned long lastPushedTime = 0; //push 버튼 여러번 눌리지 않도록 체크
int pushInterval = 200; //0.2 sec
int i = 0;

#include <IRremote.hpp>

void setup() {
    pinMode(LED_BUILTIN, OUTPUT);
    pinMode(SWITCH_PIN, INPUT_PULLUP);
    IrSender.begin();
    // disableLEDFeedback(); // Disable feedback LED at default feedback LED pin
}


void loop() {
    if(canPushable() && digitalRead(SWITCH_PIN) == LOW && !pushed){
      pushed = true;
      sendSignal();
      
      lastPushedTime = millis();
      pushed = false;
    }
}

bool canPushable() {
  unsigned long diff = millis() - lastPushedTime;
  return diff > pushInterval ? true : false;
}

void sendSignal() {
    if(i % 3 == 0) {
      IrSender.sendNEC(0x4, 0x8, 1);
    } else if (i % 3 == 1) {
      IrSender.sendSamsung(0x7, 0x2, 1);
    } else if (i % 3 == 2) {
      IrSender.sendNEC(0x1539, 0x0, 1);
    }

    i++;
}
