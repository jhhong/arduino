#include <avr/sleep.h>

#define MAX_KEYS 12
int currentSize = 0;

// WN = Whole Note 1
// HN = Half Note 2
// QN = Quarter Note 4
// EN = Eight Note 8
// SN = Sixteenth Note 16

int QN = 250;
int HN = QN * 2;
int WN = HN * 2;
int EN = QN / 2;
int SN = EN / 2;

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

void changeSpeed(int speed){
    QN = speed;
    HN = QN * 2;
    WN = HN * 2;
    EN = QN / 2;
    SN = EN / 2;  
}

int speakerPIN = PB0;
int switchPIN = PB3;
int t = 0;
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

  pinMode(switchPIN, INPUT_PULLUP);
  pinMode(speakerPIN, OUTPUT); // set a pin for buzzer output

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
    if (digitalRead(switchPIN) == 1) {
      delayMicroseconds(300);
    } else {
      playSound();
    }
  }
}

void playSound() {

  if (t % 5 == 0) {
    youshowUp(350);
  } else if (t % 5 == 1) {
    daeho(350);
  } else if (t % 5 == 2) {
    storm_and_gale(250);
  } else if (t % 5 == 3) {
    dream_of_octopus(350);
  } else if (t % 5 == 4) {
    HAPPY_BIRTHDAY_TO_YOU(250);
  }

  delay(200);
  t++;
}

//나타나
void youshowUp(int speed) {
  changeSpeed(speed);

  buzz("G", 6,EN);
  buzz("G", 7,EN);
  buzz("F", 7,EN);
  buzz("F", 7,EN);
  buzz("E", 7,EN);
  buzz("E", 7,QN);
  buzz("F", 7,EN);
  buzz("F", 7,HN + EN);

  buzz("G", 6,EN);
  buzz("F", 7,EN);
  buzz("E", 7,EN);
  buzz("E", 7,EN);
  buzz("D", 7,EN);
  buzz("D", 7,QN);
  buzz("E", 7,EN);
  buzz("E", 7,HN + EN);

  buzz("G", 6,EN);
  buzz("E", 7,EN);
  buzz("D", 7,EN);
  buzz("D", 7,EN);
  buzz("C", 7,EN);
  buzz("C", 7,QN);
  buzz("D", 7,EN);
  buzz("D", 7,QN + EN);
  
  buzz("D", 7,EN);
  buzz("E", 7,EN);
  buzz("F", 7,QN);
  buzz("E", 7,EN);
  buzz("D", 7,QN + EN);
  buzz("E", 7,QN);
  buzz("C#", 7,QN);
  buzz("A",  6,HN + QN);
  
  buzz("G",  7,HN);

  buzz("G", 7,EN);
  buzz("G", 7,EN);
  buzz("F", 7,EN);
  buzz("F", 7,EN);
  buzz("E", 7,EN);
  buzz("E", 7,QN);
  buzz("F", 7,EN);
  buzz("F", 7,HN + EN);

  buzz("A", 7,EN);
  buzz("A", 7,EN);
  buzz("G", 7,EN);
  buzz("G", 7,EN);  
  buzz("F#",7,EN);  
  buzz("F#",7,QN);  
  buzz("G", 7,EN);  
  buzz("G", 7,HN + EN);  

  buzz("E", 7,EN);
  buzz("E", 7,EN);
  buzz("D", 7,EN);
  buzz("C", 7,EN);  
  buzz("D", 7,EN);  
  buzz("D", 7,QN + EN);  
  buzz("D", 7,QN);  
  buzz("C", 7,EN);  
  buzz("B", 6,EN);  
  buzz("C", 7,EN);  
  buzz("C", 7,QN + EN);  
  buzz("C", 7,QN);  
  buzz("B", 6,EN);  
  buzz("B", 6,EN);  
  buzz("A", 6,EN);  
  buzz("A", 6,QN);  
  buzz("B", 6,EN);  
  buzz("C", 7,QN);  
  buzz("G", 7,QN);  
  buzz("G", 7,HN);  

  buzz("G", 6,EN);  
  buzz("E", 7,EN);  
  buzz("D", 7,EN);  
  buzz("C", 7,EN);  
  buzz("D", 7,QN);  
  buzz("C", 7,WN);  
}

//happy birthday to u
void HAPPY_BIRTHDAY_TO_YOU(int speed){
    changeSpeed(speed);

      buzz("G",6,EN);
      buzz("G",6,EN);
      buzz("A",6,QN);
      buzz("G",6,QN);
      buzz("C",7,QN);
      buzz("B",6,WN);
      buzz("G",6,EN);
      buzz("G",6,EN);
      buzz("A",6,QN);
      buzz("G",6,QN);
      buzz("D",7,QN);
      buzz("C",7,WN);
      buzz("G",6,EN);
      buzz("G",6,EN);
      buzz("G",7,QN);
      buzz("E",7,QN);
      buzz("C",7,QN);
      buzz("B",6,QN);
      buzz("A",6,QN);
      buzz("F",7,EN);
      buzz("F",7,EN);
      buzz("E",7,QN);
      buzz("C",7,QN);
      buzz("D",7,QN);
      buzz("C",7,WN);         
}

