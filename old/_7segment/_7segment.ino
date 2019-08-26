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
const int segmentPins[8] = {5,8,9,7,6,4,3,2};
//const int segmentPins[8] = {2,3,4,6,7,9,8,5};

void setup()

{
  for(int i=0;i<8;i++)
  {
    pinMode(segmentPins[i] , OUTPUT);
  }
  
  Serial.begin(9600);
}

void loop()
{
  for(int i=0;i<=10;i++)
  {
    showDigit(i);
    delay(1000);
  }
  
//  delay(2000);
  
}


void showDigit(int number)
{
Serial.println(number);          
  boolean isBitSet;
  for(int segment=1;segment<8;segment++)
  {
    if(number <0 || number >9)
    {
      isBitSet = 0;
    }
    else
    {
      isBitSet = bitRead(numeral[number],segment);
      Serial.print(isBitSet);      
    }
    isBitSet = !isBitSet;
    digitalWrite(segmentPins[segment], isBitSet);
  }
Serial.println("==================\n");        
}
