
#include <Wire.h>
#include <Time.h>
#include <DS1307RTC.h>

const byte numeral[10] = {

  B11111100, //0
  B01100000,
  B11011010,
  B11110010,
  B01100110,
  B10110110,
  B00111110,
  B11100000,
  B11111110, //8
  B11100110 //9
};

//dp, G,F,E,D,C,B,A
const int segmentPins[8] = {4,7,8,6,5,3,2,9};
const int digitalPins[4] = {10,11,12,13};

void setup()
{
  for(int i=0;i<8;i++)
  {
    pinMode(segmentPins[i] , OUTPUT);
  }

  for(int i=0;i<4;i++)
  {
    pinMode(digitalPins[i] , OUTPUT);
  }
     
//hour , min , sec , 
//  setTime(2,5,1,4,10,14);
  
  Serial.begin(9600);
}

void loop()
{
  
  tmElements_t tm;

  if (RTC.read(tm)) 
  {
  } 
  else 
  {
    if (RTC.chipPresent()) 
    {
      Serial.println("The DS1307 is stopped.  Please run the SetTime");
      Serial.println("example to initialize the time and begin running.");
      Serial.println();
    }
    else 
    {
      Serial.println("DS1307 read error!  Please check the circuitry.");
      Serial.println();
    }
    delay(1000);
  }

  
  int h = tm.Hour;
  int h1 = h/10;
  int h2 = h%10;
  
  int s = tm.Minute;
  int s1 = s/10;
  int s2 = s%10;
  
  showDigit(0, h1);    
  showDigit(1, h2);
  showDigit(2, s1);
  showDigit(3, s2);

//Serial.print(second());
//delay(1000);
}


void showDigit(int digit,int number)
{
  digitalWrite(digitalPins[digit] , HIGH);
  
  boolean isBitSet;
  for(int segment=1;segment<8;segment++)
  {
    isBitSet = bitRead(numeral[number],segment);
    isBitSet = !isBitSet;
    digitalWrite(segmentPins[segment], isBitSet);
  }  
  delay(5);  
  digitalWrite(digitalPins[digit] , LOW);     
}
