#include <Servo.h>

const int lightSensorPin = A0; // 조도 센서 핀 설정
const int servoPin = 8; // 서보모터 핀 설정
const int threshold = 500; // 조도 센서 임계값 설정

Servo targetServo;

void setup() {
  // Serial.begin(9600);
  move(0, 500);
}

void loop() {
  // Serial.println(lightValue);
  int lightValue = analogRead(lightSensorPin); // 조도 센서 값 읽기
  
  if (lightValue >= threshold) {
    hit();
  }
}

// 모터의 축이 길경우 위치를 잡으려는 떨림이 심해 매번 attach , detach 반복으로 우회
// -> 별 효과없음. 축의 길이를 줄이고 저항을 줄여서 해결
void move(int degree, int delayMilliSec) {
    targetServo.attach(servoPin);
    targetServo.write(degree); 
    delay(delayMilliSec);
    targetServo.detach();
}

void hit() {
  move(90, 500);
  move(0, 500);
}