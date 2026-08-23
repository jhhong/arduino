#ifndef flappy_h
#define flappy_h

// ---------------
// FLAPPY BIRD
// ---------------
// 버튼을 누르면 위로 튄다. 관을 피해 계속 날면 된다.
//
// tetrisflappy/flappy.h 를 옮겨온 것이고, 합본에 넣으면서 다음을 바꿨다.
//  - 이름 충돌을 피하려고 namespace 로 감쌌다.
//  - double / float 을 정수 연산으로 바꿨다. AVR 의 부동소수점 지원 코드만
//    플래시 1.8KB 를 먹는데, 8개 게임을 32KB 에 넣으려면 그 자리가 없었다.
//    아래 velocity 주석대로 원본과 같은 결과가 나오게 맞췄다.
//  - 새 그림 배열(birdcol)과 색 상수를 PROGMEM / 컴파일타임 상수로 옮겼다.
//    (RAM 140바이트 회수)
//  - drawPixel 매크로를 inline 함수로 바꿨다. 매크로는 네임스페이스를 무시해서
//    다른 게임의 tft.drawPixel 호출까지 망가뜨린다.

#include "core.h"

namespace Flappy {

// tetrisflappy 가 쓰던 자리를 그대로 쓴다. 기존 최고 기록이 이어진다.
const int EEPROM_ADDR = 3;

const int16_t SPEED     = 1;
const int16_t JUMP_VEL  = -100;   // 1/100 px per tick
const int16_t GRAVITY   = 26;     // 1/100 px per tick, 매 tick 더해진다
const unsigned int SKIP_TICKS = 20;   // 1000 / 50fps
const byte MAX_FRAMESKIP = 5;

// 새 크기
const int16_t BIRDW  = 8;
const int16_t BIRDH  = 8;
const int16_t BIRDW2 = 4;
const int16_t BIRDH2 = 4;

// 관
const int16_t PIPEW     = 12;
const int16_t GAPHEIGHT = 36;

// 바닥
const int16_t FLOORH = 20;
const int16_t GRASSH = 4;
const int16_t GAMEH  = TFTH - FLOORH;   // 140

// 색 (컴파일타임 상수라 RAM 을 쓰지 않는다)
const uint16_t BCKGRDCOL    = RGB565(138, 235, 244);
const uint16_t BIRDCOL      = RGB565(255, 254, 174);
const uint16_t PIPECOL      = RGB565(99, 255, 78);
const uint16_t PIPEHIGHCOL  = RGB565(250, 255, 250);
const uint16_t PIPESEAMCOL  = RGB565(0, 0, 0);
const uint16_t FLOORCOL     = RGB565(246, 240, 163);
const uint16_t GRASSCOL     = RGB565(141, 225, 87);
const uint16_t GRASSCOL2    = RGB565(156, 239, 88);

// 점수 색. 원래는 흰색이었는데 배경이 밝은 하늘색이라 밝기 차이가 거의 없어
// 글자가 묻혔다. 짙은 남색이면 하늘색 위에서도, 관(밝은 초록) 위에서도 잘 보인다.
const uint16_t SCORECOL     = RGB565(12, 36, 78);

// 새 그림 (8x8). 원본은 RAM 배열이라 128바이트를 잡아먹었다.
#define C0 BCKGRDCOL
#define C1 RGB565(195, 165, 75)
#define C2 BIRDCOL
#define C3 RGB565(255, 255, 255)
#define C4 RGB565(255, 0, 0)
#define C5 RGB565(251, 216, 114)

const uint16_t birdcol[] PROGMEM = {
  C0, C0, C1, C1, C1, C1, C1, C0,
  C0, C1, C2, C2, C2, C1, C3, C1,
  C0, C2, C2, C2, C2, C1, C3, C1,
  C1, C1, C1, C2, C2, C3, C1, C1,
  C1, C2, C2, C2, C2, C2, C4, C4,
  C1, C2, C2, C2, C1, C5, C4, C0,
  C0, C1, C2, C1, C5, C5, C5, C0,
  C0, C0, C1, C5, C5, C5, C0, C0
};

#undef C0
#undef C1
#undef C2
#undef C3
#undef C4
#undef C5

// 새
int16_t birdX, birdY, birdOldY;
int16_t birdVel;              // 1/100 px per tick

// 관
int16_t pipeX, pipeGapY;

// 잔디 줄무늬
int16_t grassX;

unsigned int score;

// setAddrWindow + pushColor 로 픽셀 하나를 빠르게 찍는다.
// (tft.drawPixel 은 매번 경계 검사를 다시 하므로 새를 그릴 때는 이쪽이 빠르다)
inline void fastPixel(int16_t x, int16_t y, uint16_t color) {
  tft.setAddrWindow(x, y, x, y);
  tft.pushColor(color);
}

// 점수를 화면 위쪽 가운데에 그린다.
// 글자 크기 2 로 키웠기 때문에 자릿수가 늘면 폭도 늘어난다. 지울 때도 같은
// 자리를 짚어야 해서, 그릴 때와 지울 때 모두 이 함수를 쓴다. (색만 다르게)
void drawScore(unsigned int value, uint16_t color) {
  char buf[6];
  utoa(value, buf, 10);

  tft.setTextSize(2);
  tft.setTextColor(color);
  tft.setCursor(TFTW2 - (int16_t)strlen(buf) * 6, 4);
  tft.print(buf);
}

// ---------------
// 관 - 한 걸음(1px)치 그리기
// ---------------
// 관은 화면 전체를 다시 그리지 않는다. 걸음마다 "새로 드러난 앞쪽 두 칸"만
// 칠하고 "뒤로 빠진 한 칸"만 지운다. 12px 폭의 관은 그 흔적이 쌓여서 보이는
// 것이다. (앞 3칸은 밝은 테두리색, 나머지는 관 색으로 덮인다)
//
// 그래서 이 함수는 반드시 1px 이동마다 정확히 한 번씩 불려야 한다.
// 한 번이라도 건너뛰면 그 칸은 지워지지 않은 채 화면에 그대로 남는다.
void drawPipeStep() {
  if (pipeX >= 0 && pipeX < TFTW) {
    tft.drawFastVLine(pipeX + 3, 0, pipeGapY, PIPECOL);
    tft.drawFastVLine(pipeX + 3, pipeGapY + GAPHEIGHT + 1,
                      GAMEH - (pipeGapY + GAPHEIGHT + 1), PIPECOL);
    tft.drawFastVLine(pipeX, 0, pipeGapY, PIPEHIGHCOL);
    tft.drawFastVLine(pipeX, pipeGapY + GAPHEIGHT + 1,
                      GAMEH - (pipeGapY + GAPHEIGHT + 1), PIPEHIGHCOL);

    fastPixel(pipeX, pipeGapY, PIPESEAMCOL);
    fastPixel(pipeX, pipeGapY + GAPHEIGHT, PIPESEAMCOL);
    fastPixel(pipeX, pipeGapY - 6, PIPESEAMCOL);
    fastPixel(pipeX, pipeGapY + GAPHEIGHT + 6, PIPESEAMCOL);
    fastPixel(pipeX + 3, pipeGapY - 6, PIPESEAMCOL);
    fastPixel(pipeX + 3, pipeGapY + GAPHEIGHT + 6, PIPESEAMCOL);
  }

  // 뒤로 빠진 칸 지우기
  tft.drawFastVLine(pipeX + PIPEW, 0, GAMEH, BCKGRDCOL);
}

// 잔디 줄무늬도 같은 방식(걸음마다 한 칸씩)이라 함께 걸음 단위로 그린다.
void drawGrassStep() {
  grassX -= SPEED;

  if (grassX < 0) {
    grassX = TFTW;
  }

  tft.drawFastVLine(grassX % TFTW, GAMEH + 1, GRASSH - 1, GRASSCOL);
  tft.drawFastVLine((grassX + 64) % TFTW, GAMEH + 1, GRASSH - 1, GRASSCOL2);
}

// 1/100 px 단위 속도를 정수 픽셀 이동량으로.
//
// 원본은 bird.y(정수) 에 float 속도를 더한 뒤 정수로 잘랐다. 즉 실제 이동량은
// floor(속도) 였다. (-0.74 를 더하면 y 가 1 줄어든다)
// C 의 정수 나눗셈은 0 방향으로 자르므로 음수에서 결과가 달라진다.
// -74/100 은 0 이 되어버린다. 그래서 음수 쪽만 따로 내림 처리한다.
int16_t velToPixels(int16_t vel) {
  if (vel >= 0) {
    return vel / 100;
  }

  return -((-vel + 99) / 100);
}

// ---------------
// 준비
// ---------------
void gameInit() {
  tft.fillScreen(BCKGRDCOL);

  score = 0;

  birdX = 20;
  birdY = birdOldY = TFTH / 2 - BIRDH;
  birdVel = JUMP_VEL;

  pipeX = TFTW;
  grassX = TFTW;

  // 게임 중에 새로 세우는 관과 같은 범위를 쓴다. 원본은 여기만 범위가 넓어서
  // 첫 관에서 이음매 점이 바닥(GAMEH) 아래에 찍히는 경우가 있었다.
  pipeGapY = random(10, GAMEH - (10 + GAPHEIGHT));
}

// ---------------
// 게임 루프
// ---------------
void gameLoop() {
  // 바닥은 한 번만 그리고 이후로는 건드리지 않는다.
  tft.drawFastHLine(0, GAMEH, TFTW, COLOR_BLACK);
  tft.fillRect(0, GAMEH + 1, TFTW2, GRASSH, GRASSCOL);
  tft.fillRect(TFTW2, GAMEH + 1, TFTW2, GRASSH, GRASSCOL2);
  tft.drawFastHLine(0, GAMEH + GRASSH, TFTW, COLOR_BLACK);
  tft.fillRect(0, GAMEH + GRASSH + 1, TFTW, FLOORH - GRASSH, FLOORCOL);

  unsigned long nextGameTick = millis();
  bool passedPipe = false;

  while (true) {
    byte loops = 0;

    // 한 걸음(20ms)마다 상태를 갱신한다.
    // 관과 잔디 그리기를 이 안에 두는 것이 중요하다. 둘 다 "걸음마다 한 칸"
    // 방식이라, 아래 그리기 단계(프레임 단위)에 두면 한 프레임에 두 걸음
    // 이상 진행됐을 때 지우지 못한 칸이 1px 세로줄로 화면에 영구히 남는다.
    while (millis() > nextGameTick && loops < MAX_FRAMESKIP) {
      // ===============
      // 입력
      // ===============
      if (buttonDown()) {
        // 화면 꼭대기에 너무 붙지 않았을 때만 위로 튄다.
        birdVel = (birdY > BIRDH2 / 2) ? JUMP_VEL : 0;
      }

      // ===============
      // 갱신
      // ===============
      birdVel += GRAVITY;
      birdY += velToPixels(birdVel);

      if (birdY < 0) {
        birdY = 0;
      }

      pipeX -= SPEED;

      // 관이 화면 왼쪽으로 사라지면 오른쪽에 새로 세운다.
      if (pipeX < -PIPEW) {
        pipeX = TFTW;
        pipeGapY = random(10, GAMEH - (10 + GAPHEIGHT));
      }

      drawPipeStep();
      drawGrassStep();

      nextGameTick += SKIP_TICKS;
      loops++;
    }

    // 한 걸음도 진행하지 않았으면 그릴 것도 없다.
    // 이 검사가 없으면 같은 장면을 초당 100번 넘게 다시 그리느라 프레임이
    // 길어지고, 그만큼 위 루프가 한 번에 여러 걸음을 돌게 된다.
    if (loops == 0) {
      continue;
    }

    // ===============
    // 그리기 - 새
    // ===============
    // 움직임이 일정하지 않아서 앞뒤 몇 줄만 지우면 자국이 남는다.
    // 그래서 이전 자리를 통째로 지우고 새 자리에 다시 그린다.
    for (int16_t tmpx = BIRDW - 1; tmpx >= 0; tmpx--) {
      int16_t px = birdX + tmpx + BIRDW;

      for (int16_t tmpy = BIRDH - 1; tmpy >= 0; tmpy--) {
        fastPixel(px, birdOldY + tmpy, BCKGRDCOL);
      }

      for (int16_t tmpy = BIRDH - 1; tmpy >= 0; tmpy--) {
        fastPixel(px, birdY + tmpy, pgm_read_word(&birdcol[tmpx + (tmpy * BIRDW)]));
      }
    }

    birdOldY = birdY;

    // ===============
    // 충돌
    // ===============
    if (birdY > GAMEH - BIRDH) {
      return;
    }

    if (birdX + BIRDW >= pipeX - BIRDW2 && birdX <= pipeX + PIPEW - BIRDW) {
      // 관 사이에 들어왔다.
      if (birdY < pipeGapY || birdY + BIRDH > pipeGapY + GAPHEIGHT) {
        return;
      }

      passedPipe = true;
    } else if (birdX > pipeX + PIPEW - BIRDW && passedPipe) {
      passedPipe = false;

      // 자릿수가 늘면 예전 글자가 삐져나오므로 배경색으로 덮어 지운다.
      drawScore(score, BCKGRDCOL);

      score++;
      beep(1200, 30);
    }

    // 관이 지나가면서 점수를 덮으므로 매 프레임 다시 그린다.
    drawScore(score, SCORECOL);
  }
}

void run() {
  gameInit();
  gameLoop();

  beep(200, 400);
  delay(600);

  gameOverScreen(score, EEPROM_ADDR);
}

}  // namespace Flappy

#endif
