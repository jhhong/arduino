#define PIN_PB0 PB0 //SDA
#define PIN_PB1 PB1 //IR_RECV_PIN
#define PIN_PB2 PB2 //SCL
#define PIN_PB3 PB3 //SWITCH_PIN
#define PIN_PB4 PB4 //IR_SEND_PIN. Only PB4 working
#define PIN_PB5 PB5 //RESET

#define IR_RECV_PIN PB1
#define IR_SEND_PIN PB4
#define SWITCH_PIN PB3

#define DECODE_LG
#define DECODE_SAMSUNG
#define DECODE_NEC          // Includes Apple and Onkyo
#define DECODE_RC5
#define DECODE_RC6
// #define DECODE_PANASONIC    // alias for DECODE_KASEIKYO
// #define DECODE_FAST

#include <TinyWireM.h>
#include <Tiny4kOLED.h>
#include <Arduino.h>
// #include "PinDefinitionsAndMore.h"
#include "ATtinySerialOut.hpp"
#include <IRremote.hpp> // include the library

char protocol[10];
char address[20];
char command[20];
int i = 0;
bool pushed = false;

void setup() {

  pinMode(SWITCH_PIN, INPUT_PULLUP);
  IrReceiver.begin(IR_RECV_PIN, ENABLE_LED_FEEDBACK);
  IrSender.begin();

  oled.begin();
  oled.enableChargePump(); // The default is off, but most boards need this.

  oled.clear();
  oled.setFont(FONT6X8P);
  oled.on();

  oled.setCursor(0, 0);
  oled.print(F("READY..."));
}

void loop() {
  if (IrReceiver.decode()) {
    if (IrReceiver.decodedIRData.protocol == UNKNOWN) {
        IrReceiver.resume();
    } else {
        printIRInformation(IrReceiver.decodedIRData);
        IrReceiver.resume();
    }
  }

  if(digitalRead(SWITCH_PIN) == LOW && !pushed) {
    pushed = true;

    if (i % 3 == 0) {
      IrSender.sendNEC(0x4, 0x8, 1);
      printText(3, "sendNEC 0x4 0x8");
    } else if (i % 3 == 1) {
      IrSender.sendSamsung(0x7, 0x2, 1); 
      printText(3, "sendSamsung");      
    } else if (i % 3 == 2) {
      IrSender.sendNEC(0x1539, 0x0, 1);       
      printText(3, "sendNEC 0x1539 0x0");
    }

    i++;
    pushed = false;
  }
}

void printText(int pos, char *text) {
  oled.setCursor(0, pos);
  oled.print(text);
}

// const char* getProtocolName(decode_type_t protocol) {
//   switch (protocol) {
//     case NEC: return "NEC";
//     case SONY: return "SONY";
//     case RC5: return "RC5";
//     case RC6: return "RC6";
//     case LG: return "LG";
//     default: return "UNKNOWN";
//   }
// }

void printIRInformation(IRData data) {
    char buffer[9];  // 16진수로 변환된 값을 저장할 버퍼 (최대 8자리 + null terminator)

    oled.clear();

    // 프로토콜 번호를 10진수로 출력 (필요에 따라 16진수로 변환 가능)
    oled.setCursor(0, 0);
    oled.print("Protocol: ");
    itoa(data.protocol, buffer, 10);  // 10진수로 변환
    oled.print(buffer);

    // 주소를 16진수로 변환하여 출력
    oled.setCursor(0, 1);
    oled.print("Address: 0x");
    itoa(data.address, buffer, 16);  // 16진수로 변환
    oled.print(buffer);

    // 명령어를 16진수로 변환하여 출력
    oled.setCursor(0, 2);
    oled.print("Command: 0x");
    itoa(data.command, buffer, 16);  // 16진수로 변환
    oled.print(buffer);

    // 시간 정보 출력
    oled.setCursor(0, 3);
    oled.print("Millis: ");
    ltoa(millis(), buffer, 10);  // 10진수로 변환 (millis는 unsigned long이므로 ltoa 사용)
    oled.print(buffer);
}
