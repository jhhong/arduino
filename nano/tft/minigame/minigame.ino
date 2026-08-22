// ---------------
// MINI GAMES - 7개 게임 모음
// ---------------
// 조이스틱 위/아래로 게임을 고르고 버튼으로 시작. 게임이 끝나면 다시 이 화면으로
// 돌아오고, 마지막에 한 게임이 기본으로 선택되어 있다. (전원을 껐다 켜도 유지)
//
// 게임마다 이름 충돌을 피하려고 namespace 로 감쌌다.
// 테트리스와 플래피는 tetrisflappy 스케치에서 옮겨온 것이다.

#include "core.h"
#include "breakout.h"
#include "snake.h"
#include "game2048.h"
#include "jumper.h"
#include "invaders.h"
#include "tetris.h"
#include "flappy.h"

#define GAME_COUNT        7

// 마지막에 고른 게임을 기억해 두는 자리.
// 테트리스가 0~2, 플래피가 3~5, 나머지 게임이 10번지부터 3바이트씩 쓰고 있어서
// 그 사이의 빈 곳을 쓴다.
#define EEPROM_LAST_GAME  9

// 화면 배치
#define LIST_Y            34    // 첫 항목의 y
#define LIST_STEP         13    // 항목 간격
#define HINT_Y            129   // 조작법 첫 줄
#define BEST_Y            150

#define MENU_REPEAT_MS    300   // 조이스틱을 계속 기울이고 있을 때 이동 간격

// 글자를 RAM 이 아니라 플래시에 둔다. 7개 게임의 이름과 조작법을 다 합치면
// 300바이트에 가까운데, 그만큼을 RAM 에서 빼앗기지 않으려는 것.
const char name0[] PROGMEM = "BREAKOUT";
const char name1[] PROGMEM = "SNAKE";
const char name2[] PROGMEM = "2048";
const char name3[] PROGMEM = "JUMPER";
const char name4[] PROGMEM = "INVADERS";
const char name5[] PROGMEM = "TETRIS";
const char name6[] PROGMEM = "FLAPPY BIRD";

const char hintA0[] PROGMEM = "JOYSTICK = PADDLE";
const char hintA1[] PROGMEM = "JOYSTICK = TURN";
const char hintA2[] PROGMEM = "JOYSTICK = SLIDE";
const char hintA3[] PROGMEM = "JOYSTICK = MOVE";
const char hintA4[] PROGMEM = "JOYSTICK = MOVE";
const char hintA5[] PROGMEM = "DOWN = HARD DROP";
const char hintA6[] PROGMEM = "AVOID THE PIPES";

const char hintB0[] PROGMEM = "BUTTON = LAUNCH";
const char hintB1[] PROGMEM = "HOLD BTN = FAST";
const char hintB2[] PROGMEM = "HOLD BTN = RESET";
const char hintB3[] PROGMEM = "BUTTON = HIGH JUMP";
const char hintB4[] PROGMEM = "BUTTON = FIRE";
const char hintB5[] PROGMEM = "BUTTON = ROTATE";
const char hintB6[] PROGMEM = "BUTTON = FLAP";

const char *const gameNames[] PROGMEM = {name0, name1, name2, name3, name4, name5, name6};
const char *const gameHintA[] PROGMEM = {hintA0, hintA1, hintA2, hintA3, hintA4, hintA5, hintA6};
const char *const gameHintB[] PROGMEM = {hintB0, hintB1, hintB2, hintB3, hintB4, hintB5, hintB6};

byte selected;

// 플래시에 있는 글자를 잠깐 꺼내 쓰는 버퍼. (가장 긴 글자가 18자)
char textBuf[20];

const char *flashText(const char *const *table, byte index) {
  strcpy_P(textBuf, (PGM_P)pgm_read_word(&table[index]));
  return textBuf;
}

// 게임마다 최고 기록을 저장하는 EEPROM 번지.
int gameAddr(byte index) {
  switch (index) {
    case 0:  return Breakout::EEPROM_ADDR;
    case 1:  return Snake::EEPROM_ADDR;
    case 2:  return Game2048::EEPROM_ADDR;
    case 3:  return Jumper::EEPROM_ADDR;
    case 4:  return Invaders::EEPROM_ADDR;
    case 5:  return Tetris::EEPROM_ADDR;
    default: return Flappy::EEPROM_ADDR;
  }
}

