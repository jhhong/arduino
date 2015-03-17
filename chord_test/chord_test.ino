/* 
 * Copyright (c) 2010 Drew Crawford
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 * IN THE SOFTWARE.
 */

#include <bassdll.h>
#include <debug.h>

void pt1(channel &drums, channel &bassline, channel& sky);
void pt2(channel &drums, channel &bassline, channel& sky);
  
mixer m;
note** memblk;
channel pin10 (10,1);
channel pin11 (11,1);
void setup(){
memblk = (note**) calloc(290,sizeof(note*));
  if (memblk==NULL) debug(1111);

m.add_channel(&pin10);
m.add_channel(&pin11);
//m.add_channel(&pin12); //LOUD PIN

  
}

void loop() {

  {pt1(pin10,pin11);}
  {pt2(pin11,pin10);}
//
//
//    {pt3(pin12,pin11,pin10);}
}





#define EIGHTH 40
#define FOURTH EIGHTH *2
#define KICK_LEN 9
#define SNARE_LEN 7


//inline void pt1(channel& melody, channel& harmony, channel& skyline)
inline void pt1(channel& melody, channel& harmony)
{

  

note transposed;
transposed.tone = TRANSPOSEDOWN;
transposed.duration = 0;


  note sc;
  sc.tone = 27;
  sc.duration = EIGHTH;
  
  note sg;
  sg.tone = 22;
  sg.duration = EIGHTH;
  
  note se;
  se.tone = 19;
  se.duration = EIGHTH;
  
  note sgsharp;
  sgsharp.tone = 23;
  sgsharp.duration = EIGHTH;
  note sf;
  sf.tone = 20;
  sf.duration = EIGHTH;
  
  note sd;
  sd.tone = 29;
  sd.duration = EIGHTH;
  
  
    note sasharp;
    sasharp.tone = 25;
    sasharp.duration = EIGHTH;
    
    ////////MELODY
note mc;
mc.tone = 3;
mc.duration = FOURTH;

note ma;
ma.tone = 12;
ma.duration = EIGHTH;

Serial.println(EIGHTH);

note mclong;
mclong.tone = 3;
mclong.duration = EIGHTH * 10;

note melong;
melong.tone = 7;
melong.duration = EIGHTH * 10;

note mg;
mg.tone = 10;
mg.duration = FOURTH;

note hg;
hg.tone = 10;
hg.duration = FOURTH;

note hc;
hc.tone = 10;
hc.duration = FOURTH;


note me;
me.tone = 7;
me.duration = FOURTH;

note mf;
mf.tone = 8;
mf.duration = EIGHTH * 4;


note md4;
md4.tone = 5;
md4.duration = EIGHTH * 4;

note me4;
me4.tone = 7;
me4.duration = EIGHTH * 4;

note mc4;
mc4.tone = 3;
mc4.duration = EIGHTH * 4;

note mchold;
mchold.tone = 3;
mchold.duration = EIGHTH * 24;
note mchold2;
mchold2.tone = 3;
mchold2.duration = EIGHTH*10;

note mghold;
mghold.tone = 10;
mghold.duration = EIGHTH * 24;
note mghold2;
mghold2.tone = 10;
mghold2.duration = EIGHTH*10;

note ma4;
ma4.tone = 12;
ma4.duration = EIGHTH * 4;

note mg4;
mg4.tone = 10;
mg4.duration = EIGHTH * 4;

note mgsharphold;
mgsharphold.tone = 11;
mgsharphold.duration = EIGHTH * 18; 

note masharphold;
masharphold.tone = 13;
masharphold.duration = EIGHTH * 16;

note mfhold;
mfhold.tone = 8;
mfhold.duration = EIGHTH * 16; 


//memblk =  (note**) malloc(sizeof(note*) * 265);
  melody.notes = memblk ;
//  harmony.notes = melody.notes + 1 ;  
//  harmony.notes = melody.notes + 50 ; //melody needs 50 notes? round numbers
//  skyline.notes = harmony.notes + 50 ; //harmony needs 50 note? round numbers
                                      //skyline needs 128 notes exact
                                      
     melody.realloc_notes();
  harmony.realloc_notes();
//  skyline.realloc_notes();
for(int i = 0; i < 12; i++)
{
   harmony.queue(&transposed);
}
  for(int k = 0; k < 2; k++)
  {
    for(int i = 0; i < 8; i++)
    {
//      skyline.queue(&sc);
//      skyline.queue(&sg);
//      skyline.queue(&se);
//      skyline.queue(&sg);
    }

  

    for(int i = 0; i < 4; i++)
    {
//      skyline.queue(&sc);
//      skyline.queue(&sgsharp);
//      skyline.queue(&sf);
//      skyline.queue(&sgsharp);
    }
    for(int i = 0; i < 4; i++)
    {
      switch(k){
        case 0:
//        skyline.queue(&sc);
        break;
        case 1:
//        skyline.queue(&sd);
        break;
      }
//      skyline.queue(&sasharp);
//      skyline.queue(&sf);
//      skyline.queue(&sasharp);
    }
  
  }


  melody.queue(&mc); //do
  harmony.queue(&hg);
  melody.queue(&mc); //sol
  harmony.queue(&hg);
  melody.queue(&mg); // do
  harmony.queue(&hg);
  melody.queue(&mg); //mi
  harmony.queue(&hg);
  melody.queue(&ma); //fa
//  harmony.queue(&ma4);
  melody.queue(&ma); //mi
//  harmony.queue(&hg);
  melody.queue(&ma); //re
    melody.queue(&ma); //re
  melody.queue(&mg); //re    
//  harmony.queue(&mf);

note stop;
stop.tone = STOP;
stop.duration = 0;
  melody.queue(&stop);

  m.play();
  
}
