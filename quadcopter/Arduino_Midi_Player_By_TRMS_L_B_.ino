#include <Tone.h>
String inString = "";  
int note_arr[] = {31,33,35,37,39,41,44,46,49,52,55,58,62,65,69,73,78,82,87,93,98,104,110,117,123,131,139,147,156,165,175,185,196,208,220,233,247,262,277,294,311,330,349,370,392,415,440,466,494,523,554,587,622,659,698,740,784,831,880,932,988,1047,1109,1175,1245,1319,1397,1480,1568,1661,1760,1865,1976,2093,2217,2349,2489,2637,2794,2960,3136,3322,3520,3729,3951,4186,4435,4699,4978};
Tone tone1;
Tone tone2;
Tone tone3;
void setup() {
  Serial.begin(115200);
  while (!Serial) {
  }
  Serial.println("\n\nString toInt():");
  Serial.println();
  tone1.begin(2);
  tone2.begin(3);
  tone3.begin(4);
}
int pitch = 0;
int channel = 0;
int ison = 0;
int index = 0;
void loop() {
  if (Serial.available() > 0) {
      inString = Serial.readStringUntil('\n');
      char str[50];
      inString.toCharArray(str, 50);
      const char * pch;
      pch = strtok(str, "-");
      while(pch != NULL) {
         switch (index) {
           case 0:
           pitch = ((String)pch).toInt();
           break;
           case 1:
           ison = ((String)pch).toInt();
           break;
           case 2:
           channel = ((String)pch).toInt();
           break;
         }
         pch = strtok(NULL, "-");
         index++;
      }
      index = 0;
      if (ison == 0) {
           switch (channel) {
            case 0:
               tone1.stop();
               break;
            case 1:
               tone2.stop();
               break;   
            case 2:
               tone3.stop();
               break;
          }
      } else {
          switch (channel) {
            case 0:
               tone1.play(note_arr[pitch-23]);
               break;
            case 1:
               tone2.play(note_arr[pitch-23]);
               break;   
            case 2:
               tone3.play(note_arr[pitch-23]);
               break;
          }
    }
  }
}

 
