#ifndef core_h
#define core_h

// tetrisflappy/core.h 에서 공통 부분만 뽑아 게임별 스케치에서 그대로 쓰도록 정리한 파일.
// 6개 게임 디렉토리에 같은 내용이 복사되어 있다. (각 스케치가 독립적으로 컴파일되도록)

#include <SPI.h>
#include <EEPROM.h>
#include "src/Adafruit_GFX/Adafruit_GFX.h"
#include "src/Adafruit_ST7735/Adafruit_ST7735.h"

// ---------------
// 핀 배치 (tetrisflappy 와 동일)
// ---------------
#define TFT_CS            10
#define TFT_DC            9
#define TFT_RST           8
#define PUSH_BUTTON       2
#define JOY_X             A0
#define JOY_Y             A1
#define BUZZER            7

// 화면 크기 (ST7735 BLACKTAB 세로 방향)
#define TFTW              128
#define TFTH              160
#define TFTW2             64
#define TFTH2             80

Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_RST);

// ---------------
// 색
// ---------------
// 5-6-5 색을 컴파일 타임에 만든다. tft.Color565() 와 달리 상수로 접히기 때문에
// const 로 선언해도 RAM 을 잡아먹지 않는다.
#define RGB565(r, g, b)   ((uint16_t)((((r) & 0xF8) << 8) | (((g) & 0xFC) << 3) | ((b) >> 3)))

#define COLOR_BLACK       ST7735_BLACK
#define COLOR_WHITE       ST7735_WHITE
#define COLOR_RED         ST7735_RED
#define COLOR_GREEN       ST7735_GREEN
#define COLOR_BLUE        ST7735_BLUE
#define COLOR_CYAN        ST7735_CYAN
#define COLOR_MAGENTA     ST7735_MAGENTA
#define COLOR_YELLOW      ST7735_YELLOW
#define COLOR_GRAY        tft.Color565(33, 33, 33)
#define COLOR_DARKGRAY    tft.Color565(80, 80, 80)
#define COLOR_ORANGE      tft.Color565(255, 165, 0)
#define COLOR_PURPLE      tft.Color565(160, 60, 200)
#define COLOR_LIME        tft.Color565(204, 255, 0)
#define COLOR_PINK        tft.Color565(255, 105, 180)
#define COLOR_BROWN       tft.Color565(139, 90, 43)

// ---------------
// 버튼
// ---------------
bool buttonDown() {
  return digitalRead(PUSH_BUTTON) == LOW;
}

// 버튼이 떨어질 때까지 대기. (한 번 누른게 다음 화면까지 넘어가는걸 방지)
void waitForRelease() {
  while (buttonDown()) {
    delay(10);
  }

  delay(50);
}

// 버튼을 새로 누를 때까지 대기.
void waitForPress() {
  waitForRelease();

  while (!buttonDown()) {
    delay(10);
  }
}

// ---------------
// 조이스틱 (중립값 자동 보정)
// ---------------
// 512 를 중립으로 가정한 고정 임계값을 쓰면 모듈 편차나 전원 전압에 따라
// 중립값이 그 밖으로 벗어나서 가만히 둬도 한쪽으로 계속 움직인다.
// 그래서 부팅할 때 실제 중립값을 측정해서 기준으로 쓴다.
#define JOY_DEADZONE      50    // 중립에서 이만큼 벗어나야 입력으로 인정
#define JOY_FULL          250   // 이만큼 벗어나면 끝까지 기울인 것으로 본다

// "한 칸씩" 움직이는 화면(메뉴, 크로시)에서 손을 뗐다고 인정하는 시간.
// 판정 문턱(JOY_DEADZONE) 근처로 살짝 기울이면 DIR_NONE 과 방향이 번갈아
// 읽히는데, 그 순간적인 떨림을 "새로 기울였다"로 보면 한 번 기울인 것이
// 수십 번의 입력으로 처리된다. 중립이 이만큼 이어져야 새 입력으로 본다.
#define JOY_NEUTRAL_MS    60

short joyCenterX = 512;
short joyCenterY = 512;

// 지금 읽히는 값을 그대로 중립값으로 삼는다.
// 부팅할 때 한 번 호출한다. 그 순간 조이스틱을 기울이고 있었다면 기준이
// 틀어지지만, 그 경우는 리셋하면 되므로 따로 처리하지 않는다.
// (노이즈만 줄이려고 16번 평균)
void calibrateJoystick() {
  long sumX = 0;
  long sumY = 0;

  for (byte i = 0; i < 16; i++) {
    sumX += analogRead(JOY_X);
    sumY += analogRead(JOY_Y);
    delay(2);
  }

  joyCenterX = sumX / 16;
  joyCenterY = sumY / 16;
}

