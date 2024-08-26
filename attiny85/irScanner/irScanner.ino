#define PIN_PB0 PB0
#define PIN_PB1 PB1
#define PIN_PB2 PB2
#define PIN_PB3 PB3
#define PIN_PB4 PB4
#define PIN_PB5 PB5
#define IR_RECV_PIN PB1

#define DECODE_LG
#define DECODE_SAMSUNG
#define DECODE_NEC          // Includes Apple and Onkyo
#define DECODE_RC5
#define DECODE_PANASONIC    // alias for DECODE_KASEIKYO
#define DECODE_RC6
#define DECODE_FAST

#include <TinyWireM.h>
#include <Tiny4kOLED.h>
#include <Arduino.h>
// #include "PinDefinitionsAndMore.h"
#include "ATtinySerialOut.hpp"
#include <IRremote.hpp> // include the library

void setup() {

  IrReceiver.begin(IR_RECV_PIN, ENABLE_LED_FEEDBACK);

  oled.begin();
  oled.clear();
  oled.on();
}

void loop() {
  if (IrReceiver.decode()) {
    if (IrReceiver.decodedIRData.protocol == UNKNOWN) {
        IrReceiver.resume();
    } else {
        printIRInfomation();
        IrReceiver.resume();
    }
  }
}

void printText(int pos, char *text) {
  oled.setFont(FONT6X8P);
  oled.setCursor(0, pos);
  oled.print(text);
}

const char* getProtocolName(decode_type_t protocol) {
  switch (protocol) {
    case NEC: return "NEC";
    case SONY: return "SONY";
    case RC5: return "RC5";
    case RC6: return "RC6";
    case LG: return "LG";
    default: return "UNKNOWN";
  }
}

void printIRInfomation(){
  oled.clear();
  
  printText(0, "Protocol: " + getProtocolName(IrReceiver.decodedIRData.protocol));
  printText(1, "Address: " + (IrReceiver.decodedIRData.address, HEX));
  printText(2, "Command : " + (IrReceiver.decodedIRData.command, HEX));
  printText(3, "---");
}