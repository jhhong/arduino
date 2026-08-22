#ifndef invaders_h
#define invaders_h

// ---------------
// SPACE INVADERS (스페이스 인베이더)
// ---------------
// 조이스틱 X축을 기울인 만큼 함선이 그 위치로 간다. 버튼: 발사.
// 적이 바닥에 닿거나 목숨이 다 떨어지면 끝.
//
// 적은 매 프레임이 아니라 "한 걸음씩" 움직인다. (원작과 같은 방식)

#include "core.h"

namespace Invaders {

const int EEPROM_ADDR = 22;

const int16_t HUD_H = 10;

// 적 편대
const byte COLS         = 6;
const byte ROWS         = 4;
const int16_t ALIEN_W   = 12;
const int16_t ALIEN_H   = 8;
const int16_t GAP_X     = 16;
const int16_t GAP_Y     = 12;
const int16_t FLEET_W   = (COLS - 1) * GAP_X + ALIEN_W;   // 92
const int16_t STEP_X    = 4;
const int16_t STEP_DOWN = 6;

// 함선
const int16_t SHIP_W = 12;
const int16_t SHIP_H = 8;
const int16_t SHIP_Y = TFTH - 14;
// 한 프레임에 함선이 움직일 수 있는 최대 픽셀. (3px * 62fps = 초당 약 190px)
const int16_t SHIP_SPEED = 3;

// 총알
const int16_t SHOT_W     = 2;
const int16_t SHOT_H     = 5;
const int16_t SHOT_SPEED = 6;
const int16_t BOMB_SPEED = 2;
const byte MAX_BOMBS     = 3;

const byte START_LIVES = 3;
const unsigned int FRAME_MS = 16;

// 적이 여기까지 내려오면 끝.
const int16_t FLEET_LIMIT_Y = SHIP_Y - ((ROWS - 1) * GAP_Y + ALIEN_H);

// 줄마다 살아있는 적을 비트로. (오른쪽 비트가 0번 열)
uint8_t alive[ROWS];
byte aliveCount;

int16_t fleetX, fleetY;
int8_t fleetDir;
bool fleetFrame;              // 걸음마다 다리 모양이 바뀐다
unsigned long nextStep;

int16_t shipX, shipOldX;

struct Shot {
  int16_t x, y, oldY;
  bool active;
};

Shot bullet;                  // 내 총알은 한 번에 하나
Shot bombs[MAX_BOMBS];

unsigned int score;
byte lives;
byte wave;

// ---------------
// 좌표
// ---------------
int16_t alienX(byte col) {
  return fleetX + col * GAP_X;
}

int16_t alienY(byte row) {
  return fleetY + row * GAP_Y;
}

bool isAlive(byte row, byte col) {
  return alive[row] & (1 << col);
}

uint16_t alienColor(byte row) {
  switch (row) {
    case 0:  return COLOR_MAGENTA;
    case 1:  return COLOR_CYAN;
    case 2:  return COLOR_LIME;
    default: return COLOR_YELLOW;
  }
}

// ---------------
// 그리기
// ---------------
// 비트맵 대신 사각형 몇 개로 그린다. drawBitmap 은 픽셀 단위라 24마리를
// 매 걸음마다 그리기에는 너무 느리다.
void drawAlien(byte row, byte col) {
  int16_t x = alienX(col);
  int16_t y = alienY(row);
  uint16_t c = alienColor(row);

  tft.fillRect(x + 2, y,     ALIEN_W - 4, 2, c);          // 머리
  tft.fillRect(x,     y + 2, ALIEN_W,     3, c);          // 몸통
  tft.fillRect(x + 3, y + 2, 2, 2, COLOR_BLACK);          // 눈
  tft.fillRect(x + 7, y + 2, 2, 2, COLOR_BLACK);

  // 다리 (걸음마다 벌렸다 오므렸다)
  if (fleetFrame) {
    tft.fillRect(x,               y + 5, 3, 3, c);
    tft.fillRect(x + ALIEN_W - 3, y + 5, 3, 3, c);
  } else {
    tft.fillRect(x + 2,           y + 5, 3, 3, c);
    tft.fillRect(x + ALIEN_W - 5, y + 5, 3, 3, c);
  }
}

void eraseAlien(int16_t x, int16_t y) {
  tft.fillRect(x, y, ALIEN_W, ALIEN_H, COLOR_BLACK);
}

void drawFleet() {
  for (byte row = 0; row < ROWS; row++) {
    for (byte col = 0; col < COLS; col++) {
      if (isAlive(row, col)) {
        drawAlien(row, col);
      }
    }
  }
}

void drawShipFull() {
  tft.fillRect(shipX, SHIP_Y + 5, SHIP_W, 3, COLOR_WHITE);
  tft.fillRect(shipX + 3, SHIP_Y + 2, SHIP_W - 6, 3, COLOR_WHITE);
  tft.fillRect(shipX + 5, SHIP_Y, 2, 2, COLOR_CYAN);
  shipOldX = shipX;
}

void drawShip() {
  if (shipX == shipOldX) {
    return;
  }

  tft.fillRect(shipOldX, SHIP_Y, SHIP_W, SHIP_H, COLOR_BLACK);
  drawShipFull();
}

void drawHud() {
  tft.fillRect(0, 0, TFTW, HUD_H, COLOR_BLACK);

  tft.setTextSize(1);
  tft.setTextColor(COLOR_WHITE);
  tft.setCursor(2, 1);
  tft.print(score);

  for (byte i = 0; i < lives; i++) {
    tft.fillRect(TFTW - 8 - i * 10, 2, 6, 3, COLOR_WHITE);
  }
}

// ---------------
// 총알
// ---------------
void moveShot(Shot &s, int16_t dy, uint16_t color) {
  if (!s.active) {
    return;
  }

  s.oldY = s.y;
  s.y += dy;

  tft.fillRect(s.x, s.oldY, SHOT_W, SHOT_H, COLOR_BLACK);

  if (s.y < HUD_H || s.y > TFTH - SHOT_H) {
    s.active = false;
    return;
  }

  tft.fillRect(s.x, s.y, SHOT_W, SHOT_H, color);
}

void killShot(Shot &s) {
  if (!s.active) {
    return;
  }

  tft.fillRect(s.x, s.y, SHOT_W, SHOT_H, COLOR_BLACK);
  s.active = false;
}

// 적이 폭탄을 떨어뜨린다. 각 열에서 가장 아래에 살아있는 적이 쏜다.
void dropBomb() {
  byte slot = MAX_BOMBS;

  for (byte i = 0; i < MAX_BOMBS; i++) {
    if (!bombs[i].active) {
      slot = i;
      break;
    }
  }

  if (slot == MAX_BOMBS) {
    return;
  }

  byte col = random(COLS);

  for (int8_t row = ROWS - 1; row >= 0; row--) {
    if (!isAlive(row, col)) {
      continue;
    }

    bombs[slot].x = alienX(col) + ALIEN_W / 2 - 1;
    bombs[slot].y = alienY(row) + ALIEN_H;
    bombs[slot].oldY = bombs[slot].y;
    bombs[slot].active = true;
    return;
  }
}

// ---------------
// 편대 한 걸음
// ---------------
// 적이 바닥까지 내려왔으면 false.
bool stepFleet() {
  // 살아있는 적의 좌우 끝 열을 찾아서, 그 만큼만 움직일 수 있게 한다.
  byte leftCol = COLS, rightCol = 0;

  for (byte row = 0; row < ROWS; row++) {
    for (byte col = 0; col < COLS; col++) {
      if (!isAlive(row, col)) {
        continue;
      }

      if (col < leftCol) leftCol = col;
      if (col > rightCol) rightCol = col;
    }
  }

  if (leftCol == COLS) {
    return true;    // 다 잡았다
  }

  int16_t leftEdge = alienX(leftCol);
  int16_t rightEdge = alienX(rightCol) + ALIEN_W;

  bool drop = false;

  if (fleetDir > 0 && rightEdge + STEP_X > TFTW) {
    drop = true;
  } else if (fleetDir < 0 && leftEdge - STEP_X < 0) {
    drop = true;
  }

  // 옮기기 전 자리를 지운다.
  for (byte row = 0; row < ROWS; row++) {
    for (byte col = 0; col < COLS; col++) {
      if (isAlive(row, col)) {
        eraseAlien(alienX(col), alienY(row));
      }
    }
  }

  if (drop) {
    fleetDir = -fleetDir;
    fleetY += STEP_DOWN;
  } else {
    fleetX += fleetDir * STEP_X;
  }

  fleetFrame = !fleetFrame;
  drawFleet();

  beep(drop ? 110 : (fleetFrame ? 160 : 130), 20);

  return fleetY <= FLEET_LIMIT_Y;
}

// ---------------
// 판 준비
// ---------------
void resetFleet() {
  for (byte row = 0; row < ROWS; row++) {
    alive[row] = (1 << COLS) - 1;
  }

  aliveCount = ROWS * COLS;

  fleetX = (TFTW - FLEET_W) / 2;
  fleetY = HUD_H + 6 + (wave - 1) * 4;   // 판이 넘어갈수록 조금 더 아래에서 시작
  fleetDir = 1;
  fleetFrame = false;

  if (fleetY > FLEET_LIMIT_Y - 20) {
    fleetY = FLEET_LIMIT_Y - 20;
  }
}

// 남은 적이 적을수록 빨라진다.
unsigned int stepInterval() {
  unsigned int base = 60 + (unsigned int)aliveCount * 11;

  // 판이 넘어갈수록 전체적으로 빨라진다.
  if (wave > 1) {
    unsigned int cut = (wave - 1) * 20;
    base = (base > cut + 50) ? base - cut : 50;
  }

  return base;
}

void clearShots() {
  killShot(bullet);

  for (byte i = 0; i < MAX_BOMBS; i++) {
    killShot(bombs[i]);
  }
}

void resetField() {
  tft.fillScreen(COLOR_BLACK);
  drawHud();
  drawFleet();

  shipX = joyPosX(TFTW - SHIP_W);
  shipOldX = shipX;
  drawShipFull();

  bullet.active = false;

  for (byte i = 0; i < MAX_BOMBS; i++) {
    bombs[i].active = false;
  }

  nextStep = millis() + stepInterval();
}

// ---------------
// 내 총알이 적을 맞췄나
// ---------------
void checkBulletHit() {
  if (!bullet.active) {
    return;
  }

  for (byte row = 0; row < ROWS; row++) {
    if (alive[row] == 0) {
      continue;
    }

    int16_t ay = alienY(row);

    if (bullet.y + SHOT_H <= ay || bullet.y >= ay + ALIEN_H) {
      continue;
    }

    for (byte col = 0; col < COLS; col++) {
      if (!isAlive(row, col)) {
        continue;
      }

      int16_t ax = alienX(col);

      if (bullet.x + SHOT_W <= ax || bullet.x >= ax + ALIEN_W) {
        continue;
      }

      alive[row] &= ~(1 << col);
      aliveCount--;
      eraseAlien(ax, ay);
      killShot(bullet);

      // 위쪽 줄일수록 점수가 높다.
      score += (ROWS - row) * 10;
      drawHud();
      beep(1200 - row * 100, 30);
      return;
    }
  }
}

// ---------------
// 폭탄에 맞았나
// ---------------
bool checkBombHit() {
  for (byte i = 0; i < MAX_BOMBS; i++) {
    if (!bombs[i].active) {
      continue;
    }

    if (bombs[i].y + SHOT_H < SHIP_Y || bombs[i].y > SHIP_Y + SHIP_H) {
      continue;
    }

    if (bombs[i].x + SHOT_W <= shipX || bombs[i].x >= shipX + SHIP_W) {
      continue;
    }

    return true;
  }

  return false;
}

// 맞았을 때 잠깐 깜빡인다.
void shipHit() {
  beep(150, 400);

  for (byte i = 0; i < 4; i++) {
    tft.fillRect(shipX, SHIP_Y, SHIP_W, SHIP_H, COLOR_RED);
    delay(100);
    tft.fillRect(shipX, SHIP_Y, SHIP_W, SHIP_H, COLOR_BLACK);
    delay(100);
  }

  clearShots();
  drawShipFull();
}

// ---------------
// 판 하나. 다 잡으면 true, 죽으면 false.
// ---------------
bool playWave() {
  resetFleet();
  resetField();

  unsigned long nextFrame = millis();

  while (true) {
    while (millis() < nextFrame) {
    }
    nextFrame = millis() + FRAME_MS;

    // ===============
    // 입력
    // ===============
    shipX = joyFollowX(shipX, TFTW - SHIP_W, SHIP_SPEED);

    if (buttonDown() && !bullet.active) {
      bullet.x = shipX + SHIP_W / 2 - 1;
      bullet.y = SHIP_Y - SHOT_H;
      bullet.oldY = bullet.y;
      bullet.active = true;
      beep(1600, 25);
    }

    // ===============
    // 총알
    // ===============
    moveShot(bullet, -SHOT_SPEED, COLOR_WHITE);
    checkBulletHit();

    for (byte i = 0; i < MAX_BOMBS; i++) {
      moveShot(bombs[i], BOMB_SPEED, COLOR_RED);
    }

    // 판이 넘어갈수록 자주 쏜다.
    if (random(255) < (int)(3 + wave * 2)) {
      dropBomb();
    }

    // ===============
    // 편대
    // ===============
    if (millis() >= nextStep) {
      if (!stepFleet()) {
        return false;   // 바닥까지 내려왔다
      }

      nextStep = millis() + stepInterval();

      // 편대를 다시 그리면서 함선 자리를 건드렸을 수 있으니 다시 그린다.
      drawShipFull();
    }

    drawShip();

    // ===============
    // 판정
    // ===============
    if (aliveCount == 0) {
      return true;
    }

    if (checkBombHit()) {
      lives--;
      drawHud();
      shipHit();

      if (lives == 0) {
        return false;
      }
    }
  }
}

void playGame() {
  score = 0;
  lives = START_LIVES;
  wave = 1;

  while (playWave()) {
    // 판을 깼다.
    tft.fillScreen(COLOR_BLACK);
    centerText("WAVE", TFTH2 - 20, COLOR_WHITE, 2);
    centerNumber(wave + 1, TFTH2 + 4, COLOR_LIME, 3);

    beep(880, 100);
    delay(120);
    beep(1174, 150);
    delay(800);

    wave++;
  }
}

void run() {
  playGame();
  gameOverScreen(score, EEPROM_ADDR);
}

}  // namespace Invaders

#endif