// 중립 기준 기울기. 배선상 X 는 값이 커지면 왼쪽, Y 는 값이 작아지면 아래쪽이다.
// 양수면 왼쪽 / 아래쪽, 음수면 오른쪽 / 위쪽, 절대값이 기울인 정도.
short joyLeftward() {
  return analogRead(JOY_X) - joyCenterX;
}

short joyDownward() {
  return joyCenterY - analogRead(JOY_Y);
}

// 4방향 입력. 더 많이 기울어진 축 하나만 인정한다.
#define DIR_NONE          0
#define DIR_UP            1
#define DIR_DOWN          2
#define DIR_LEFT          3
#define DIR_RIGHT         4

byte joyDir() {
  short leftward = joyLeftward();
  short downward = joyDownward();

  short absX = leftward < 0 ? -leftward : leftward;
  short absY = downward < 0 ? -downward : downward;

  if (absX < JOY_DEADZONE && absY < JOY_DEADZONE) {
    return DIR_NONE;
  }

  if (absX > absY) {
    return leftward > 0 ? DIR_LEFT : DIR_RIGHT;
  }

  return downward > 0 ? DIR_DOWN : DIR_UP;
}

// 기울인 정도를 0 ~ range 위치로 바꾼다. (패들처럼 비례 제어가 필요할 때)
// 왼쪽 끝까지 기울이면 0, 오른쪽 끝까지 기울이면 range.
int16_t joyPosX(int16_t range) {
  short leftward = joyLeftward();

  if (leftward > JOY_FULL) leftward = JOY_FULL;
  if (leftward < -JOY_FULL) leftward = -JOY_FULL;

  return (int32_t)(JOY_FULL - leftward) * range / (2L * JOY_FULL);
}

// joyPosX 를 그대로 쓰면 조이스틱이 가리키는 자리로 한 번에 순간이동한다.
// 이동 폭(104px 남짓)이 ADC 값 ±JOY_FULL 에 다 눌러담겨 있어서, 살짝만 튕겨도
// 한 프레임에 수십 픽셀이 건너뛰어진다.
// 그래서 목표 위치까지 한 프레임에 maxStep 픽셀씩만 따라가게 한다.
// current 에는 지금 위치를 넣고, 돌려받은 값을 다시 지금 위치로 쓰면 된다.
int16_t joyFollowX(int16_t current, int16_t range, int16_t maxStep) {
  int16_t diff = joyPosX(range) - current;

  // 1px 이내 차이는 무시. 가만히 쥐고 있는 손의 떨림과 ADC 잡음까지
  // 따라가면 제자리에서 계속 덜덜거린다.
  if (diff >= -1 && diff <= 1) {
    return current;
  }

  if (diff > maxStep) diff = maxStep;
  if (diff < -maxStep) diff = -maxStep;

  return current + diff;
}

// ---------------
// 소리 (부저 D7)
// ---------------
// tone() 은 지정한 시간이 지나면 알아서 멈추므로 게임 루프를 막지 않는다.
void beep(unsigned int freq, unsigned long ms) {
  tone(BUZZER, freq, ms);
}

// ---------------
// 그리기 보조
// ---------------
// 라이브러리의 fillRect 는 화면 오른쪽/아래로 넘치는 것만 잘라주고 음수 좌표는
// 그대로 setAddrWindow 에 넘겨서 화면이 깨진다. 스크롤하는 게임처럼 좌표가
// 음수가 될 수 있는 곳에서는 이걸 쓴다.
void fillRectClipped(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color) {
  if (w <= 0 || h <= 0) return;
  if (x >= TFTW || y >= TFTH) return;

  if (x < 0) { w += x; x = 0; }
  if (y < 0) { h += y; y = 0; }

  if (w <= 0 || h <= 0) return;
  if (x + w > TFTW) w = TFTW - x;
  if (y + h > TFTH) h = TFTH - y;

  tft.fillRect(x, y, w, h, color);
}

// 위와 같지만 y 를 [top, bottom) 안으로도 잘라준다. (아래에 점수줄이 있는 게임용)
void fillRectClippedY(int16_t x, int16_t y, int16_t w, int16_t h,
                      int16_t top, int16_t bottom, uint16_t color) {
  if (y < top) { h += (y - top); y = top; }
  if (y + h > bottom) h = bottom - y;

  fillRectClipped(x, y, w, h, color);
}

// ---------------
// 글자
// ---------------
// 화면 가운데에 글자 출력. (글자 하나가 6px * size)
void centerText(const char *text, int16_t y, uint16_t color, byte size = 1) {
  tft.setTextSize(size);
  tft.setTextColor(color);
  tft.setCursor(TFTW2 - (int16_t)(strlen(text) * 3 * size), y);
  tft.print(text);
}

void centerNumber(unsigned int value, int16_t y, uint16_t color, byte size = 1) {
  char buf[7];
  utoa(value, buf, 10);
  centerText(buf, y, color, size);
}

