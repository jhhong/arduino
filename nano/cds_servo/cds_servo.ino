#include <Servo.h>

const int lightSensorPin1 = A0; // 조도 센서1 핀 설정
const int lightSensorPin2 = A1; // 조도 센서1 핀 설정
const int servoPinLeft = 7; // Left 서보모터 핀 설정
const int servoPinRight = 8; // Right 서보모터 핀 설정
const int threshold = 500; // 조도 센서 임계값 설정

Servo servoLeft, servoRight;

void setup() {
  // Serial.begin(9600);
  moveLeftServo(0, 500);
  moveRightServo(0, 500);
}

void loop() {
  // Serial.println(lightValue);

  int lightValue1 = analogRead(lightSensorPin1); // 조도 센서 값 읽기
  int lightValue2 = analogRead(lightSensorPin2); // 조도 센서 값 읽기

  if (lightValue1 >= threshold) {
    hitLeftServo();
  }

  if (lightValue2 >= threshold) {
    hitRightServo();
  }  

  delay(50);
}

// 모터의 축이 길경우 위치를 잡으려는 떨림이 심해 매번 attach , detach 반복으로 우회
// -> 별 효과없음. 축의 길이를 줄이고 저항을 줄여서 해결
void moveLeftServo(int degree, int delayMilliSec) {
    servoLeft.attach(servoPinLeft);
    servoLeft.write(degree); 
    delay(delayMilliSec);
    servoLeft.detach();
}

void moveRightServo(int degree, int delayMilliSec) {
    servoRight.attach(servoPinRight);
    servoRight.write(degree); 
    delay(delayMilliSec);
    servoRight.detach();
}

void hitLeftServo() {
  moveLeftServo(90, 500);
  moveLeftServo(0, 500);
}

void hitRightServo() {
  moveRightServo(90, 500);
  moveRightServo(0, 500);
}