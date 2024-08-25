#include <SoftwareServo.h>
#include <tiny_IRremote.h>

int pos = 0;                           // variable to store the servo position
int SERVO_PIN = PB0;
int RECV_PIN = PB1;
bool hit = false;
bool first = true;

SoftwareServo myservo;
IRrecv irrecv(RECV_PIN);
decode_results results;

void setup()
{
  irrecv.enableIRIn();                        // Start the receiverirrecv.enableIRIn(); // Start the receiver
  myservo.attach(SERVO_PIN);                  // attaches the servo on PB0 to the servo object

  //  myservo.write(90);
  //  delay(20);
  //  SoftwareServo::refresh();
}

void loop()
{
  if (first) {
    warmup();
    first = false;
  }

  if (irrecv.decode(&results) && !hit) {
    hit = true;
    shoot();
    irrecv.resume();
  }
}

void shoot() {

  myservo.write(180);              // tell servo to go to position in variable 'pos'
  SoftwareServo::refresh();        // must call at least once every 50ms or so to keep your servos updating
  delay(1000);                       // waits 15ms for the servo to reach the positionmyservo.write(0);              // tell servo to go to position in variable 'pos'

  myservo.write(90);              // tell servo to go to position in variable 'pos'
  SoftwareServo::refresh();        // must call at least once every 50ms or so to keep your servos updating
  delay(1000);

  hit = false;
}

void warmup()
{
  for(pos = 90; pos < 180; pos += 1)  // goes from 0 degrees to 180 degrees 
  {                                  // in steps of 1 degree 
    myservo.write(pos);              // tell servo to go to position in variable 'pos' 
    SoftwareServo::refresh();        // must call at least once every 50ms or so to keep your servos updating
    delay(15);                       // waits 15ms for the servo to reach the position 
  } 
  for(pos = 180; pos>=90; pos-=1)     // goes from 180 degrees to 0 degrees 
  {                                
    myservo.write(pos);              // tell servo to go to position in variable 'pos' 
    SoftwareServo::refresh();
    delay(15);                       // waits 15ms for the servo to reach the position 
  } 
}
