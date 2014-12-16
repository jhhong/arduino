
#include <Wire.h>
#include <Time.h>
#include <DS1307RTC.h>
#include <dht.h>
#include <EEPROM.h>
dht DHT;

#define DHT11_PIN 4
#define TACT_PIN 14

const byte numeral[12] = {

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

//dp, G,F,E,D,C,B,A
const int segmentPins[8] = {4,7,8,6,5,3,2,9};
const int digitalPins[4] = {10,11,12,13};
long previousMillis = 0;
long previousMillis2 = 0;
int addr = 0;
int mode = 1;
unsigned long currentMillis;
int result;

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
  addr = 0;
  mode = EEPROM.read(addr);

  mode = 3;
  
  if (mode == 2)
  {
    showTempOrHum(2);
  }
  else if (mode == 3)
  {
    showTempOrHum(3);
  }
  else
  {
    showTime();
  }
  
  unsigned long checktime = millis();  
//  if(checktime - previousMillis2 > 1000)
//  {
//    previousMillis2 = checktime;
    int test = digitalRead(TACT_PIN);
    Serial.println(test);
//  }  
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
}

void showTempOrHum(int mode)
{
  currentMillis = millis();
 
  if(currentMillis - previousMillis > 1000) 
  {
    previousMillis = currentMillis; 
    result = DHT.read11(DHT11_PIN);
    switch (result)
    {
      case DHTLIB_OK:  
  		Serial.print("OK,\t"); 
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
  
  int tempOrHum = mode == 2 ? DHT.temperature : DHT.humidity;
  int v1 = tempOrHum/10;
  int v2 = tempOrHum%10;
  
  showDigit(0, v1);    
  showDigit(1, v2);
  showDigit(3, mode == 2 ? 10 : 11);
}  
