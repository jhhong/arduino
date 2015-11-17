#include <Tone.h>

const int buttonPins[] = {3,4,5,6,7,8,9,10};
int buttonState[8];
const int ledPin =  13;
const int speakerPin = 11;
const int buttonCount = sizeof(buttonPins)/sizeof(buttonPins[0]);
int tones[] = { 1915, 1700, 1519, 1432, 1275, 1136, 1014, 956};
  
void setup() {
  pinMode(ledPin, OUTPUT);  
  pinMode(speakerPin, OUTPUT);
//  notePlayer[0].begin(11);  
  for(int i=0;i<buttonCount;i++){
    pinMode(buttonPins[i], INPUT);
  }

  Serial.begin(9600);
}

void loop() {
  for(int i=0;i<buttonCount;i++){
    buttonState[i] = digitalRead(buttonPins[i]);
    
    if (buttonState[i] == HIGH) {
       play(tones[i]);         
      digitalWrite(ledPin, HIGH);       
    } else {
      digitalWrite(ledPin, LOW);      
    }      
  }  
}

void play(int tone)
{
  int duration =1;
  
  for (long i = 0; i < duration * 1000L; i += tone * 2) {  
    digitalWrite(speakerPin, HIGH);
    delayMicroseconds(tone);
    digitalWrite(speakerPin, LOW);
    delayMicroseconds(tone);  
  }
}  
