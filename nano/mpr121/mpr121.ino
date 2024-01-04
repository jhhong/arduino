#include <Wire.h>
#include <Adafruit_MPR121.h>

#ifndef _BV
#define _BV(bit) (1 << (bit)) 
#endif

Adafruit_MPR121 cap = Adafruit_MPR121();

uint16_t lasttouched = 0;
uint16_t currtouched = 0;

void setup() {
  Serial.begin(115200);

  pinMode(LED_BUILTIN, OUTPUT);

  Serial.println("Adafruit MPR121 Capacitive Touch sensor test"); 

  // Default address is 0x5A, if tied to 3.3V its 0x5B
  // If tied to SDA its 0x5C and if SCL then 0x5D
  if (!cap.begin(0x5A)) {
    Serial.println("MPR121 not found, check wiring?");
    while (1);
  }

  Serial.println("MPR121 found!");
}

void loop() {

  // digitalWrite(LED_BUILTIN, HIGH);
  // delay(1000);
  // digitalWrite(LED_BUILTIN, LOW);
  // delay(1000);

  currtouched = cap.touched();
  
  for (uint8_t i=0; i<12; i++) {

    if ((currtouched & _BV(i)) && !(lasttouched & _BV(i)) ) {
      Serial.print(i); 
      Serial.println(" touched");

      if(i == 0) {
        digitalWrite(LED_BUILTIN, HIGH);
        delay(50);
        digitalWrite(LED_BUILTIN, LOW);
        delay(50);
      }

      if(i==1) {
        
      }

      if(i==2) {
        
      }

      if(i==3) {
        
      }

      if(i==4) {
        
      }

      if(i==5) {
        
      }

      if(i==6) {
        
      }

      if(i==7) {
        
      }
            
    }

    if (!(currtouched & _BV(i)) && (lasttouched & _BV(i)) ) {
      Serial.print(i); 
      Serial.println(" released");
    }
  }

  lasttouched = currtouched;

  // put a delay so it isn't overwhelming
  delay(100);
}
