
#include <Wire.h>
#include <EEPROM.h>
#include <Time.h>
#include <DS1307RTC.h>

#define TACT_PIN 14
#define ALARM_PIN 15

// 7 segment
const byte expressableChars[12] = {

  B11111100, //0
  B01100000, //1
  B11011010, //2
  B11110010, //3
  B01100110, //4
  B10110110, //5
  B00111110, //6
  B11100000, //7
  B11111110, //8
  B11100110, //9
  B10011100, //C
  B01101111, //H
};

//dp, G,F,E,D,C,B,A (reverse)
const int segmentPins[8] = {4,7,8,6,5,3,2,9};
const int digitalPins[4] = {10,11,12,13};

//Debouncing
long lastDebounceTime = 0;
int switchState;
int lastSwitchState = LOW;
long debounceDelay = 50;

boolean canPlaying = true;

//Alarm
char* alarm[] = {"0730","0800"};

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

  pinMode(TACT_PIN , INPUT);
  pinMode(ALARM_PIN , OUTPUT);  
        
  Serial.begin(9600);
}

void loop()
{
  showTime();  
  checkSwitch();
}

void checkSwitch()
{
  int switchValue = digitalRead(TACT_PIN);  
  if (switchValue != lastSwitchState) 
  {
    lastDebounceTime = millis();
  } 

  if ((millis() - lastDebounceTime) > debounceDelay) 
  {
    if (switchValue != switchState) 
    {
      switchState = switchValue;
      if (switchState == HIGH) 
      {
        canPlaying = !canPlaying;
      }
    }
  }
  
  lastSwitchState = switchValue;
}

//show 7 segment
void showDigit(int digit,int number)
{
  digitalWrite(digitalPins[digit] , HIGH);
  
  boolean isBitSet;
  for(int segment=1;segment<8;segment++)
  {
    isBitSet = bitRead(expressableChars[number],segment);
    isBitSet = !isBitSet;
    digitalWrite(segmentPins[segment], isBitSet);
  }  
  delay(5);  
  digitalWrite(digitalPins[digit] , LOW);     
}

void showTime()
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
  
  int hour24 = tm.Hour;
  int h1 = hour24 / 10;
  int h2 = hour24 % 10;
  
  int minute60 = tm.Minute;
  int m1 = minute60 / 10;
  int m2 = minute60 % 10;

  int sec = tm.Second;
  
  showDigit(0, h1);    
  showDigit(1, h2);
  showDigit(2, m1);
  showDigit(3, m2);  
  
  alarmCheck(h1,h2,m1,m2,sec);  
}

void alarmCheck(int h1,int h2,int m1,int m2,int sec)
{
  int checkCount = 0;
        
  for (int i=0,icnt=sizeof(alarm)/sizeof(alarm[0]);i<icnt;i++)
  {
    char *time = alarm[i];
    int t1 = int(time[0]) - 48;
    int t2 = int(time[1]) - 48;
    int t3 = int(time[2]) - 48;
    int t4 = int(time[3]) - 48;
    
    if (h1 ==  t1 && h2 == t2 && m1 == t3 && m2 == t4)
    { 
      checkCount++;
    }
  }

  if(checkCount > 0)
  {
     if (canPlaying )
     {
        ringAlarm();       
     }
     else
     {
       ringOffAlarm();
     }
  }
  else
  {
    canPlaying = true;
    ringOffAlarm();
  }
}

void ringAlarm() 
{
    digitalWrite(ALARM_PIN , HIGH);
} 

void ringOffAlarm() 
{
    digitalWrite(ALARM_PIN , LOW);
}
