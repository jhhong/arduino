int piezo = 8;

void setup() {
  pinMode(piezo, OUTPUT);
  
  tone(piezo, 523); // 5옥타브 도
  delay(1000);
  tone(piezo, 587); // 레
  delay(1000);
  tone(piezo, 659); // 미
  delay(1000);
  tone(piezo, 698); // 파 
  delay(1000);
  tone(piezo, 784); // 솔
  delay(1000);
  tone(piezo, 880); // 라
  delay(1000);
  tone(piezo, 988); // 시
  delay(1000);
  tone(piezo, 1046); // 6옥타브 도
  delay(1000);
  noTone(piezo);
}

void loop() {
  
}