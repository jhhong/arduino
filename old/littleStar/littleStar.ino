/*
  Melody
 
 Plays a melody 
 
 circuit:
 * 8-ohm speaker on digital pin 8
 
 created 21 Jan 2010
 modified 30 Aug 2011
 by Tom Igoe 

This example code is in the public domain.
 
 http://arduino.cc/en/Tutorial/Tone
 
 */
//#include "pitches.h"
#include <Tone.h>

// notes in the melody:
int melody[] = {
//  NOTE_C4, NOTE_G3,NOTE_G3, NOTE_A3, NOTE_G3,0, NOTE_B3, NOTE_C4
//  NOTE_C4, NOTE_C4,NOTE_G4, NOTE_G4, NOTE_A4,NOTE_A4, NOTE_G4, NOTE_F4, NOTE_F4, NOTE_E4, NOTE_E4, NOTE_D4, NOTE_D4, NOTE_C4
NOTE_C4,NOTE_C4,NOTE_G4,NOTE_G4,NOTE_A4,NOTE_A4,NOTE_G4,NOTE_F4,NOTE_F4,NOTE_E4,NOTE_E4,NOTE_D4,NOTE_D4
};

int chord[] = {NOTE_G4, NOTE_G4,NOTE_C4,NOTE_C4,NOTE_F4,NOTE_F4,NOTE_C4,NOTE_C4,NOTE_C4,NOTE_G4,NOTE_G4,NOTE_D4,NOTE_D4,NOTE_C3};

Tone channel[3];

// note durations: 4 = quarter note, 8 = eighth note, etc.:
int noteDurations[] = {
  2};

void setup() {
  // iterate over the notes of the melody:
  
  channel[0].begin(2);
  channel[1].begin(3);  
  channel[2].begin(4);  
  
  int count = sizeof(noteDurations)/sizeof(noteDurations[0]);
  for (int thisNote = 0; thisNote < count; thisNote++) {

    // to calculate the note duration, take one second 
    // divided by the note type.
    //e.g. quarter note = 1000 / 4, eighth note = 1000/8, etc.
    int noteDuration = 1000/noteDurations[0];
//    tone(8, melody[thisNote],noteDuration);
//    tone(9, melody[2],noteDuration);
    channel[0].play(NOTE_C4);
    channel[1].play(NOTE_E4);
    channel[2].play(NOTE_G4);
    
    // to distinguish the notes, set a minimum time between them.
    // the note's duration + 30% seems to work well:
    int pauseBetweenNotes = noteDuration * 1.30;
    delay(pauseBetweenNotes);
    // stop the tone playing:
//    noTone(8);
//    noTone(9);    

    channel[0].stop();
    channel[1].stop();
    channel[2].stop();    
  }
}

void loop() {
  // no need to repeat the melody.
}
