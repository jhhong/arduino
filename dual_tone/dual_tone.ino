#include <Tone.h>

int notes[] = 
{
  NOTE_A3,
  NOTE_B3,
  NOTE_C4,
  NOTE_D4,
  NOTE_E4,
  NOTE_F4,
  NOTE_G4,  
};

Tone notePlayer[2];

void setup()
{
  Serial.begin(9600);
  notePlayer[0].begin(11);
  notePlayer[1].begin(12);  
  
}

void loop()
{
  
     notePlayer[0].play(notes[2]);
     notePlayer[1].play(notes[6]);     
     notePlayer[0].stop();
     notePlayer[1].stop();
     delay(1000);     
     notePlayer[0].play(notes[2]);
     notePlayer[1].play(notes[6]);     
     notePlayer[0].stop();
     notePlayer[1].stop();
     delay(1000);
     
     
// char c;
// 
// if(Serial.available())
// {
//   c = Serial.read();
//   
//   switch(c)
//   {
//     case 'a'...'g':
//     Serial.println("channel1");     
//     notePlayer[0].play(notes[c - 'a']);
//     Serial.println(notes[c - 'a']);
//     break;
//     
//     case 's':
//     notePlayer[0].stop();
//     break;
//     
//     case 'A'...'G':
//     Serial.println("channel2");     
//     notePlayer[1].play(notes[c - 'A']);
//     Serial.println(notes[c - 'A']);     
//     break;
//     
//     case 'S':
//     notePlayer[1].stop();
//     break;
//     
//     default:
//     Serial.println("default");
//     notePlayer[1].stop();
//     notePlayer[0].play(NOTE_B4);
//     delay(300);
//     notePlayer[0].stop();     
//     delay(100);
//     notePlayer[1].play(NOTE_B4);
//     delay(300);     
//     notePlayer[1].stop();    
//     break;
     
   }
