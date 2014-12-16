const int red = 3;
const int green = 5;
const int blue = 6;

void setup()
{
}

void loop()
{
  on(red,0);
  on(green,0);
  on(blue,0);  
}

void on(int pin,int degree)
{
  analogWrite(pin,degree);
  delay(1000);  
  analogWrite(pin,255);  
}
