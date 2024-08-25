#include <SoftwareServo.h>

SoftwareServo myservo;                 // create servo object to control a servo
// a maximum of eight servo objects can be created

int pos = 0;                           // variable to store the servo position

void setup()
{
  myservo.attach(PB0);                  // attaches the servo on PB1 to the servo object
}

void loop()
{
  myservo.write(0);              // tell servo to go to position in variable 'pos'
  SoftwareServo::refresh();        // must call at least once every 50ms or so to keep your servos updating
  delay(500);                       // waits 15ms for the servo to reach the position
  myservo.write(180);              // tell servo to go to position in variable 'pos'
  SoftwareServo::refresh();        // must call at least once every 50ms or so to keep your servos updating
  delay(500);                       // waits 15ms for the servo to reach the position
}
