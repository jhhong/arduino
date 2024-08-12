#define MAX_KEYS 12

#include <avr/sleep.h>

int currentSize = 0;

int qt = 250; 
int db = qt * 2;
int half = qt / 2;

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

int sn = PB0;
int sw = PB3;
int t = 1;
long WDPI = 1500000;

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

  pinMode(sw, INPUT_PULLUP);
  pinMode(sn, OUTPUT); // set a pin for buzzer output

  GIMSK = 0b00100000;
  PCMSK = 0b00001000;
}

//C, 4
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
  sleep();
  waitInput();
}

void sleep() {
  ADCSRA &= ~_BV(ADEN);                   // ADC off
  set_sleep_mode(SLEEP_MODE_PWR_DOWN);    // replaces above statement

  sleep_enable();                         // Sets the Sleep Enable bit in the MCUCR Register (SE BIT)
  sei();                                  // Enable interrupts
  sleep_cpu();                            // sleep

  cli();                                  // Disable interrupts
  sleep_disable();                        // Clear SE bit
  ADCSRA |= _BV(ADEN);                    // ADC on

  sei();                                  // Enable interrupts
}

ISR(PCINT0_vect)
{
}

void waitInput() {
  for (int i = 0; i <= 30000; i++) {
    if (digitalRead(sw) == 1) {
      delayMicroseconds(300);
    } else {
      playSound();
    }
  }
}

void playSound() {

  if (t % 2 == 0) {
    storm_and_gale();
  }   else if (t % 2 == 1) {
    HAPPY_BIRTHDAY_TO_YOU();
  }

  delay(200);
  t++;
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

void storm_and_gale(){
  buzz("G", 6,qt);
  buzz("A", 6,qt);
  buzz("B", 6,db);
  buzz("C", 7,qt);
  buzz("B", 6,half);
  buzz("A", 6,db + half);
  buzz("B", 6,qt);
  buzz("A", 6,half);
  buzz("G", 6,qt + half);
  buzz("G", 6,qt);
  buzz("G", 6,half);
  buzz("A", 6,half);
  buzz("B", 6,half);  
  buzz("B", 6,db + half);    
  buzz("E", 6,qt);    
  buzz("F#",6,half);    
  buzz("G", 6,qt + half);    
  buzz("G", 6,qt);    
  buzz("A", 6,qt);    
  buzz("G", 6,half);    
  buzz("F#",6,qt + half);    
  buzz("F#",6,qt);    
  buzz("E", 6,qt);    
  buzz("D", 6,half);    
  buzz("E", 6,db + db);

  buzz("G", 6,qt);
  buzz("A", 6,qt);
  buzz("B", 6,db);
  buzz("C", 7,qt);
  buzz("B", 6,half);
  buzz("A", 6,db + half);
  buzz("B", 6,qt);
  buzz("A", 6,half);
  buzz("G", 6,qt + half);
  buzz("G", 6,qt);
  buzz("G", 6,half);
  buzz("A", 6,half);
  buzz("B", 6,half);  
  buzz("B", 6,db + half);    
  buzz("E", 6,qt);    
  buzz("F#",6,half);    
  buzz("G", 6,qt + half);    
  buzz("G", 6,qt);    
  buzz("A", 6,qt);    
  buzz("G", 6,half);    
  buzz("F#",6,db + half);    
  buzz("B", 6,qt);    
  buzz("B", 6,qt);    
  buzz("A", 6,half);  
  buzz("G", 6,db + db + qt + half);  
  buzz("E", 6,qt);    
  buzz("D", 6,qt);    
  buzz("G", 6,db + db + qt + half);    
}

void buzz(const char* key, int octave , long length) {
  float frequency = getFrequency(key, octave);

  long delayValue = WDPI / frequency / 2; // calculate the delay value between transitions
  long numCycles = frequency * length / 1300; // calculate the number of cycles for proper timing
  for (long i = 0; i < numCycles; i++) { // for the calculated length of time...
    digitalWrite(sn, HIGH); // write the buzzer pin high to push out the diaphram
    delayMicroseconds(delayValue); // wait for the calculated delay value
    digitalWrite(sn, LOW); // write the buzzer pin low to pull back the diaphram
    delayMicroseconds(delayValue - 1); // wait againf or the calculated delay value
  }
  delay(20);
}
