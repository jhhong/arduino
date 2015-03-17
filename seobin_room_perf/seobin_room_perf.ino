
#include <Wire.h>
#include <EEPROM.h>
#include <Time.h>
#include <DS1307RTC.h>
#include <dht.h>

#define DHT11_PIN 4
#define TACT_PIN 14

enum displayMode {Clock, Temparature, Humidity};

int displayMode = Clock;

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

//EEPROM
const int addr = 0;

//Debouncing
long lastDebounceTime = 0;
int switchState;
int lastSwitchState = LOW;
long debounceDelay = 50;

//DHT11
dht DHT;
unsigned long previousTimeStamp = 0;
unsigned long currentTimeStamp;

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
  
  Serial.begin(9600);
}

void loop()
{
  displayMode = EEPROM.read(addr);
  
  if (displayMode == Temparature || displayMode == Humidity)
  {
    showWeatherInfo();
  }
  else
  {
    showTime();
  }
  
  changeDisplayMode();
}

void changeDisplayMode()
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
        int nextDisplayMode = EEPROM.read(addr);
        nextDisplayMode++;
        if (nextDisplayMode > Humidity)
        {
          nextDisplayMode = Clock;
        }
        
        EEPROM.write(addr, nextDisplayMode);      
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
  
  showDigit(0, h1);    
  showDigit(1, h2);
  showDigit(2, m1);
  showDigit(3, m2);  
}

//Temparature , Humidity
void showWeatherInfo()
{
  currentTimeStamp = millis();
 
  if(currentTimeStamp - previousTimeStamp > 1000) 
  {
    previousTimeStamp = currentTimeStamp; 
    int result = DHT.read11(DHT11_PIN);
    switch (result)
    {
      case DHTLIB_OK:
  		break;
      case DHTLIB_ERROR_CHECKSUM: 
  		Serial.print("Checksum error,\t"); 
  		break;
      case DHTLIB_ERROR_TIMEOUT: 
  		Serial.print("Time out error,\t"); 
  		break;
      default: 
  		Serial.print("Unknown error,\t"); 
  		break;
    }
  }
  
  int weatherValue = displayMode == Temparature ? DHT.temperature : DHT.humidity;
  int w1 = weatherValue / 10;
  int w2 = weatherValue % 10;
  
  showDigit(0, w1);    
  showDigit(1, w2);
  showDigit(3, displayMode == Temparature ? 10 : 11);
}  
