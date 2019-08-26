#include <Wire.h>  // Comes with Arduino IDE
#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x3F, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE);  // or 0x27

void setup()
{
  lcd.begin(16,2);   // initialize the lcd for 16 chars 2 lines, turn on backlight
  lcd.backlight(); // finish with backlight on  
  lcd.clear();
}


void loop()   /*----( LOOP: RUNS CONSTANTLY )----*/
{
  lprint("I Love LeeSukMin","michi!",2000);
  lprint("& BAND iOS team",",too!",2000);  
  lprint("Coffee please","^^;;",2000);    
}

void lprint(char *str1, char*str2,int waitTime) {
  lcd.clear();
  lcd.setCursor(0,0);  
  lcd.print(str1);
  lcd.setCursor(0,1);  
  lcd.print(str2);
  delay(waitTime);
}
