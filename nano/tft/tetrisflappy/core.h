#include <SPI.h>
#include <EEPROM.h>
#include "src/Adafruit_GFX/Adafruit_GFX.h"
#include "src/Adafruit_ST7735/Adafruit_ST7735.h"
#include "src/tetris/config.h"

// TFT display and SD card will share the hardware SPI interface.
// Hardware SPI pins are specific to the Arduino board type and
// cannot be remapped to alternate pins.  For Arduino Uno,
// Duemilanove, etc., pin 11 = MOSI, pin 12 = MISO, pin 13 = SCK.
// #define SD_CS    4  // Chip select line for SD card
#define TFT_CS            10  // Chip select line for TFT display
#define TFT_DC            9   // Data/command line for TFT
#define TFT_RST           8   // Reset line for TFT (or connect to +5V)
#define PUSH_BUTTON       2   // button

Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_RST);

// 게임이 끝나면 true 가 되고, 다시 게임 선택 화면으로 돌아간다.
bool returnToMenu = false;

// 버튼이 떨어질 때까지 대기. (한 번 누른게 다음 화면까지 넘어가는걸 방지)
void waitForRelease() {
  while (digitalRead(PUSH_BUTTON) == LOW) {
    delay(10);
  }

  delay(50);
}

// ---------------
// 조이스틱 (중립값 자동 보정)
// ---------------
// 512 를 중립으로 가정한 고정 임계값(440/490 등)을 쓰면, 조이스틱 모듈 편차나
// 전원 전압(3.3V 등)에 따라 중립값이 그 밖으로 벗어나서 가만히 둬도
// 한쪽으로 계속 움직인다. 그래서 부팅할 때 실제 중립값을 측정해서 기준으로 쓴다.
#define JOY_DEADZONE      50    // 중립에서 이만큼 벗어나야 입력으로 인정
#define JOY_FULL          250   // 이만큼 벗어나면 끝까지 기울인 것으로 본다

short joyCenterX = 512;
short joyCenterY = 512;

// 지금 읽히는 값을 그대로 중립값으로 삼는다.
// 게임 선택 화면이 뜨는 시점에 한 번 호출한다. 그 순간 조이스틱을 기울이고
// 있었다면 기준이 틀어지지만, 그 경우는 리셋하면 되므로 따로 처리하지 않는다.
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

// ---------------
// 최고 기록 (EEPROM 영구 저장)
// ---------------
// 게임별로 3바이트씩 사용: [매직 1바이트][점수 2바이트]
// 매직값이 없으면 아직 저장된 기록이 없다는 뜻.
#define EEPROM_MAGIC          0xA5
#define EEPROM_TETRIS_ADDR    0
#define EEPROM_FLAPPY_ADDR    3

#define HIGHSCORE_SHOW_MS     1000  // 게임 시작시 최고 기록을 보여주는 시간

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

// 화면 가운데에 글자 출력. (텍스트 크기 1 기준, 글자당 6px)
void centerText(const char *text, unsigned short yPos, uint16_t color) {
  tft.setCursor((tft.width() / 2) - (strlen(text) * 3), yPos);
  tft.setTextColor(color);
  tft.print(text);
}

// 게임 시작시 최고 기록을 잠깐 보여준다.
void showHighScore(const char *gameName, int addr) {
  unsigned int high;
  bool hasRecord = loadHighScore(addr, high);

  tft.fillScreen(ST7735_BLACK);
  tft.setTextSize(1);

  unsigned short yPos = (tft.height() / 2) - 20;
  centerText(gameName, yPos, ST7735_WHITE);
  centerText("HIGH SCORE", yPos + 20, ST7735_WHITE);

  if (hasRecord) {
    char buf[7];
    utoa(high, buf, 10);
    centerText(buf, yPos + 35, ST7735_YELLOW);
  } else {
    centerText("NO RECORD", yPos + 35, ST7735_YELLOW);
  }

  delay(HIGHSCORE_SHOW_MS);

  tft.fillScreen(ST7735_BLACK);
  tft.setTextSize(1);
  tft.setTextColor(ST7735_WHITE);
}
