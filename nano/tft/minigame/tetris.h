#ifndef tetris_h
#define tetris_h

// ---------------
// TETRIS (테트리스)
// ---------------
// 조이스틱 좌/우: 이동, 아래: 빠르게 내리기. 버튼: 회전.
//
// tetrisflappy/tetris.h 를 옮겨온 것이고, 합본에 넣으면서 다음을 바꿨다.
//  - 이름 충돌을 피하려고 namespace 로 감쌌다.
//  - String 을 const char* 로 바꿨다. (String 클래스만 플래시 1.2KB 를 먹는다)
//  - shapes / shapeRotations 를 PROGMEM 으로 옮겼다. (RAM 119바이트 회수)
//  - grid 를 uint16_t(색) 에서 uint8_t(조각 번호) 로 바꿨다. (RAM 400 -> 200바이트)
//  - 배경음악(melody.h)을 뺐다. 8개 게임을 32KB 에 넣으려면 자리가 없었다.
//  - 아래 방향 입력이 동작하지 않던 문제를 고쳤다. (joystickMovement 주석 참고)

#include "core.h"

namespace Tetris {

// tetrisflappy 가 쓰던 자리를 그대로 쓴다. 기존 최고 기록이 이어진다.
const int EEPROM_ADDR = 0;

// ---------------
// 판 배치 (원본 src/tetris/config.h 에서 필요한 값만 가져왔다)
// ---------------
const byte BOARD_WIDTH   = 10;
const byte BOARD_HEIGHT  = 20;
const int16_t BOARD_OFFSET_X = 2;
const int16_t BOARD_OFFSET_Y = 17;

// 화면에 들어가는 최대 블록 크기. min((128-1)/10, (160-1)/20) = 7
const int16_t BLOCK_SIZE = 7;

const int16_t GAMEOVER_X = 90;
const int16_t GAMEOVER_Y = 20;
const int16_t SCORE_X    = GAMEOVER_X;
const int16_t SCORE_Y    = GAMEOVER_Y + 50;
const int16_t NEXTSHAPE_X = GAMEOVER_X;
const int16_t NEXTSHAPE_Y = SCORE_Y + 50;

const unsigned int LINE_SCORE_VALUE = 100;

const unsigned int MOVE_DELAY = 50;
const unsigned int DOWN_DELAY = 150;

const byte SHAPE_COUNT = 7;

const byte SHAPE_I = 0;
const byte SHAPE_J = 1;
const byte SHAPE_L = 2;
const byte SHAPE_O = 3;
const byte SHAPE_S = 4;
const byte SHAPE_T = 5;
const byte SHAPE_Z = 6;

// ---------------
// 조각 모양 (PROGMEM)
// ---------------
// const 만 붙이면 AVR 은 이걸 RAM 으로 복사해 온다. 112바이트짜리라 PROGMEM 필수.
const byte shapeRotations[SHAPE_COUNT] PROGMEM = { 2, 4, 4, 1, 2, 4, 2 };

const byte shapes[SHAPE_COUNT][4][4] PROGMEM = {
  { // SHAPE_I
    { B1000, B1000, B1000, B1000 },
    { B0000, B1111, B0000, B0000 }
  },
  { // SHAPE_J
    { B0000, B0100, B0100, B1100 },
    { B0000, B0000, B1000, B1110 },
    { B0000, B1100, B1000, B1000 },
    { B0000, B0000, B1110, B0010 }
  },
  { // SHAPE_L
    { B0000, B1000, B1000, B1100 },
    { B0000, B0000, B1110, B1000 },
    { B0000, B1100, B0100, B0100 },
    { B0000, B0000, B0010, B1110 }
  },
  { // SHAPE_O
    { B0000, B0000, B1100, B1100 }
  },
  { // SHAPE_S
    { B0000, B0110, B1100, B0000 },
    { B0000, B1000, B1100, B0100 }
  },
  { // SHAPE_T
    { B0000, B0000, B0100, B1110 },
    { B0000, B1000, B1100, B1000 },
    { B0000, B0000, B1110, B0100 },
    { B0000, B0100, B1100, B0100 }
  },
  { // SHAPE_Z
    { B0000, B0000, B1100, B0110 },
    { B0000, B0100, B1100, B1000 }
  }
};

byte shapeRow(byte shape, byte rotation, byte i) {
  return pgm_read_byte(&shapes[shape][rotation][i]);
}

byte rotationCount(byte shape) {
  return pgm_read_byte(&shapeRotations[shape]);
}

// ---------------
// 상태
// ---------------
byte currentShape = 0;
byte currentRotation = 0;
byte nextShapeIndex = 0;

short yOffset = -4;
short xOffset = 0;
short lastY = -4;
short lastX = 0;

unsigned short level = 300;
unsigned int score = 0;
unsigned long stamp = 0;
unsigned long lastDown = 0;

bool gameIsOver = false;

// 칸마다 "비었으면 0, 아니면 조각번호+1" 만 담는다.
// 원본은 색(uint16_t)을 통째로 담아서 400바이트를 썼는데, 색은 조각번호로
// 언제든 다시 구할 수 있으므로 절반이면 충분하다.
uint8_t grid[BOARD_WIDTH][BOARD_HEIGHT];

// 조각 번호별 색. 배열로 두면 RAM 14바이트를 잡아먹어서 함수로 만들었다.
uint16_t shapeColor(byte shape) {
  switch (shape) {
    case SHAPE_I: return COLOR_CYAN;
    case SHAPE_J: return COLOR_BLUE;
    case SHAPE_L: return COLOR_ORANGE;
    case SHAPE_O: return COLOR_YELLOW;
    case SHAPE_S: return COLOR_LIME;
    case SHAPE_T: return COLOR_PURPLE;
    default:      return COLOR_RED;
  }
}

// 칸 값(0 = 빈칸, 1~7 = 조각번호+1)을 색으로.
uint16_t cellColor(uint8_t cell) {
  return cell ? shapeColor(cell - 1) : COLOR_BLACK;
}

// ---------------
// 블록 하나 칠하기
// ---------------
// 좌표가 보드 밖이면 아무것도 하지 않는다. 새 블록은 yOffset = -4 에서 시작하고
// 지울 때 쓰는 lastXoffset 은 보드 폭을 넘을 수 있어서, 이 검사가 없으면
// grid 배열 밖(= xOffset 같은 다른 전역변수)을 덮어써서 블록이 멋대로 움직인다.
void fillBlock(short x, short y, uint8_t cell) {
  if (x < 0 || x >= BOARD_WIDTH || y < 0 || y >= BOARD_HEIGHT) {
    return;
  }

  grid[x][y] = cell;
  tft.fillRect(1 + BOARD_OFFSET_X + (x * BLOCK_SIZE),
               1 + BOARD_OFFSET_Y + (y * BLOCK_SIZE),
               BLOCK_SIZE - 1, BLOCK_SIZE - 1, cellColor(cell));
}

// 지금 떨어지는 조각의 칸 값.
uint8_t currentCell() {
  return currentShape + 1;
}

// 블록의 가장 아래쪽 행이 보드 바닥에 닿았는지.
bool hittingBottom() {
  for (short i = 3; i >= 0; i--) {
    if (shapeRow(currentShape, currentRotation, i) != 0) {
      return (i + 1 + yOffset) >= BOARD_HEIGHT;
    }
  }

  return false;
}

void drawNextShape() {
  tft.fillRect(NEXTSHAPE_X, NEXTSHAPE_Y, 30, 30, COLOR_BLACK);

  for (byte i = 0; i < 4; i++) {
    for (short j = 3, x = 0; j != -1; j--, x++) {
      if (bitRead(shapeRow(nextShapeIndex, 0, i), j) == 1) {
        tft.fillRect(NEXTSHAPE_X + (x * BLOCK_SIZE), NEXTSHAPE_Y + (i * BLOCK_SIZE),
                     BLOCK_SIZE - 1, BLOCK_SIZE - 1, shapeColor(nextShapeIndex));
      }
    }
  }
}

void nextShape() {
  yOffset = -4;
  xOffset = 0;
  currentRotation = 0;
  currentShape = nextShapeIndex;
  nextShapeIndex = random(SHAPE_COUNT);
  drawNextShape();
}

void redrawScore() {
  tft.fillRect(SCORE_X, SCORE_Y + 10, 40, 10, COLOR_BLACK);
  tft.setTextSize(1);
  tft.setTextColor(COLOR_YELLOW);
  tft.setCursor(SCORE_X, SCORE_Y + 10);
  tft.print(score);
}

bool isShapeColliding() {
  short p[4];
  short shiftedUp[4];

  for (byte i = 0; i < 3; i++) {
    shiftedUp[i] = shapeRow(currentShape, currentRotation, i + 1);
  }
  shiftedUp[3] = 0;

  for (byte i = 0; i < 4; i++) {
    short row = shapeRow(currentShape, currentRotation, i);
    p[i] = row - (row & shiftedUp[i]);
  }

  for (byte i = 0; i < 4; i++) {
    byte x = 0;
    for (short j = 3; j != -1; j--) {
      if (bitRead(p[i], j) == 1) {
        if (grid[xOffset + x][max(0, yOffset + i + 1)] != 0) {
          if (yOffset < -1) {
            gameIsOver = true;
          }

          return true;
        }
      }

      x++;
    }
  }

  return false;
}

// 한 줄이 다 채워졌는지 확인. 같은 행에서 빈 칸이 하나라도 있으면 아직 아니다.
void checkForTetris() {
  bool foundScore = false;
  byte rowsPastFirstMatching = 0;

  for (byte row = 0; row < BOARD_HEIGHT; row++) {
    // 한 번에 지워지는 줄은 최대 4줄이라 그 뒤로는 볼 필요가 없다.
    if (foundScore && (++rowsPastFirstMatching > 4)) {
      return;
    }

    for (byte col = 0; col < BOARD_WIDTH; col++) {
      if (grid[col][row] == 0) {
        break;
      }

      // 한 줄이 꽉 찼다.
      if (col == (BOARD_WIDTH - 1)) {
        score += LINE_SCORE_VALUE;
        foundScore = true;

        for (byte i = 0; i < BOARD_WIDTH; i++) {
          fillBlock(i, row, 0);
        }

        for (byte r = row; r > 1; r--) {
          for (byte c = 0; c < BOARD_WIDTH; c++) {
            uint8_t tmp = grid[c][r];
            grid[c][r] = grid[c][r - 1];
            grid[c][r - 1] = tmp;
            fillBlock(c, r, grid[c][r]);
          }
        }
      }
    }
  }

  if (foundScore) {
    beep(1000, 60);
    redrawScore();
  }
}

void detectCurrentShapeCollision() {
  if (hittingBottom() || isShapeColliding()) {
    if (gameIsOver) {
      return;
    }

    checkForTetris();
    nextShape();
  }
}

void gravity(bool apply) {
  static byte lastXoffset = 0;

  for (byte k = 0; k < 2; k++) {
    for (byte i = 0; i < 4; i++) {
      for (short j = 3, x = 0; j != -1; j--, x++) {
        if (bitRead(shapeRow(currentShape, currentRotation, i), j) == 1) {
          fillBlock((k == 0 ? lastXoffset : xOffset) + x, yOffset + i,
                    k == 0 ? 0 : currentCell());
        }
      }
    }

    if (k == 0 && apply) {
      yOffset++;
    }
  }

  if (xOffset != lastXoffset) {
    lastXoffset = xOffset;
  }
}

void drawGrid() {
  for (int i = 0; i < BOARD_HEIGHT + 1; i++) {
    tft.drawFastHLine(BOARD_OFFSET_X, BOARD_OFFSET_Y + (i * BLOCK_SIZE),
                      BLOCK_SIZE * BOARD_WIDTH, COLOR_GRAY);
  }

  for (int i = 0; i < BOARD_WIDTH + 1; i++) {
    tft.drawFastVLine(BOARD_OFFSET_X + (i * BLOCK_SIZE), BOARD_OFFSET_Y,
                      BLOCK_SIZE * BOARD_HEIGHT, COLOR_GRAY);
  }
}

byte getShapeWidth() {
  static byte lastShape = 0xFF;
  static byte lastRotation = 0xFF;
  static byte lastWidth = 0;

  if (currentShape == lastShape && lastRotation == currentRotation && lastWidth != 0) {
    return lastWidth;
  }

  lastWidth = 0;
  lastShape = currentShape;
  lastRotation = currentRotation;

  for (byte i = 0; i < 4; i++) {
    for (short j = 3, x = 0; j != -1; j--, x++) {
      if (bitRead(shapeRow(currentShape, currentRotation, i), j) == 1) {
        if (j == 0) {
          lastWidth = 4;
          return 4;
        }

        if ((x + 1) > lastWidth) {
          lastWidth = x + 1;
        }
      }
    }
  }

  return lastWidth;
}

bool canMove(bool left) {
  byte predicted[4];

  for (byte i = 0; i < 4; i++) {
    byte row = shapeRow(currentShape, currentRotation, i);
    byte shifted = (left ? row >> 1 : row << 1) & B00001111;
    predicted[i] = row - (shifted & row);
  }

  for (byte i = 0; i < 4; i++) {
    for (short j = 3, x = 0; j != -1; j--, x++) {
      if ((yOffset + i) >= 0) {
        if ((bitRead(predicted[i], j) == 1) &&
            (grid[xOffset + x + (left ? -1 : 1)][yOffset + i] != 0)) {
          return false;
        }
      }
    }
  }

  return true;
}

byte getNextRotation() {
  return (rotationCount(currentShape) - 1) == currentRotation ? 0 : currentRotation + 1;
}

bool canRotate() {
  byte nextRotation = getNextRotation();

  for (byte i = 0; i < 4; i++) {
    for (short j = 3, x = 0; j != -1; j--, x++) {
      if (bitRead(shapeRow(currentShape, nextRotation, i), j) == 1) {
        if ((xOffset + x) >= BOARD_WIDTH) {
          // 회전하면 판 밖으로 나간다.
          return false;
        }

        // 보드 위쪽(아직 내려오는 중)은 항상 비어있으므로 검사 불필요.
        // 검사하면 grid 배열 밖을 읽어서 회전이 엉뚱하게 막힌다.
        if ((yOffset + i) < 0) {
          continue;
        }

        if (grid[xOffset + x][yOffset + i] != 0) {
          if (bitRead(shapeRow(currentShape, currentRotation, i), j) != 1) {
            return false;
          }
        }
      }
    }
  }

  return true;
}

void rotate() {
  if (rotationCount(currentShape) == 1) {
    return;
  }

  if (!canRotate()) {
    return;
  }

  for (byte k = 0; k < 2; k++) {
    for (byte i = 0; i < 4; i++) {
      for (short j = 3, x = 0; j != -1; j--, x++) {
        if (bitRead(shapeRow(currentShape, currentRotation, i), j) == 1) {
          fillBlock(xOffset + x, yOffset + i, k == 0 ? 0 : currentCell());
        }
      }
    }

    if (k == 0) {
      currentRotation = getNextRotation();
    }
  }

  beep(700, 20);
}

// ---------------
// 조이스틱
// ---------------
void joystickMovement() {
  // 측정된 중립값 기준으로 판정. (양수면 왼쪽/아래쪽)
  short leftward = joyLeftward();
  short downward = joyDownward();
  unsigned long now = millis();

  static unsigned long lastMove = 0;
  static short lastYoffset = 0;
  static bool downLocked = false;
  static bool hasClicked = false;

  // left
  if (leftward > JOY_DEADZONE && xOffset > 0 &&
      (now - lastMove) > (MOVE_DELAY + (leftward > JOY_FULL ? 0 : MOVE_DELAY * 5))) {
    if (canMove(true)) {
      lastMove = now;
      xOffset--;
    }
  }

  // right
  if (leftward < -JOY_DEADZONE && xOffset < (BOARD_WIDTH - getShapeWidth()) &&
      (now - lastMove) > (MOVE_DELAY + (leftward < -JOY_FULL ? 0 : MOVE_DELAY * 5))) {
    if (canMove(false)) {
      lastMove = now;
      xOffset++;
    }
  }

  // down (빠르게 내리기)
  //
  // 원래 코드는 여기가 두 군데 잘못돼 있어서 아래로는 사실상 동작하지 않았다.
  //
  // 1) 문턱값이 JOY_FULL(250) 이었다. 좌/우는 JOY_DEADZONE(50) 을 쓰는데
  //    아래만 조이스틱을 끝까지 밀어야 했고, 모듈에 따라 중립에서 250 만큼
  //    내려가지 않는 경우가 있어 아예 반응이 없었다.
  //
  // 2) "새 조각이 나오면 이전 입력을 무시" 하는 검사가
  //       if (yOffset < lastYoffset && abs(downward) > JOY_DEADZONE) return;
  //       lastYoffset = yOffset;
  //    순서라서, 한 번 걸리면 lastYoffset 이 갱신되지 못하고 낡은 값으로 굳었다.
  //    그러면 조이스틱을 중립으로 되돌리기 전까지 계속 return 해서 아래는 물론
  //    회전(아래쪽 click 처리)까지 통째로 막혔다.
  //
  // 그래서 "새 조각이 나왔으면 중립으로 한 번 돌아와야 다시 인정" 을
  // 잠금 플래그로 따로 두고, lastYoffset 은 항상 갱신한다.
  if (yOffset < lastYoffset) {
    downLocked = true;
  }

  lastYoffset = yOffset;

  if (downward < JOY_DEADZONE) {
    downLocked = false;
  }

  if (!downLocked && downward > JOY_DEADZONE && (now - lastDown) > DOWN_DELAY) {
    stamp -= level;
    lastDown = now;
  }

  // click (회전)
  bool isClicked = buttonDown();

  if (isClicked) {
    delay(50);
    isClicked = buttonDown();
  }

  hasClicked = hasClicked && isClicked;

  if (!hasClicked && isClicked) {
    hasClicked = true;
    rotate();
  }
}

// ---------------
// 한 게임
// ---------------
void playGame() {
  score = 0;
  level = 300;
  currentRotation = 0;
  yOffset = lastY = -4;
  xOffset = lastX = 0;
  lastDown = 0;
  gameIsOver = false;

  memset(grid, 0, sizeof(grid));

  tft.fillScreen(COLOR_BLACK);
  tft.setTextSize(1);
  tft.setTextColor(COLOR_WHITE);
  tft.setCursor(SCORE_X, SCORE_Y);
  tft.print("SCORE");
  tft.setCursor(SCORE_X, SCORE_Y + 10);
  tft.print("0");

  drawGrid();

  nextShapeIndex = random(SHAPE_COUNT);
  nextShape();
  stamp = millis();

  while (!gameIsOver) {
    unsigned long now = millis();

    if ((now - stamp) > level) {
      stamp = millis();
      gravity(true);
    }

    if ((lastX != xOffset) || (lastY != yOffset)) {
      gravity(false);
      detectCurrentShapeCollision();
      lastX = xOffset;
      lastY = yOffset;
    }

    if (gameIsOver) {
      break;
    }

    joystickMovement();
  }

  beep(200, 400);
}

void run() {
  playGame();
  gameOverScreen(score, EEPROM_ADDR);
}

}  // namespace Tetris

#endif
