#ifndef jumper_h
#define jumper_h

// ---------------
// JUMPER (수직 점프 - Doodle Jump 류)
// ---------------
// 조이스틱 X축을 기울인 만큼 좌우로 움직인다. 화면 양 끝은 반대편으로 이어진다.
// 발판에 닿으면 자동으로 튀어오르고, 닿는 순간 버튼을 누르고 있으면 크게 튄다.
//
// 좌표는 1/16 픽셀 고정소수점(int16)으로 다룬다. float 을 쓰지 않으려는 것.

#include "core.h"

namespace Jumper {

const int EEPROM_ADDR = 19;

// 점수줄을 화면 맨 아래 10px 에 두고, 그 위쪽만 게임 화면으로 쓴다.
const int16_t GAMEH = 150;
const int16_t HUD_Y = GAMEH + 2;

const int16_t FP = 16;      // 고정소수점 배율 (1px = 16)

// 플레이어
const int16_t PLW = 9;
const int16_t PLH = 9;

// 발판
const byte PLAT_COUNT     = 7;
const int16_t PLAT_W      = 26;
const int16_t PLAT_H      = 4;
const int16_t PLAT_GAP_MIN = 22;
const int16_t PLAT_GAP_MAX = 33;

// 물리 (1/16 px 단위)
const int16_t GRAVITY        = 2;     // 프레임당 속도 증가
const int16_t JUMP_VEL       = -51;   // 약 40px 높이까지 올라간다
const int16_t SUPER_JUMP_VEL = -66;   // 버튼을 누르고 있었을 때
const int16_t MAX_FALL       = 80;
const int16_t MOVE_MAX       = 56;    // 끝까지 기울였을 때 좌우 속도 (3.5px/프레임)

const int16_t SCROLL_LINE = 58;       // 이 선보다 위로 올라가면 화면이 따라 내려간다
const unsigned int FRAME_MS = 20;

// 움직이는 발판은 점수가 이만큼 넘어가면 나오기 시작한다.
const unsigned int MOVING_FROM = 250;

struct Platform {
  int16_t x, y;         // px
  int16_t oldX, oldY;
  int8_t vx;            // 0 이면 고정 발판
};

Platform plats[PLAT_COUNT];

int16_t playerX, playerY;       // 1/16 px, 왼쪽 위
int16_t playerVX, playerVY;
int16_t oldPX, oldPY;           // px, 지웠다 그리려고 보관

unsigned int score;
unsigned int shownScore;
uint16_t climbed;               // 스크롤한 픽셀 누적

// ---------------
// 그리기
// ---------------
uint16_t platColor(const Platform &p) {
  return p.vx ? COLOR_ORANGE : COLOR_LIME;
}

void drawPlatform(Platform &p) {
  if (p.x == p.oldX && p.y == p.oldY) {
    return;
  }

  fillRectClippedY(p.oldX, p.oldY, PLAT_W, PLAT_H, 0, GAMEH, COLOR_BLACK);
  fillRectClippedY(p.x, p.y, PLAT_W, PLAT_H, 0, GAMEH, platColor(p));

  p.oldX = p.x;
  p.oldY = p.y;
}

void drawPlayer() {
  int16_t px = playerX / FP;
  int16_t py = playerY / FP;

  if (px == oldPX && py == oldPY) {
    return;
  }

  fillRectClippedY(oldPX, oldPY, PLW, PLH, 0, GAMEH, COLOR_BLACK);

  // 몸통 + 눈 두 개
  fillRectClippedY(px, py, PLW, PLH, 0, GAMEH, COLOR_CYAN);
  fillRectClippedY(px + 2, py + 2, 2, 2, 0, GAMEH, COLOR_BLACK);
  fillRectClippedY(px + PLW - 4, py + 2, 2, 2, 0, GAMEH, COLOR_BLACK);

  oldPX = px;
  oldPY = py;
}

void drawScore() {
  if (score == shownScore) {
    return;
  }

  tft.fillRect(0, HUD_Y, TFTW, TFTH - HUD_Y, COLOR_BLACK);

  tft.setTextSize(1);
  tft.setTextColor(COLOR_DARKGRAY);
  tft.setCursor(2, HUD_Y);
  tft.print("HEIGHT");

  tft.setTextColor(COLOR_WHITE);
  tft.setCursor(50, HUD_Y);
  tft.print(score);

  shownScore = score;
}

// ---------------
// 발판 배치
// ---------------
// 가장 높은(=y 가 가장 작은) 발판을 찾는다.
int16_t topPlatformY() {
  int16_t top = plats[0].y;

  for (byte i = 1; i < PLAT_COUNT; i++) {
    if (plats[i].y < top) {
      top = plats[i].y;
    }
  }

  return top;
}

// 화면 위쪽으로 발판을 하나 새로 올린다.
void recyclePlatform(Platform &p) {
  p.y = topPlatformY() - (int16_t)random(PLAT_GAP_MIN, PLAT_GAP_MAX + 1);
  p.x = random(0, TFTW - PLAT_W + 1);

  // 어느 정도 올라가면 좌우로 움직이는 발판이 섞여 나온다.
  if (score > MOVING_FROM && random(4) == 0) {
    p.vx = random(2) ? 1 : -1;
  } else {
    p.vx = 0;
  }

  // 새 발판은 화면 밖에서 시작하므로 지울 자리가 없다.
  p.oldX = p.x;
  p.oldY = p.y;
}

void resetGame() {
  score = 0;
  shownScore = 0xFFFF;
  climbed = 0;

  tft.fillScreen(COLOR_BLACK);
  tft.drawFastHLine(0, GAMEH, TFTW, COLOR_GRAY);

  // 맨 아래 발판은 플레이어 바로 밑에 고정으로 깔아준다.
  plats[0].x = TFTW2 - PLAT_W / 2;
  plats[0].y = GAMEH - 12;
  plats[0].vx = 0;
  plats[0].oldX = plats[0].x;
  plats[0].oldY = plats[0].y;

  for (byte i = 1; i < PLAT_COUNT; i++) {
    plats[i].y = plats[i - 1].y - (int16_t)random(PLAT_GAP_MIN, PLAT_GAP_MAX + 1);
    plats[i].x = random(0, TFTW - PLAT_W + 1);
    plats[i].vx = 0;
    plats[i].oldX = plats[i].x;
    plats[i].oldY = plats[i].y;
  }

  for (byte i = 0; i < PLAT_COUNT; i++) {
    fillRectClippedY(plats[i].x, plats[i].y, PLAT_W, PLAT_H, 0, GAMEH, platColor(plats[i]));
  }

  playerX = (int16_t)(TFTW2 - PLW / 2) * FP;
  playerY = (int16_t)(plats[0].y - PLH) * FP;
  playerVX = 0;
  playerVY = JUMP_VEL;

  oldPX = playerX / FP;
  oldPY = playerY / FP;

  fillRectClippedY(oldPX, oldPY, PLW, PLH, 0, GAMEH, COLOR_CYAN);
  drawScore();
}

// ---------------
// 발판 충돌
// ---------------
// 떨어지는 중에 발판 윗면을 지나쳤으면 튀어오른다.
void checkLanding(int16_t prevFeet) {
  if (playerVY <= 0) {
    return;
  }

  int16_t px = playerX / FP;
  int16_t feet = playerY / FP + PLH;

  for (byte i = 0; i < PLAT_COUNT; i++) {
    Platform &p = plats[i];

    // 이번 프레임에 발판 윗면을 통과했는지. (빨리 떨어져도 뚫고 지나가지 않게)
    if (prevFeet > p.y || feet < p.y) {
      continue;
    }

    if (px + PLW <= p.x || px >= p.x + PLAT_W) {
      continue;
    }

    playerY = (int16_t)(p.y - PLH) * FP;

    // 닿는 순간 버튼을 누르고 있으면 크게 튄다.
    if (buttonDown()) {
      playerVY = SUPER_JUMP_VEL;
      beep(1400, 60);
    } else {
      playerVY = JUMP_VEL;
      beep(900, 30);
    }

    return;
  }
}

// ---------------
// 한 게임
// ---------------
void playGame() {
  resetGame();

  unsigned long nextFrame = millis();

  while (true) {
    while (millis() < nextFrame) {
    }
    nextFrame = millis() + FRAME_MS;

    // ===============
    // 입력 - 기울인 만큼 좌우 속도가 붙는다
    // ===============
    short leftward = joyLeftward();

    if (leftward > -JOY_DEADZONE && leftward < JOY_DEADZONE) {
      playerVX = 0;
    } else {
      if (leftward > JOY_FULL) leftward = JOY_FULL;
      if (leftward < -JOY_FULL) leftward = -JOY_FULL;

      playerVX = -(int32_t)leftward * MOVE_MAX / JOY_FULL;
    }

    // ===============
    // 이동
    // ===============
    int16_t prevFeet = playerY / FP + PLH;

    playerX += playerVX;
    playerVY += GRAVITY;

    if (playerVY > MAX_FALL) {
      playerVY = MAX_FALL;
    }

    playerY += playerVY;

    // 좌우는 반대편으로 이어진다.
    if (playerX < -PLW * FP) {
      playerX = TFTW * FP;
    } else if (playerX > TFTW * FP) {
      playerX = -PLW * FP;
    }

    // 움직이는 발판
    for (byte i = 0; i < PLAT_COUNT; i++) {
      if (!plats[i].vx) {
        continue;
      }

      plats[i].x += plats[i].vx;

      if (plats[i].x <= 0 || plats[i].x >= TFTW - PLAT_W) {
        plats[i].vx = -plats[i].vx;
      }
    }

    checkLanding(prevFeet);

    // ===============
    // 스크롤 - 플레이어가 기준선 위로 올라가면 세상이 내려온다
    // ===============
    int16_t py = playerY / FP;

    if (py < SCROLL_LINE) {
      int16_t dy = SCROLL_LINE - py;

      // oldY 는 "화면에 실제로 그려져 있는 위치"라서 같이 내리면 안 된다.
      // 스크롤한다고 이미 찍힌 픽셀이 따라 움직이는게 아니기 때문.
      playerY += dy * FP;

      for (byte i = 0; i < PLAT_COUNT; i++) {
        plats[i].y += dy;
      }

      // 올라간 높이가 곧 점수. (5px 당 1점)
      climbed += dy;

      while (climbed >= 5) {
        climbed -= 5;
        score++;
      }
    }

    // 화면 아래로 내려간 발판은 위로 다시 올린다.
    for (byte i = 0; i < PLAT_COUNT; i++) {
      if (plats[i].y >= GAMEH) {
        // 내려가면서 남긴 자국을 먼저 지운다.
        fillRectClippedY(plats[i].oldX, plats[i].oldY, PLAT_W, PLAT_H, 0, GAMEH, COLOR_BLACK);
        recyclePlatform(plats[i]);
      }
    }

    // ===============
    // 그리기
    // ===============
    for (byte i = 0; i < PLAT_COUNT; i++) {
      drawPlatform(plats[i]);
    }

    drawPlayer();
    drawScore();

    // 바닥으로 떨어졌다.
    if (playerY / FP > GAMEH) {
      beep(250, 400);
      return;
    }
  }
}

void run() {
  playGame();
  gameOverScreen(score, EEPROM_ADDR);
}

}  // namespace Jumper

#endif
