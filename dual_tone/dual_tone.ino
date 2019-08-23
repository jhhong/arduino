#include <Tone.h>

int buttonPin1 = 3;
int buttonPin2 = 4;
int buttonPin3 = 5;

int tonePins[] = {11,12,2};
Tone tones[3];

void setup()
{
  pinMode(buttonPin1,INPUT);
  pinMode(buttonPin2,INPUT);  
  pinMode(buttonPin3,INPUT);  
  
  for (int i=0;i<3;i++) {
    tones[i].begin(tonePins[i]);
  }
}

void loop()
{
  int state1 = digitalRead(buttonPin1);
  int state2 = digitalRead(buttonPin2);  
  int state3 = digitalRead(buttonPin3);  
  
  // assign by first visit.
  int toneIndex = 0;
  
  if (state1 == HIGH) {
    tone[toneIndex].play(NOTE_C4);
  } else {
    tone1.stop();    
  }

  if (state2 == HIGH) {
    tone1.play(NOTE_E4);      
  } else {
    tone1.stop();
  }

  if (state3 == HIGH) {
    tone3.play(NOTE_G4);      
  } else {
    tone3.stop();
  }
 }
