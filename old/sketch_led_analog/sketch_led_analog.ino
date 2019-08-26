const int led1=3;
const int led2=5;
const int led3=6;

int brightness =0;
int increment = 1;

void setup()
{
//  pinMode(led1,OUTPUT);
//  pinMode(led2,OUTPUT);
//  pinMode(led3,OUTPUT);  
}

void loop(){
 
  if(brightness > 255) 
  {
    increment = -1;
  }
  else if(brightness < 1)
  {
    increment = 1;
  }
  
  brightness = brightness + increment;
  
  
  analogWrite(led1,brightness);
  analogWrite(led2,brightness);
  analogWrite(led3,brightness);  
  
  delay(10);
}

void blinkALlOn()
{
    digitalWrite(led1,HIGH);
    digitalWrite(led2,HIGH);    
    digitalWrite(led3,HIGH);    
    delay(1000);
    digitalWrite(led1,LOW);
    digitalWrite(led2,LOW);    
    digitalWrite(led3,LOW); 
    delay(1000);    
    
}

void blinkLED(int pin,int duration)
{
//  for(int i=0;i<5;i++)
//  {
    digitalWrite(pin,HIGH);
    delay(duration);
    digitalWrite(pin,LOW);  
    delay(duration);      
//  }
}
