#ifndef snake_h
#define snake_h

// ---------------
// SNAKE (스네이크)
// ---------------
// 조이스틱 4방향으로 방향 전환. 버튼을 누르고 있으면 2배 속도.
// 벽이나 자기 몸에 부딪히면 끝.
//
// 한 칸 움직일 때 다시 그리는건 머리 한 칸 + 꼬리 한 칸뿐이다.

#include "core.h"

namespace Snake {

const int EEPROM_ADDR = 13;

// 판 크기. 가로 16칸 * 세로 16칸이면 칸 번호가 0~255 라서 1바이트에 들어간다.
const int16_t CELL    = 8;
const byte COLS       = 16;
const byte ROWS       = 16;
const int16_t FIELD_Y = TFTH - ROWS * CELL;   // 32

// 몸 길이 상한. (칸이 256개지만 이 정도면 충분하고 RAM 도 아낀다)
const uint16_t MAX_LEN = 180;

const uint16_t START_LEN     = 4;
const unsigned int START_STEP_MS = 180;
const unsigned int MIN_STEP_MS   = 70;
const unsigned int STEP_DEC_MS   = 6;   // 먹이 하나당 빨라지는 정도

const unsigned int FOOD_SCORE = 10;

// 몸통 칸 번호를 순환 버퍼에 담는다. body[headPos] 가 머리.
uint8_t body[MAX_LEN];
uint16_t headPos;
uint16_t snakeLen;

// 어느 칸이 몸에 물려있는지 비트로 표시. 자기 몸 충돌을 한 번에 판정하려고 쓴다.
uint8_t occupied[COLS * ROWS / 8];

uint8_t foodCell;
byte dir, nextDir;
unsigned int score;
unsigned int stepMs;

// ---------------
// 칸 번호 <-> 좌표
// ---------------
uint8_t cellOf(byte col, byte row) {
  return row * COLS + col;
}

int16_t cellX(uint8_t cell) {
  return (cell % COLS) * CELL;
}

int16_t cellY(uint8_t cell) {
  return FIELD_Y + (cell / COLS) * CELL;
}

bool isOccupied(uint8_t cell) {
  return occupied[cell >> 3] & (1 << (cell & 7));
}

void setOccupied(uint8_t cell) {
  occupied[cell >> 3] |= (1 << (cell & 7));
}

void clearOccupied(uint8_t cell) {
  occupied[cell >> 3] &= ~(1 << (cell & 7));
}

// ---------------
// 그리기
// ---------------
// 칸 사이가 붙어보이지 않게 1px 씩 띄운다.
void drawSegment(uint8_t cell, uint16_t color) {
  tft.fillRect(cellX(cell) + 1, cellY(cell) + 1, CELL - 2, CELL - 2, color);
}

void eraseCell(uint8_t cell) {
  tft.fillRect(cellX(cell), cellY(cell), CELL, CELL, COLOR_BLACK);
}

void drawFood() {
  tft.fillCircle(cellX(foodCell) + CELL / 2, cellY(foodCell) + CELL / 2, 3, COLOR_RED);
}

void drawScore() {
  tft.fillRect(0, 0, TFTW, FIELD_Y - 2, COLOR_BLACK);

  tft.setTextSize(1);
  tft.setTextColor(COLOR_DARKGRAY);
  tft.setCursor(2, 4);
  tft.print("SNAKE");

  tft.setTextSize(2);
  tft.setTextColor(COLOR_WHITE);
  tft.setCursor(TFTW - 60, 12);
  tft.print(score);
}

// ---------------
// 먹이를 빈 칸에 놓는다
// ---------------
void placeFood() {
  // 대부분 몇 번 안에 빈 칸이 나온다.
  for (byte tries = 0; tries < 60; tries++) {
    uint8_t cell = random(COLS * ROWS);

    if (!isOccupied(cell)) {
      foodCell = cell;
      drawFood();
      return;
    }
  }

  // 몸이 아주 길어져서 운이 나쁜 경우, 처음부터 훑어서 빈 칸을 찾는다.
  for (uint16_t cell = 0; cell < COLS * ROWS; cell++) {
    if (!isOccupied(cell)) {
      foodCell = cell;
      drawFood();
      return;
    }
  }
}

// ---------------
// 게임 준비
// ---------------
void resetGame() {
  score = 0;
  stepMs = START_STEP_MS;
  dir = nextDir = DIR_RIGHT;

  memset(occupied, 0, sizeof(occupied));

  tft.fillScreen(COLOR_BLACK);
  tft.drawFastHLine(0, FIELD_Y - 1, TFTW, COLOR_DARKGRAY);
  drawScore();

  // 가운데 왼쪽에서 오른쪽을 보고 시작.
  snakeLen = START_LEN;
  headPos = 0;

  byte row = ROWS / 2;

  for (uint16_t i = 0; i < snakeLen; i++) {
    // body[0] 이 꼬리, body[snakeLen-1] 이 머리가 되도록 채운다.
    uint8_t cell = cellOf(2 + i, row);
    body[i] = cell;
    setOccupied(cell);
    drawSegment(cell, (i == snakeLen - 1) ? COLOR_LIME : COLOR_GREEN);
  }

  headPos = snakeLen - 1;

  placeFood();
}

// ---------------
// 한 칸 전진. 죽으면 false.
// ---------------
bool step() {
  uint8_t head = body[headPos];
  byte col = head % COLS;
  byte row = head / COLS;

  // 벽에 부딪히면 끝.
  switch (dir) {
    case DIR_UP:
      if (row == 0) return false;
      row--;
      break;
    case DIR_DOWN:
      if (row == ROWS - 1) return false;
      row++;
      break;
    case DIR_LEFT:
      if (col == 0) return false;
      col--;
      break;
    default:
      if (col == COLS - 1) return false;
      col++;
      break;
  }

  uint8_t next = cellOf(col, row);
  bool grow = (next == foodCell) && (snakeLen < MAX_LEN);

  // 자라지 않으면 꼬리를 먼저 뺀다.
  // 이렇게 해야 "지금 꼬리가 있던 칸"으로 들어가는게 충돌로 잡히지 않는다.
  if (!grow) {
    uint16_t tailPos = (headPos + MAX_LEN - (snakeLen - 1)) % MAX_LEN;
    uint8_t tail = body[tailPos];
    clearOccupied(tail);
    eraseCell(tail);
  }

  // 자기 몸에 부딪히면 끝.
  if (isOccupied(next)) {
    return false;
  }

  // 직전 머리는 이제 몸통 색으로.
  drawSegment(head, COLOR_GREEN);

  if (grow) {
    snakeLen++;
  }

  headPos = (headPos + 1) % MAX_LEN;
  body[headPos] = next;
  setOccupied(next);
  drawSegment(next, COLOR_LIME);

  // 먹이를 먹었다.
  if (next == foodCell) {
    score += FOOD_SCORE;
    drawScore();
    beep(1000, 40);

    if (stepMs > MIN_STEP_MS + STEP_DEC_MS) {
      stepMs -= STEP_DEC_MS;
    }

    placeFood();
  }

  return true;
}

// ---------------
// 입력을 받으면서 다음 스텝까지 기다린다
// ---------------
void waitStep() {
  unsigned long start = millis();

  while (true) {
    // 버튼을 누르고 있으면 2배 속도. 누르는 즉시 반영되도록 매번 다시 본다.
    unsigned int wait = buttonDown() ? stepMs / 2 : stepMs;

    if (millis() - start >= wait) {
      break;
    }

    byte d = joyDir();

    if (d == DIR_NONE) {
      continue;
    }

    // 가던 방향의 정반대로는 못 꺾는다. (바로 자기 목을 무는걸 방지)
    if ((d == DIR_UP && dir == DIR_DOWN) ||
        (d == DIR_DOWN && dir == DIR_UP) ||
        (d == DIR_LEFT && dir == DIR_RIGHT) ||
        (d == DIR_RIGHT && dir == DIR_LEFT)) {
      continue;
    }

    nextDir = d;
  }

  dir = nextDir;
}

void playGame() {
  resetGame();

  while (true) {
    waitStep();

    if (!step()) {
      beep(200, 400);
      return;
    }
  }
}

void run() {
  playGame();
  gameOverScreen(score, EEPROM_ADDR);
}

}  // namespace Snake

#endif