//질풍가도
void storm_and_gale(int speed){
    changeSpeed(speed);

      buzz("G", 6,QN);
      buzz("A", 6,QN);
      buzz("B", 6,HN);
      buzz("C", 7,QN);
      buzz("B", 6,EN);
      buzz("A", 6,HN + EN);
      buzz("B", 6,QN);
      buzz("A", 6,EN);
      buzz("G", 6,QN + EN);
      buzz("G", 6,QN);
      buzz("G", 6,EN);
      buzz("A", 6,EN);
      buzz("B", 6,EN);  
      buzz("B", 6,HN + EN);    
      buzz("E", 6,QN);    
      buzz("F#",6,EN);    
      buzz("G", 6,QN + EN);    
      buzz("G", 6,QN);    
      buzz("A", 6,QN);    
      buzz("G", 6,EN);    
      buzz("F#",6,QN + EN);    
      buzz("F#",6,QN);    
      buzz("E", 6,QN);    
      buzz("D", 6,EN);    
      buzz("E", 6,WN);

      buzz("G", 6,QN);
      buzz("A", 6,QN);
      buzz("B", 6,HN);
      buzz("C", 7,QN);
      buzz("B", 6,EN);
      buzz("A", 6,HN + EN);
      buzz("B", 6,QN);
      buzz("A", 6,EN);
      buzz("G", 6,QN + EN);
      buzz("G", 6,QN);
      buzz("G", 6,EN);
      buzz("A", 6,EN);
      buzz("B", 6,EN);  
      buzz("B", 6,HN + EN);    
      buzz("E", 6,QN);    
      buzz("F#",6,EN);    
      buzz("G", 6,QN + EN);    
      buzz("G", 6,QN);    
      buzz("A", 6,QN);    
      buzz("G", 6,EN);    
      buzz("F#",6,HN + EN);    
      buzz("B", 6,QN);    
      buzz("B", 6,QN);    
      buzz("A", 6,EN);  
      buzz("G", 6,WN + QN + EN);  
      buzz("E", 6,QN);    
      buzz("D", 6,QN);    
      buzz("G", 6,WN + QN + EN);
}

//문어의 꿈
void dream_of_octopus(int speed){
  changeSpeed(speed);
  
  buzz("D", 6,EN + SN);
  buzz("C", 6,SN);
  buzz("B", 5,EN + SN);
  buzz("D", 6,HN + SN);
  buzz("B", 5,EN + SN);
  buzz("D", 6,QN);
  buzz("B", 6,QN);
  buzz("B", 6,SN);
  buzz("A", 6,EN + SN);
  buzz("G", 6,HN + QN + SN);
  
  buzz("A", 6,EN + SN);
  buzz("G", 6,SN);
  buzz("F#",6,QN);
  buzz("D", 6,EN + SN);
  buzz("D", 6,SN);
  buzz("D", 6,EN + SN);
  buzz("D", 6,SN);  
  buzz("D", 6,EN + SN);
  buzz("D", 6,QN);    
  buzz("A", 5,QN + SN);    
  buzz("D", 6,EN + SN);
  buzz("A", 5,SN);  
  buzz("C", 6,EN + SN);  
  buzz("B", 5,QN);  
  buzz("D", 6,WN);

  buzz("D", 6,EN + SN);
  buzz("C", 6,SN);
  buzz("B", 5,EN + SN);
  buzz("D", 6,HN + SN);
  buzz("B", 5,EN + SN);
  buzz("D", 6,QN);
  buzz("B", 6,QN);
  buzz("B", 6,SN);
  buzz("A", 6,EN + SN);
  buzz("G", 6,HN + QN);

  buzz("A", 6,EN + SN);
  buzz("G", 6,SN);
  buzz("F#",6,QN);
  buzz("D", 6,EN + SN);
  buzz("D", 6,SN);
  buzz("D", 6,EN + SN);
  buzz("D", 6,SN);  
  buzz("D", 6,EN + SN);
  buzz("D", 6,QN);    
  
  buzz("B", 6,QN);    
  buzz("B", 6,QN);
  buzz("B", 6,QN);  
  buzz("B", 6,SN);  
  buzz("E", 6,EN + SN);  
  buzz("G", 6,HN + QN);   
}

//이대호 응원가. 대 - 호 대 -호
void daeho(int speed){
  changeSpeed(speed);
  
  buzz("C", 6,HN);
  buzz("G", 5,HN);
  buzz("C", 6,HN);
  buzz("G", 5,HN);
  buzz("C", 6,HN);
  buzz("G", 5,HN);
  buzz("C", 6,HN);
  buzz("G", 5,HN);  
}

void buzz(const char* key, int octave , long length) {
  float frequency = getFrequency(key, octave);

  long delayValue = WDPI / frequency / 2; // calculate the delay value between transitions
  long numCycles = frequency * length / 1300; // calculate the number of cycles for proper timing
  for (long i = 0; i < numCycles; i++) { // for the calculated length of time...
    digitalWrite(speakerPIN, HIGH); // write the buzzer pin high to push out the diaphram
    delayMicroseconds(delayValue); // wait for the calculated delay value
    digitalWrite(speakerPIN, LOW); // write the buzzer pin low to pull back the diaphram
    delayMicroseconds(delayValue - 1); // wait againf or the calculated delay value
  }
  delay(20);
}
