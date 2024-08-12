#define MAX_KEYS 12
int currentSize = 0;

struct KeyFrequencyMap {
  char key[3];  // 두 개의 문자와 null 종결자 '\0'을 위한 공간
  float frequency;
};

KeyFrequencyMap keyFrequencyMap[MAX_KEYS];

void addKeyValue(const char* key, float value) {
  if (currentSize < MAX_KEYS) {
    strncpy(keyFrequencyMap[currentSize].key, key, 2); // 두 문자를 복사
    keyFrequencyMap[currentSize].key[2] = '\0'; // null 종결자 추가
    keyFrequencyMap[currentSize].frequency = value;
    currentSize++;
  }
}

long WDPI = 1500000;
int piezo = 8;

void setup() {
  addKeyValue("C", 32.7032);
  addKeyValue("C#", 34.6478);
  addKeyValue("D", 36.7081);
  addKeyValue("D#", 38.8909);
  addKeyValue("E", 41.2034);
  addKeyValue("F", 43.6535);
  addKeyValue("F#", 46.2493);
  addKeyValue("G", 48.9994);
  addKeyValue("G#", 51.9130);
  addKeyValue("A", 55.0000);
  addKeyValue("A#", 58.2705);
  addKeyValue("B", 61.7354);
  
  pinMode(piezo, OUTPUT);

  scale();
  // HAPPY_BIRTHDAY_TO_YOU();
}

float getFrequency(const char* key, int octave){
  float frequency = 0;

  for(int i = 0; i < MAX_KEYS; i++) {
    if(strcmp(keyFrequencyMap[i].key, key) == 0){
      frequency =  keyFrequencyMap[i].frequency;
      break;
    }
  }

  // pow(2, octave - 1) 대신 bit shift를 사용
  float octaveMultiplier = 1 << (octave - 1);
  float octaveFrequency = frequency * octaveMultiplier;

  return octaveFrequency;
}

void loop() {
  // put your main code here, to run repeatedly:

}

void scale(){
  buzz("C", 6,250);
  buzz("C#",6,250);
  buzz("D", 6,250);
  buzz("D#",6,250);
  buzz("E", 6,250);
  buzz("F", 6,250);
  buzz("F#",6,250);
  buzz("G", 6,250);
  buzz("G#",6,250);
  buzz("A", 6,250);
  buzz("A#",6,250);
  buzz("B", 6,250);
  buzz("C", 7,500);  
}

//happy birthday to u
void HAPPY_BIRTHDAY_TO_YOU() {
buzz("G",6,125);
buzz("G",6,125);
buzz("A",6,250);
buzz("G",6,250);
buzz("C",7,250);
buzz("B",6,500);
buzz("G",6,125);
buzz("G",6,125);
buzz("A",6,250);
buzz("G",6,250);
buzz("D",7,250);
buzz("C",7,500);
buzz("G",6,125);
buzz("G",6,125);
buzz("G",7,250);
buzz("E",7,250);
buzz("C",7,250);
buzz("B",6,250);
buzz("A",6,250);
buzz("F",7,125);
buzz("F",7,125);
buzz("E",7,250);
buzz("C",7,250);
buzz("D",7,250);
buzz("C",7,500);         
}

void buzz(const char* key, int octave , long length) {
  float frequency = getFrequency(key, octave);

  long delayValue = WDPI / frequency / 2; // calculate the delay value between transitions
  long numCycles = frequency * length / 1300; // calculate the number of cycles for proper timing
  for (long i = 0; i < numCycles; i++) { // for the calculated length of time...
    digitalWrite(piezo, HIGH); // write the buzzer pin high to push out the diaphram
    delayMicroseconds(delayValue); // wait for the calculated delay value
    digitalWrite(piezo, LOW); // write the buzzer pin low to pull back the diaphram
    delayMicroseconds(delayValue - 1); // wait againf or the calculated delay value
  }
  delay(20);
}
