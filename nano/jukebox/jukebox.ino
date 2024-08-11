#define C6  1047
#define D6  1175
#define E6  1319
#define F6  1397
#define G6  1568
#define A6  1760
#define B6  1976
#define C7  2093
#define D7  2349
#define E7  2637
#define F7  2794
#define G7  3136
#define A7  3520
#define B7  3951
#define C8  4186

long WDPI = 1500000;
int piezo = 8;

void setup() {
  pinMode(piezo, OUTPUT);

  HAPPY_BIRTHDAY_TO_YOU();
}

void loop() {
  // put your main code here, to run repeatedly:

}

//happy birthday to u
void HAPPY_BIRTHDAY_TO_YOU() {
  buzz(G6, 125);
  buzz(G6, 125);
  buzz(A6, 250);
  buzz(G6, 250);
  buzz(C7, 250);
  buzz(B6, 500);
  buzz(G6, 125);
  buzz(G6, 125);
  buzz(A6, 250);
  buzz(G6, 250);
  buzz(D7, 250);
  buzz(C7, 500);
  buzz(G6, 125);
  buzz(G6, 125);
  buzz(G7, 250);
  buzz(E7, 250);
  buzz(C7, 250);
  buzz(B6, 250);
  buzz(A6, 250);
  buzz(F7, 125);
  buzz(F7, 125);
  buzz(E7, 250);
  buzz(C7, 250);
  buzz(D7, 250);
  buzz(C7, 500);
}


void buzz(long frequency, long length) {
  long delayValue = WDPI / frequency / 2; // calculate the delay value between transitions
  //// 1 second's worth of microseconds, divided by the frequency, then split in half since
  //// there are two phases to each cycle
  long numCycles = frequency * length / 1300; // calculate the number of cycles for proper timing
  //// multiply frequency, which is really cycles per second, by the number of seconds to
  //// get the total number of cycles to produce
  for (long i = 0; i < numCycles; i++) { // for the calculated length of time...
    digitalWrite(piezo, HIGH); // write the buzzer pin high to push out the diaphram
    delayMicroseconds(delayValue); // wait for the calculated delay value
    digitalWrite(piezo, LOW); // write the buzzer pin low to pull back the diaphram
    delayMicroseconds(delayValue - 1); // wait againf or the calculated delay value
  }
  delay(20);
}