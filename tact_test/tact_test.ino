#define TACT_PIN 14

unsinged long previous = millis();

void setup()
{
  pinMode(14,INPUT);
  Serial.begin(9600);
}

void loop()
{
  unsigned long current = millis();
  
  if (current - previous > 1000 )
  {
    previous = current;
      
    int test = digitalRead(TACT_PIN);
    
    if (test == HIGH)
    {
      Serial.print("HIGH");
    }
    else
    {
      Serial.print("LOW");    
    }    
  }
}
