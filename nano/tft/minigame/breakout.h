#ifndef breakout_h
#define breakout_h

// ---------------
// BREAKOUT (벽돌깨기)
// ---------------
// 조이스틱을 기울이고 있는 동안 패들이 그 방향으로 움직인다. 놓으면 그 자리에 선다.
// 버튼: 공 발사
//
// 단독 스케치(../breakout)와 같은 내용이다. 여러 게임을 한 스케치에 넣으면
// score / FRAME_MS 같은 이름이 서로 부딪히므로, 게임마다 네임스페이스로 감싸고
// #define 대신 네임스페이스 안의 const 를 쓴다.

#include "core.h"

namespace Breakout {

const int EEPROM_ADDR   = 10;

// 상단 정보 표시줄
const int16_t HUD_H     = 10;

// 벽돌 배치
const byte BRICK_COLS   = 10;
const byte BRICK_ROWS   = 5;
const int16_t BRICK_W   = 12;
const int16_t BRICK_H   = 7;
const int16_t BRICK_GAP_Y = 2;
const int16_t BRICK_X0  = 4;     // (128 - 10*12) / 2
const int16_t BRICK_Y0  = 16;

// 패들
const int16_t PADDLE_W  = 24;
const int16_t PADDLE_H  = 4;
const int16_t PADDLE_Y  = TFTH - 12;
// 끝까지 기울였을 때 패들이 한 프레임에 움직이는 픽셀. 살짝 기울이면 그만큼 느려진다.
// 조작이 둔하면 올리고 너무 빠르면 내린다. (4px * 71fps = 초당 약 280px)
const int16_t PADDLE_SPEED = 4;

// 공
const int16_t BALL_SIZE = 3;
const int16_t BALL_MIN_Y = HUD_H;

const byte START_LIVES  = 3;
const unsigned int FRAME_MS = 14;

// 벽돌 한 줄을 비트 하나씩(10비트) 담는다. 살아있으면 1.
uint16_t bricks[BRICK_ROWS];
byte bricksLeft;

int16_t ballX, ballY;         // 공 왼쪽 위 좌표
int16_t ballOldX, ballOldY;
int8_t ballVX, ballVY;

int16_t paddleX, paddleOldX;

unsigned int score;
byte lives;
byte level;
bool ballHeld;                // 발사 전에는 패들에 붙어있다

// 줄마다 다른 색.
uint16_t rowColor(byte row) {
  switch (row) {
    case 0:  return COLOR_RED;
    case 1:  return COLOR_ORANGE;
    case 2:  return COLOR_YELLOW;
    case 3:  return COLOR_LIME;
    default: return COLOR_CYAN;
  }
}

int16_t brickX(byte col) {
  return BRICK_X0 + col * BRICK_W;
}

int16_t brickY(byte row) {
  return BRICK_Y0 + row * (BRICK_H + BRICK_GAP_Y);
}

void drawBrick(byte row, byte col) {
  // 벽돌 사이가 붙어보이지 않게 오른쪽 1px 는 비워둔다.
  tft.fillRect(brickX(col), brickY(row), BRICK_W - 1, BRICK_H, rowColor(row));
}

void eraseBrick(byte row, byte col) {
  tft.fillRect(brickX(col), brickY(row), BRICK_W - 1, BRICK_H, COLOR_BLACK);
}

void drawAllBricks() {
  for (byte row = 0; row < BRICK_ROWS; row++) {
    for (byte col = 0; col < BRICK_COLS; col++) {
      if (bricks[row] & ((uint16_t)1 << col)) {
        drawBrick(row, col);
      }
    }
  }
}

void resetBricks() {
  for (byte row = 0; row < BRICK_ROWS; row++) {
    bricks[row] = ((uint16_t)1 << BRICK_COLS) - 1;
  }

  bricksLeft = BRICK_ROWS * BRICK_COLS;
}

// ---------------
// 정보 표시줄
// ---------------
void drawHud() {
  tft.fillRect(0, 0, TFTW, HUD_H, COLOR_BLACK);

  tft.setTextSize(1);
  tft.setTextColor(COLOR_WHITE);
  tft.setCursor(2, 1);
  tft.print(score);

  // 남은 목숨을 오른쪽에 작은 사각형으로.
  for (byte i = 0; i < lives; i++) {
    tft.fillRect(TFTW - 6 - i * 8, 2, 5, 5, COLOR_CYAN);
  }
}

// ---------------
// 패들 / 공 그리기
// ---------------
void drawPaddle() {
  if (paddleX == paddleOldX) {
    return;
  }

  // 움직여서 비워진 쪽만 지운다. (패들 전체를 지웠다 그리면 깜빡인다)
  if (paddleX > paddleOldX) {
    int16_t w = paddleX - paddleOldX;
    if (w > PADDLE_W) w = PADDLE_W;
    tft.fillRect(paddleOldX, PADDLE_Y, w, PADDLE_H, COLOR_BLACK);
  } else {
    int16_t w = paddleOldX - paddleX;
    if (w > PADDLE_W) w = PADDLE_W;
    tft.fillRect(paddleOldX + PADDLE_W - w, PADDLE_Y, w, PADDLE_H, COLOR_BLACK);
  }

  tft.fillRect(paddleX, PADDLE_Y, PADDLE_W, PADDLE_H, COLOR_WHITE);
  paddleOldX = paddleX;
}

void drawBall() {
  if (ballX == ballOldX && ballY == ballOldY) {
    return;
  }

  tft.fillRect(ballOldX, ballOldY, BALL_SIZE, BALL_SIZE, COLOR_BLACK);
  tft.fillRect(ballX, ballY, BALL_SIZE, BALL_SIZE, COLOR_WHITE);

  ballOldX = ballX;
  ballOldY = ballY;
}

// ---------------
// 벽돌 충돌
// ---------------
// 공이 지금 위치에서 벽돌과 겹치면 그 벽돌을 없애고 true.
bool hitBrick() {
  // 공이 벽돌 영역 밖이면 검사할 필요가 없다.
  int16_t bottom = brickY(BRICK_ROWS - 1) + BRICK_H;

  if (ballY + BALL_SIZE <= BRICK_Y0 || ballY >= bottom) {
    return false;
  }

  for (byte row = 0; row < BRICK_ROWS; row++) {
    if (bricks[row] == 0) {
      continue;
    }

    int16_t by = brickY(row);

    if (ballY + BALL_SIZE <= by || ballY >= by + BRICK_H) {
      continue;
    }

    for (byte col = 0; col < BRICK_COLS; col++) {
      if (!(bricks[row] & ((uint16_t)1 << col))) {
        continue;
      }

      int16_t bx = brickX(col);

      if (ballX + BALL_SIZE <= bx || ballX >= bx + BRICK_W - 1) {
        continue;
      }

      bricks[row] &= ~((uint16_t)1 << col);
      bricksLeft--;
      eraseBrick(row, col);

      // 위쪽 줄일수록 점수가 높다.
      score += (BRICK_ROWS - row) * 10;
      drawHud();
      beep(600 + (BRICK_ROWS - row) * 120, 15);
      return true;
    }
  }

  return false;
}

// ---------------
// 공 위치 초기화 (발사 대기 상태)
// ---------------
void resetBall() {
  ballHeld = true;
  ballVX = 1;
  ballVY = -2;

  ballX = paddleX + PADDLE_W / 2 - BALL_SIZE / 2;
  ballY = PADDLE_Y - BALL_SIZE - 1;
  ballOldX = ballX;
  ballOldY = ballY;
}

void drawField() {
  tft.fillScreen(COLOR_BLACK);
  drawHud();
  drawAllBricks();

  paddleOldX = paddleX;
  tft.fillRect(paddleX, PADDLE_Y, PADDLE_W, PADDLE_H, COLOR_WHITE);
  tft.fillRect(ballX, ballY, BALL_SIZE, BALL_SIZE, COLOR_WHITE);
}

// ---------------
// 한 판 (공 하나)
// ---------------
// 공을 놓치면 false, 벽돌을 다 깨면 true.
bool playBall() {
  unsigned long nextFrame = millis();

  while (true) {
    // 프레임 간격 유지
    while (millis() < nextFrame) {
    }
    nextFrame = millis() + FRAME_MS;

    // ===============
    // 입력 - 기울인 쪽으로 패들이 따라간다
    // ===============
    paddleX = joyMoveX(paddleX, TFTW - PADDLE_W, PADDLE_SPEED);

    if (ballHeld) {
      // 발사 전에는 공이 패들 위에 붙어있다.
      ballX = paddleX + PADDLE_W / 2 - BALL_SIZE / 2;
      ballY = PADDLE_Y - BALL_SIZE - 1;

      drawPaddle();
      drawBall();

      if (buttonDown()) {
        ballHeld = false;
        // 패들이 향한 쪽으로 살짝 기울여 발사.
        ballVX = (paddleX < TFTW2 - PADDLE_W / 2) ? 1 : -1;
        beep(1200, 30);
        waitForRelease();
      }

      continue;
    }

    // ===============
    // 공 이동 (축 하나씩 옮기고 그때마다 충돌 검사)
    // ===============
    // 가로
    ballX += ballVX;

    if (ballX < 0) {
      ballX = 0;
      ballVX = -ballVX;
      beep(300, 10);
    } else if (ballX > TFTW - BALL_SIZE) {
      ballX = TFTW - BALL_SIZE;
      ballVX = -ballVX;
      beep(300, 10);
    } else if (hitBrick()) {
      ballVX = -ballVX;
      ballX += ballVX;
    }

    // 세로
    ballY += ballVY;

    if (ballY < BALL_MIN_Y) {
      ballY = BALL_MIN_Y;
      ballVY = -ballVY;
      beep(300, 10);
    } else if (hitBrick()) {
      ballVY = -ballVY;
      ballY += ballVY;
    }

    // ===============
    // 패들 충돌
    // ===============
    if (ballVY > 0 &&
        ballY + BALL_SIZE >= PADDLE_Y &&
        ballY < PADDLE_Y + PADDLE_H &&
        ballX + BALL_SIZE > paddleX &&
        ballX < paddleX + PADDLE_W) {

      ballY = PADDLE_Y - BALL_SIZE;
      ballVY = -ballVY;

      // 패들 어디에 맞았느냐로 튀는 각도가 바뀐다. (가운데는 수직, 끝은 비스듬히)
      int16_t offset = (ballX + BALL_SIZE / 2) - (paddleX + PADDLE_W / 2);

      if (offset < -8)      ballVX = -2;
      else if (offset < -3) ballVX = -1;
      else if (offset <= 3) ballVX = (ballVX < 0) ? -1 : 1;
      else if (offset <= 8) ballVX = 1;
      else                  ballVX = 2;

      beep(800, 20);
    }

    drawPaddle();
    drawBall();

    // 벽돌을 다 깼다
    if (bricksLeft == 0) {
      return true;
    }

    // 공을 놓쳤다
    if (ballY > TFTH) {
      return false;
    }
  }
}

// ---------------
// 다음 단계 안내
// ---------------
void levelUp() {
  level++;

  tft.fillScreen(COLOR_BLACK);
  centerText("LEVEL", TFTH2 - 20, COLOR_WHITE, 2);
  centerNumber(level, TFTH2 + 4, COLOR_YELLOW, 3);

  beep(880, 100);
  delay(120);
  beep(1174, 100);
  delay(700);

  resetBricks();
  paddleX = (TFTW - PADDLE_W) / 2;
  resetBall();
  drawField();

  // 단계가 오를수록 공이 빨라진다. (프레임 간격은 그대로 두고 수직 속도를 올린다)
  if (level >= 3) {
    ballVY = -3;
  }
}

// ---------------
// 한 게임
// ---------------
void playGame() {
  score = 0;
  lives = START_LIVES;
  level = 1;

  resetBricks();
  paddleX = (TFTW - PADDLE_W) / 2;
  resetBall();
  drawField();

  while (true) {
    if (playBall()) {
      levelUp();
      continue;
    }

    // 공을 놓쳤다.
    beep(200, 300);
    lives--;
    drawHud();

    if (lives == 0) {
      return;
    }

    delay(600);
    resetBall();

    // 공이 화면 밖으로 나가면서 남긴 흔적 정리.
    tft.fillRect(0, PADDLE_Y - 10, TFTW, TFTH - (PADDLE_Y - 10), COLOR_BLACK);

    // 패들과 공을 직접 그린다.
    // drawBall() 은 "이전에 그린 자리와 달라졌을 때만" 그리는데, resetBall() 이
    // 이전 자리를 지금 자리와 같게 맞춰버리기 때문에 그냥 두면 조이스틱을
    // 움직여 공 위치가 바뀌기 전까지 공이 화면에 나타나지 않는다.
    tft.fillRect(paddleX, PADDLE_Y, PADDLE_W, PADDLE_H, COLOR_WHITE);
    tft.fillRect(ballX, ballY, BALL_SIZE, BALL_SIZE, COLOR_WHITE);
    paddleOldX = paddleX;
  }
}

// 메뉴에서 부르는 진입점.
void run() {
  playGame();
  gameOverScreen(score, EEPROM_ADDR);
}

}  // namespace Breakout

#endif
