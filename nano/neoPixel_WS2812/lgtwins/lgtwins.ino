// Adafruit_NeoMatrix example for single NeoPixel Shield.
// Scrolls 'Howdy' across the matrix in a portrait (vertical) orientation.

#include <Adafruit_GFX.h>
#include <Adafruit_NeoMatrix.h>
#include <Adafruit_NeoPixel.h>
#ifndef PSTR
 #define PSTR // Make Arduino Due happy
#endif

#define PIN 6

#define MAX_WIDTH 32
#define MAX_HEIGHT 8

// MATRIX DECLARATION:
// Parameter 1 = width of NeoPixel matrix
// Parameter 2 = height of matrix
// Parameter 3 = pin number (most are valid)
// Parameter 4 = matrix layout flags, add together as needed:
//   NEO_MATRIX_TOP, NEO_MATRIX_BOTTOM, NEO_MATRIX_LEFT, NEO_MATRIX_RIGHT:
//     Position of the FIRST LED in the matrix; pick two, e.g.
//     NEO_MATRIX_TOP + NEO_MATRIX_LEFT for the top-left corner.
//   NEO_MATRIX_ROWS, NEO_MATRIX_COLUMNS: LEDs are arranged in horizontal
//     rows or in vertical columns, respectively; pick one or the other.
//   NEO_MATRIX_PROGRESSIVE, NEO_MATRIX_ZIGZAG: all rows/columns proceed
//     in the same order, or alternate lines reverse direction; pick one.
//   See example below for these values in action.
// Parameter 5 = pixel type flags, add together as needed:
//   NEO_KHZ800  800 KHz bitstream (most NeoPixel products w/WS2812 LEDs)
//   NEO_KHZ400  400 KHz (classic 'v1' (not v2) FLORA pixels, WS2811 drivers)
//   NEO_GRB     Pixels are wired for GRB bitstream (most NeoPixel products)
//   NEO_GRBW    Pixels are wired for GRBW bitstream (RGB+W NeoPixel products)
//   NEO_RGB     Pixels are wired for RGB bitstream (v1 FLORA pixels, not v2)


// Example for NeoPixel Shield.  In this application we'd like to use it
// as a 5x8 tall matrix, with the USB port positioned at the top of the
// Arduino.  When held that way, the first pixel is at the top right, and
// lines are arranged in columns, progressive order.  The shield uses
// 800 KHz (v2) pixels that expect GRB color data.
Adafruit_NeoMatrix matrix = Adafruit_NeoMatrix(MAX_WIDTH, MAX_HEIGHT, PIN,
  NEO_MATRIX_BOTTOM     + NEO_MATRIX_RIGHT +
  NEO_MATRIX_COLUMNS + NEO_MATRIX_ZIGZAG,
  NEO_GRB            + NEO_KHZ800);

// 스크롤할 문장들
const char* messages[] = {
  "LG TWINS",
  "LET'S GO LG",
  "HOME RUN LG",
  "LG TWINS",
  "RUN AND HIT",
  "KOREA SERIES VICTORY",
  "LG TWINS"
};

// 각 문장의 색상 (RGB)
const uint16_t colors[][3] = {
  {255, 255, 255},      // 빨강 - LG TWINS
  {0, 255, 0},      // 초록 - LET'S GO LG
  {255, 255, 0},    // 노랑 - HOME RUN LG
  {0, 100, 255},    // 파랑 - LG TWINS FOREVER
  {255, 0, 255}     // 자주색 - RUN AND HIT
};

const int NUM_MESSAGES = sizeof(messages) / sizeof(messages[0]);  // 메시지 개수 자동 계산
int scrollPos = MAX_WIDTH;  // 스크롤 위치
const int MESSAGE_SPACING = 5;  // 문장 사이 간격 (픽셀)

void setup() {
  matrix.begin();
  matrix.setBrightness(100);  // 밝기 조절 (0-255)
  matrix.setTextWrap(false);  // 텍스트 줄바꿈 방지
  matrix.setTextSize(1);      // 텍스트 크기
}

void loop() {
  matrix.clear();
  
  // 전체 스크롤 길이 계산
  int totalWidth = 0;
  for (int i = 0; i < NUM_MESSAGES; i++) {
    totalWidth += strlen(messages[i]) * 6 + MESSAGE_SPACING;
  }
  
  // 모든 문장을 연속으로 그리기
  int currentPos = scrollPos;
  for (int i = 0; i < NUM_MESSAGES; i++) {
    // 현재 문장의 색상 설정
    uint16_t color = matrix.Color(
      colors[i][0],
      colors[i][1],
      colors[i][2]
    );
    
    matrix.setTextColor(color);
    matrix.setCursor(currentPos, 0);
    matrix.print(messages[i]);
    
    // 다음 문장 위치 계산
    currentPos += strlen(messages[i]) * 6 + MESSAGE_SPACING;
  }
  
  matrix.show();
  
  scrollPos--;
  
  // 전체가 화면을 벗어나면 다시 시작
  if (scrollPos < -totalWidth) {
    scrollPos = MAX_WIDTH;
  }
  
  delay(60);  // 스크롤 속도 조절 (값이 클수록 느림)
}