// ---------------
// 최고 기록 (EEPROM 영구 저장)
// ---------------
// 게임별로 3바이트씩 사용: [매직 1바이트][점수 2바이트]
// 매직값이 없으면 아직 저장된 기록이 없다는 뜻.
// tetrisflappy 가 0~5번지를 쓰고 있으므로 새 게임은 10번지부터 나눠 쓴다.
#define EEPROM_MAGIC      0xA5

// 저장된 기록이 있으면 outScore 에 담아 true, 없으면 false.
bool loadHighScore(int addr, unsigned int &outScore) {
  if (EEPROM.read(addr) != EEPROM_MAGIC) {
    return false;
  }

  EEPROM.get(addr + 1, outScore);
  return true;
}

// 기존 기록보다 높거나, 저장된 기록이 아예 없으면 저장. 저장했으면 true.
bool saveHighScore(int addr, unsigned int score) {
  unsigned int high;

  if (loadHighScore(addr, high) && score <= high) {
    return false;
  }

  EEPROM.write(addr, EEPROM_MAGIC);
  EEPROM.put(addr + 1, score);
  return true;
}

// ---------------
// 공통 화면
// ---------------
// 타이틀 화면. 버튼을 누르면 넘어간다.
void titleScreen(const char *title, const char *subtitle, const char *hint, uint16_t color) {
  tft.fillScreen(COLOR_BLACK);

  centerText(title, TFTH2 - 30, color, 2);

  if (subtitle) {
    centerText(subtitle, TFTH2 - 8, COLOR_WHITE, 1);
  }

  tft.drawFastHLine(10, TFTH2 + 6, TFTW - 20, COLOR_DARKGRAY);

  if (hint) {
    centerText(hint, TFTH2 + 16, COLOR_DARKGRAY, 1);
  }

  centerText("PRESS BUTTON", TFTH2 + 46, COLOR_WHITE, 1);

  waitForPress();
}

// 게임 시작시 최고 기록을 잠깐 보여준다.
#define HIGHSCORE_SHOW_MS 1000

void showHighScore(const char *gameName, int addr) {
  unsigned int high;
  bool hasRecord = loadHighScore(addr, high);

  tft.fillScreen(COLOR_BLACK);

  centerText(gameName, TFTH2 - 20, COLOR_WHITE, 1);
  centerText("HIGH SCORE", TFTH2, COLOR_WHITE, 1);

  if (hasRecord) {
    centerNumber(high, TFTH2 + 15, COLOR_YELLOW, 1);
  } else {
    centerText("NO RECORD", TFTH2 + 15, COLOR_YELLOW, 1);
  }

  delay(HIGHSCORE_SHOW_MS);

  tft.fillScreen(COLOR_BLACK);
  tft.setTextSize(1);
  tft.setTextColor(COLOR_WHITE);
}

// 게임 오버 화면. 최고 기록이면 저장하고 알려준다. 버튼을 누르면 넘어간다.
void gameOverScreen(unsigned int score, int addr) {
  bool isNewRecord = saveHighScore(addr, score);

  delay(600);

  tft.fillScreen(COLOR_BLACK);

  centerText("GAME", TFTH2 - 46, COLOR_RED, 2);
  centerText("OVER", TFTH2 - 26, COLOR_RED, 2);

  centerText("SCORE", TFTH2 + 2, COLOR_WHITE, 1);
  centerNumber(score, TFTH2 + 14, COLOR_YELLOW, 2);

  if (isNewRecord) {
    centerText("NEW RECORD!", TFTH2 + 40, COLOR_LIME, 1);
    beep(880, 120);
    delay(140);
    beep(1174, 200);
  } else {
    unsigned int high;

    if (loadHighScore(addr, high)) {
      tft.setTextSize(1);
      tft.setTextColor(COLOR_DARKGRAY);
      tft.setCursor(TFTW2 - 30, TFTH2 + 40);
      tft.print("BEST ");
      tft.print(high);
    }
  }

  centerText("PRESS BUTTON", TFTH2 + 60, COLOR_WHITE, 1);

  waitForPress();
}

// ---------------
// 부팅 초기화. 각 스케치의 setup() 맨 앞에서 한 번 호출한다.
// ---------------
void coreBegin() {
  pinMode(PUSH_BUTTON, INPUT_PULLUP);
  pinMode(JOY_X, INPUT);
  pinMode(JOY_Y, INPUT);
  pinMode(BUZZER, OUTPUT);

  tft.initR(INITR_BLACKTAB);
  tft.fillScreen(COLOR_BLACK);

  // 조이스틱 중립값 측정. 이 시점에 조이스틱을 건드리면 안 된다.
  calibrateJoystick();

  // 아무것도 연결되지 않은 아날로그 핀의 잡음을 난수 시드로 쓴다.
  randomSeed(analogRead(A5));
}

#endif