// 고른 게임을 실행한다. 게임 오버 화면까지 각 게임이 알아서 처리하고 돌아온다.
void runGame(byte index) {
  switch (index) {
    case 0:  Breakout::run();  break;
    case 1:  Snake::run();     break;
    case 2:  Game2048::run();  break;
    case 3:  Jumper::run();    break;
    case 4:  Invaders::run();  break;
    case 5:  Tetris::run();    break;
    default: Flappy::run();    break;
  }
}

// ---------------
// 메뉴 그리기
// ---------------
void drawItem(byte index) {
  bool on = (index == selected);
  int16_t y = LIST_Y + index * LIST_STEP;

  tft.fillRect(0, y - 2, TFTW, LIST_STEP, COLOR_BLACK);

  tft.setTextSize(1);

  if (on) {
    tft.setTextColor(COLOR_YELLOW);
    tft.setCursor(10, y);
    tft.print('>');
  }

  tft.setTextColor(on ? COLOR_YELLOW : COLOR_DARKGRAY);
  tft.setCursor(24, y);
  tft.print(flashText(gameNames, index));
}

// 고른 게임의 조작법과 최고 기록.
void drawHint() {
  tft.fillRect(0, HINT_Y - 2, TFTW, TFTH - (HINT_Y - 2), COLOR_BLACK);

  centerText(flashText(gameHintA, selected), HINT_Y, COLOR_WHITE);
  centerText(flashText(gameHintB, selected), HINT_Y + 10, COLOR_WHITE);

  unsigned int high;

  if (loadHighScore(gameAddr(selected), high)) {
    tft.setTextSize(1);
    tft.setTextColor(COLOR_LIME);
    tft.setCursor(TFTW2 - 27, BEST_Y);
    tft.print("BEST ");
    tft.print(high);
  }
}

void drawMenu() {
  tft.fillScreen(COLOR_BLACK);

  centerText("MINI GAMES", 8, COLOR_CYAN, 2);
  tft.drawFastHLine(6, 28, TFTW - 12, COLOR_DARKGRAY);

  for (byte i = 0; i < GAME_COUNT; i++) {
    drawItem(i);
  }

  tft.drawFastHLine(6, 124, TFTW - 12, COLOR_DARKGRAY);
  drawHint();
}

// ---------------
// 메뉴 조작
// ---------------
// 버튼을 누를 때까지 돌다가, 고른 게임 번호를 남기고 끝난다.
void menuLoop() {
  drawMenu();

  // 직전 화면(게임 오버 등)에서 누른 버튼이 그대로 넘어오지 않게 대기.
  waitForRelease();

  unsigned long lastMove = millis();
  unsigned long neutralSince = millis();

  while (true) {
    if (buttonDown()) {
      beep(1000, 60);
      waitForRelease();
      return;
    }

    byte d = joyDir();

    if (d != DIR_UP && d != DIR_DOWN) {
      if (neutralSince == 0) {
        neutralSince = millis();
      }

      continue;
    }

    // 중립이 JOY_NEUTRAL_MS 이상 이어진 뒤에 기울인 것만 "새로 기울였다"로 본다.
    // 문턱 근처로 살짝 기울이면 방향과 DIR_NONE 이 번갈아 읽히는데, 그걸
    // 새 입력으로 인정하면 한 번 기울인 게 목록 끝까지 주르륵 넘어간다.
    bool fresh = (neutralSince != 0) && (millis() - neutralSince >= JOY_NEUTRAL_MS);
    neutralSince = 0;

    // 새로 기울인게 아니면(계속 기울이고 있으면) 일정 간격마다 한 칸.
    if (!fresh && millis() - lastMove < MENU_REPEAT_MS) {
      continue;
    }

    lastMove = millis();

    byte prev = selected;

    if (d == DIR_UP) {
      selected = (selected == 0) ? GAME_COUNT - 1 : selected - 1;
    } else {
      selected = (selected + 1) % GAME_COUNT;
    }

    beep(700, 15);

    // 바뀐 두 줄과 아래쪽 안내만 다시 그린다.
    drawItem(prev);
    drawItem(selected);
    drawHint();
  }
}

void setup() {
  coreBegin();

  // 마지막에 했던 게임을 기본 선택으로. (처음 켜면 0번)
  selected = EEPROM.read(EEPROM_LAST_GAME);

  if (selected >= GAME_COUNT) {
    selected = 0;
  }
}

void loop() {
  menuLoop();

  // 고른 게임을 기억해 둔다. 값이 그대로면 쓰지 않는다. (EEPROM 수명 아끼기)
  if (EEPROM.read(EEPROM_LAST_GAME) != selected) {
    EEPROM.write(EEPROM_LAST_GAME, selected);
  }

  runGame(selected);
}
