#define MAX_KEYS 10
int currentSize = 0;

struct KeyFrequencyMap {
  char key;
  long frequency;
};

KeyFrequencyMap keyFrequencyMap[MAX_KEYS];

void addKeyValue(char key, int value) {
  if (currentSize < MAX_KEYS) {
    keyFrequencyMap[currentSize].key = key;
    keyFrequencyMap[currentSize].frequency = value;
    currentSize++;
  }
}

long WDPI = 1500000;
int piezo = 8;

void setup() {

  Serial.begin(115200);

  addKeyValue('C', 33); //32.7032
  addKeyValue('D', 37); //36.7081
  addKeyValue('E', 41); //41.2034
  addKeyValue('F', 44); //43.6535
  addKeyValue('G', 49); //48.9994
  addKeyValue('A', 55); //55.0000
  addKeyValue('B', 62); //61.7354

  pinMode(piezo, OUTPUT);

  HAPPY_BIRTHDAY_TO_YOU();
}

//C, 4
long getFrequency(char key, int octave){
  long frequency = 0;

  for(int i = 0; i < MAX_KEYS; i++) {
    if(keyFrequencyMap[i].key == key){
      frequency =  keyFrequencyMap[i].frequency;
      break;
    }
  }

  long octaveFrequency = frequency * pow(2, octave - 1);

  Serial.print("key=");
  Serial.println(key);
  Serial.print("frequency=");
  Serial.println(frequency);  
  Serial.print("octave=");
  Serial.println(octave);
  Serial.print("octaveFrequency=");
  Serial.println(octaveFrequency);

  return octaveFrequency;
}

void loop() {
  // put your main code here, to run repeatedly:

}

//happy birthday to u
void HAPPY_BIRTHDAY_TO_YOU() {
buzz('G',6,125);
buzz('G',6,125);
buzz('A',6,250);
buzz('G',6,250);
buzz('C',7,250);
buzz('B',6,500);
buzz('G',6,125);
buzz('G',6,125);
buzz('A',6,250);
buzz('G',6,250);
buzz('D',7,250);
buzz('C',7,500);
buzz('G',6,125);
buzz('G',6,125);
buzz('G',7,250);
buzz('E',7,250);
buzz('C',7,250);
buzz('B',6,250);
buzz('A',6,250);
buzz('F',7,125);
buzz('F',7,125);
buzz('E',7,250);
buzz('C',7,250);
buzz('D',7,250);
buzz('C',7,500);         

  // buzz(G6, 125);
  // buzz(G6, 125);
  // buzz(A6, 250);
  // buzz(G6, 250);
  // buzz(C7, 250);
  // buzz(B6, 500);
  // buzz(G6, 125);
  // buzz(G6, 125);
  // buzz(A6, 250);
  // buzz(G6, 250);
  // buzz(D7, 250);
  // buzz(C7, 500);
  // buzz(G6, 125);
  // buzz(G6, 125);
  // buzz(G7, 250);
  // buzz(E7, 250);
  // buzz(C7, 250);
  // buzz(B6, 250);
  // buzz(A6, 250);
  // buzz(F7, 125);
  // buzz(F7, 125);
  // buzz(E7, 250);
  // buzz(C7, 250);
  // buzz(D7, 250);
  // buzz(C7, 500);
}

void buzz(char key, int octave , long length) {
  long frequency = getFrequency(key, octave);

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