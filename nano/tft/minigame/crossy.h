#ifndef crossy_h
#define crossy_h

// ---------------
// CROSSY (길 건너기 - Frogger 류)
// ---------------
// 조이스틱 4방향으로 한 칸씩 뛴다. 차에 치이면 목숨이 하나 줄고 아래에서 다시 시작.
// 맨 위 초록 띠까지 올라가면 점수. 건널수록 차가 빨라진다.
//
// 차는 지나간 자리(꼬리)만 지우고 나아가는 자리(머리)만 칠한다.

#include "core.h"

namespace Crossy {

const int EEPROM_ADDR = 25;

// 판: 16px 칸으로 가로 8칸 * 세로 10줄.
const int16_t CELL = 16;
const byte COLS    = 8;
const byte ROWS    = 10;

const int8_t GOAL_ROW   = 0;          // 맨 위 = 도착 지점
const int8_t START_ROW  = ROWS - 1;   // 맨 아래 = 출발 지점
const byte LANE_COUNT   = ROWS - 2;   // 가운데 8줄이 찻길

// 칸 안에서 차/개구리가 차지하는 크기
const int16_t BODY     = 12;
const int16_t BODY_OFF = 2;

const byte MAX_CARS    = 3;
const byte START_LIVES = 3;
const unsigned int FRAME_MS = 33;     // 약 30fps

const unsigned long HOP_REPEAT_MS = 300;  // 조이스틱을 계속 기울이고 있을 때 연속 이동 간격

const unsigned int GOAL_SCORE = 10;
const uint16_t SAFE_COLOR = RGB565(20, 70, 30);
const uint16_t ROAD_COLOR = RGB565(24, 24, 24);

struct Lane {
  int16_t x[MAX_CARS];
  int16_t oldX[MAX_CARS];
  int8_t speed;         // px/프레임, 음수면 왼쪽으로
  uint8_t carW;
  uint8_t nCars;
  uint16_t color;
};

Lane lanes[LANE_COUNT];

int8_t playerCol, playerRow;
int8_t oldCol, oldRow;

unsigned int score;
byte lives;
byte level;
byte crossings;

byte lastDir;
unsigned long lastHop;

// ---------------
// 좌표
// ---------------
int16_t rowY(int8_t row) {
  return row * CELL + BODY_OFF;
}

int16_t colX(int8_t col) {
  return col * CELL + BODY_OFF;
}

// ---------------
// 찻길 만들기
// ---------------
uint16_t laneColor(byte i) {
  switch (i % 6) {
    case 0:  return COLOR_RED;
    case 1:  return COLOR_ORANGE;
    case 2:  return COLOR_CYAN;
    case 3:  return COLOR_PINK;
    case 4:  return COLOR_YELLOW;
    default: return COLOR_PURPLE;
  }
}

void setupLanes() {
  for (byte i = 0; i < LANE_COUNT; i++) {
    Lane &l = lanes[i];

    l.carW = 20 + random(3) * 4;              // 20, 24, 28
    l.nCars = (l.carW >= 28) ? 2 : (2 + random(2));
    l.color = laneColor(i + level);

    // 단계가 오를수록 빨라진다. (레벨 1: 1~2px, 레벨 5부터 최대 4px)
    int8_t mag = 1 + random(2 + (level > 4 ? 2 : level / 2));

    if (mag > 4) {
      mag = 4;
    }

    // 줄마다 방향을 번갈아 준다.
    l.speed = (i & 1) ? mag : -mag;

    // 차 사이를 고르게 벌려 놓는다. (span 을 한 바퀴로 보고 그 안에 nCars 대)
    int16_t span = TFTW + l.carW;

    for (byte c = 0; c < l.nCars; c++) {
      l.x[c] = (int16_t)((long)span * c / l.nCars) - l.carW + random(6);
      l.oldX[c] = l.x[c];
    }
  }
}

// ---------------
// 그리기
// ---------------
void drawSafeBands() {
  tft.fillRect(0, GOAL_ROW * CELL, TFTW, CELL, SAFE_COLOR);
  tft.fillRect(0, START_ROW * CELL, TFTW, CELL, SAFE_COLOR);
}

void drawHud() {
  // 도착 지점 띠 위에 점수와 목숨을 같이 얹는다.
  tft.fillRect(0, GOAL_ROW * CELL, TFTW, CELL, SAFE_COLOR);

  tft.setTextSize(1);
  tft.setTextColor(COLOR_WHITE);
  tft.setCursor(3, GOAL_ROW * CELL + 4);
  tft.print(score);

  for (byte i = 0; i < lives; i++) {
    tft.fillRect(TFTW - 8 - i * 9, GOAL_ROW * CELL + 5, 6, 6, COLOR_LIME);
  }
}

void drawCarFull(const Lane &l, byte c, int8_t row) {
  fillRectClipped(l.x[c], rowY(row), l.carW, BODY, l.color);
}

void drawAllCars() {
  for (byte i = 0; i < LANE_COUNT; i++) {
    for (byte c = 0; c < lanes[i].nCars; c++) {
      drawCarFull(lanes[i], c, i + 1);
    }
  }
}

void drawPlayer() {
  int16_t x = colX(playerCol);
  int16_t y = rowY(playerRow);

  tft.fillRect(x, y, BODY, BODY, COLOR_LIME);
  tft.fillRect(x + 2, y + 2, 3, 3, COLOR_BLACK);
  tft.fillRect(x + BODY - 5, y + 2, 3, 3, COLOR_BLACK);
  tft.fillRect(x + 3, y + 8, BODY - 6, 2, RGB565(10, 100, 20));
}

// 개구리가 떠난 자리를 원래 배경으로 되돌린다.
// 그 자리에 차가 걸쳐 있었다면 차의 그 부분만 다시 칠해준다.
void restoreCell(int8_t col, int8_t row) {
  int16_t x = colX(col);
  int16_t y = rowY(row);

  uint16_t bg = (row == GOAL_ROW || row == START_ROW) ? SAFE_COLOR : ROAD_COLOR;
  tft.fillRect(x, y, BODY, BODY, bg);

  if (row == GOAL_ROW || row == START_ROW) {
    return;
  }

  const Lane &l = lanes[row - 1];

  for (byte c = 0; c < l.nCars; c++) {
    int16_t left = l.x[c] > x ? l.x[c] : x;
    int16_t right = (l.x[c] + l.carW < x + BODY) ? (l.x[c] + l.carW) : (x + BODY);

    if (right > left) {
      fillRectClipped(left, y, right - left, BODY, l.color);
    }
  }
}

// ---------------
// 차 이동
// ---------------
void moveCars() {
  for (byte i = 0; i < LANE_COUNT; i++) {
    Lane &l = lanes[i];

    int16_t y = rowY(i + 1);
    int16_t span = TFTW + l.carW;
    int16_t dx = l.speed;
    int16_t adx = dx < 0 ? -dx : dx;

    for (byte c = 0; c < l.nCars; c++) {
      l.oldX[c] = l.x[c];
      l.x[c] += dx;

      // 화면 밖으로 완전히 나가면 반대편 끝(역시 화면 밖)으로 옮긴다.
      // 그 자리는 아직 아무것도 안 그려져 있으므로 머리부터 자연스럽게 들어온다.
      // 옮기기 전에, 화면 끝에 몇 픽셀 남아있을 수 있으니 통째로 한 번 지운다.
      if (l.x[c] >= TFTW || l.x[c] <= -(int16_t)l.carW) {
        fillRectClipped(l.oldX[c], y, l.carW, BODY, ROAD_COLOR);

        l.x[c] += (l.x[c] >= TFTW) ? -span : span;
        l.oldX[c] = l.x[c];
        continue;
      }

      if (dx > 0) {
        // 꼬리 지우고 머리 칠하기
        fillRectClipped(l.oldX[c], y, adx, BODY, ROAD_COLOR);
        fillRectClipped(l.x[c] + l.carW - adx, y, adx, BODY, l.color);
      } else {
        fillRectClipped(l.oldX[c] + l.carW - adx, y, adx, BODY, ROAD_COLOR);
        fillRectClipped(l.x[c], y, adx, BODY, l.color);
      }
    }
  }
}

// ---------------
// 충돌
// ---------------
bool hitByCar() {
  if (playerRow == GOAL_ROW || playerRow == START_ROW) {
    return false;
  }

  const Lane &l = lanes[playerRow - 1];
  int16_t px = colX(playerCol);

  for (byte c = 0; c < l.nCars; c++) {
    if (px + BODY <= l.x[c] || px >= l.x[c] + l.carW) {
      continue;
    }

    return true;
  }

  return false;
}

// ---------------
// 판 준비
// ---------------
void drawField() {
  tft.fillRect(0, CELL, TFTW, (ROWS - 2) * CELL, ROAD_COLOR);

  // 차선 표시
  for (byte i = 1; i < ROWS - 1; i++) {
    tft.drawFastHLine(0, i * CELL, TFTW, COLOR_GRAY);
  }

  drawSafeBands();
  drawHud();
  drawAllCars();
  drawPlayer();
}

void resetPlayer() {
  playerCol = COLS / 2;
  playerRow = START_ROW;
  oldCol = playerCol;
  oldRow = playerRow;
  lastDir = DIR_NONE;
  lastHop = millis();
}

void resetGame() {
  score = 0;
  lives = START_LIVES;
  level = 1;
  crossings = 0;

  setupLanes();
  resetPlayer();

  tft.fillScreen(COLOR_BLACK);
  drawField();
}

// 도착했을 때
void reachGoal() {
  score += GOAL_SCORE * level;
  crossings++;

  beep(1200, 60);
  delay(80);
  beep(1600, 120);

  // 세 번 건널 때마다 찻길이 새로 깔리고 빨라진다.
  if (crossings % 3 == 0) {
    level++;
    setupLanes();
  }

  resetPlayer();
  drawField();
}

// 치였을 때
void crash() {
  beep(160, 400);

  for (byte i = 0; i < 4; i++) {
    tft.fillRect(colX(playerCol), rowY(playerRow), BODY, BODY, COLOR_RED);
    delay(90);
    tft.fillRect(colX(playerCol), rowY(playerRow), BODY, BODY, COLOR_WHITE);
    delay(90);
  }

  lives--;
  resetPlayer();
  drawField();
}

// ---------------
// 입력 - 한 칸씩 뛴다
// ---------------
void handleInput() {
  byte d = joyDir();

  if (d == DIR_NONE) {
    lastDir = DIR_NONE;
    return;
  }

  // 방향을 새로 잡았을 때, 또는 계속 기울이고 있으면 일정 간격마다 한 칸.
  if (d == lastDir && millis() - lastHop < HOP_REPEAT_MS) {
    return;
  }

  lastDir = d;
  lastHop = millis();

  oldCol = playerCol;
  oldRow = playerRow;

  switch (d) {
    case DIR_UP:    if (playerRow > 0) playerRow--;          break;
    case DIR_DOWN:  if (playerRow < ROWS - 1) playerRow++;   break;
    case DIR_LEFT:  if (playerCol > 0) playerCol--;          break;
    default:        if (playerCol < COLS - 1) playerCol++;   break;
  }

  if (playerCol == oldCol && playerRow == oldRow) {
    return;
  }

  beep(500, 15);
  restoreCell(oldCol, oldRow);

  // 점수줄을 지웠을 수 있으니 도착 띠는 통째로 다시.
  if (oldRow == GOAL_ROW) {
    drawHud();
  }

  // 다음 프레임까지 기다리지 않고 바로 새 자리에 그린다.
  drawPlayer();
}

// ---------------
// 한 게임
// ---------------
void playGame() {
  resetGame();

  unsigned long nextFrame = millis();

  while (true) {
    while (millis() < nextFrame) {
      // 기다리는 동안에도 입력은 받는다.
      handleInput();
    }
    nextFrame = millis() + FRAME_MS;

    moveCars();
    drawPlayer();

    if (playerRow == GOAL_ROW) {
      reachGoal();
      nextFrame = millis() + FRAME_MS;
      continue;
    }

    if (hitByCar()) {
      crash();

      if (lives == 0) {
        return;
      }

      nextFrame = millis() + FRAME_MS;
    }
  }
}

void run() {
  playGame();
  gameOverScreen(score, EEPROM_ADDR);
}

}  // namespace Crossy

#endif
