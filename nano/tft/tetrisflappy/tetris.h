#include "./src/tetris/config.h"
#include "./src/tetris/shapes.h"
#include "./src/tetris/melody.h"

byte currentShape = 0;
byte currentRotation = 0;
byte nextShapeIndex = 0;

short yOffset = -4;
short xOffset = 0;
short lastY = -4;
short lastX = 0;

unsigned long toneStamp = millis();
unsigned short currentNote = 0;

unsigned short level = 300;
unsigned int score = 0;
unsigned long stamp = 0;
unsigned long lastDown = 0;

uint16_t grid[BOARD_WIDTH][BOARD_HEIGHT];
uint16_t shapeColors[SHAPE_COUNT];

uint16_t getCurrentShapeColor() {
  return shapeColors[currentShape];
}

//블록 1개를 특정 색으로 칠하기.
// 좌표가 보드 밖이면 아무것도 하지 않는다. 새 블록은 yOffset = -4 에서 시작하고
// 지울 때 쓰는 lastXoffset 은 보드 폭을 넘을 수 있어서, 이 검사가 없으면
// grid 배열 밖(= xOffset 같은 다른 전역변수)을 덮어써서 블록이 멋대로 움직인다.
void fillBlock(short x, short y, uint16_t color) {
  if (x < 0 || x >= BOARD_WIDTH || y < 0 || y >= BOARD_HEIGHT) {
    return;
  }

  grid[x][y] = color;
  tft.fillRect(1 + BOARD_OFFSET_X + (x * BLOCK_SIZE), 1 + BOARD_OFFSET_Y + (y * BLOCK_SIZE), BLOCK_SIZE - 1, BLOCK_SIZE - 1, color);
}

// 블록의 가장 아래쪽 행이 보드 바닥에 닿았는지.
// 행 전체를 한번에 검사해서 0번 행과 0번 비트(x = 3)도 빠뜨리지 않고,
// 어떤 경우에도 값을 반환한다. (예전 코드는 둘 다 건너뛰고 return 도 없었다)
bool hittingBottom() {
  for (short i = 3; i >= 0; i--) {
    if (shapes[currentShape][currentRotation][i] != 0) {
      return (i + 1 + yOffset) >= BOARD_HEIGHT;
    }
  }

  return false;
}

void drawNextShape() {
  tft.fillRect(NEXTSHAPE_X, NEXTSHAPE_Y, 30, 30, COLOR_BLACK);
  for (byte i = 0; i < 4; i++) {
    for (short j = 3, x = 0; j != -1; j--, x++) {
      if (bitRead(shapes[nextShapeIndex][0][i], j) == 1) {
        tft.fillRect(NEXTSHAPE_X + (x * BLOCK_SIZE), NEXTSHAPE_Y + (i * BLOCK_SIZE), BLOCK_SIZE - 1, BLOCK_SIZE - 1, shapeColors[nextShapeIndex]);
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

void waitForClick() {
  while (true) {
    while (digitalRead(PUSH_BUTTON) != LOW) {
      delay(100);
    }

    delay(50);
    if (digitalRead(PUSH_BUTTON) == LOW) {
      return;
    }
  }
}

void gameOver() {

  noTone(BUZZER);

  // 최고 기록이면 영구 저장.
  saveHighScore(EEPROM_TETRIS_ADDR, score);

  tft.setTextSize(1);
  tft.setTextColor(COLOR_WHITE);
  tft.setCursor(GAMEOVER_X, GAMEOVER_Y);
  tft.print("GAME");
  tft.setCursor(GAMEOVER_X, GAMEOVER_Y + 10);
  tft.print("OVER");

  waitForClick();

  // 게임 선택 화면으로 돌아간다.
  returnToMenu = true;
}

bool isShapeColliding() {
  short p[4];
  short shiftedUp[4];

  for (byte i = 0; i < 3; i++) {
    shiftedUp[i] = shapes[currentShape][currentRotation][i + 1];
  }
  shiftedUp[3] = 0;

  for (byte i = 0; i < 4; i++) {
    p[i] = shapes[currentShape][currentRotation][i] - (shapes[currentShape][currentRotation][i] & shiftedUp[i]);
  }

  for (byte i = 0; i < 4; i++) {
    byte x = 0;
    for (short j = 3; j != -1; j--) {
      if (bitRead(p[i], j) == 1) {
        if (grid[xOffset + x][max(0, yOffset + i + 1)] != COLOR_BLACK) {
          if (yOffset < -1) {
            gameOver();
          }

          return true;
        }
      }

      x++;
    }
  }

  return false;
}

//한줄이 다 채워졌는지 확인. 동일 row 중 모든 col 블록중 하나라도 검정이면 다 채워지지 않음.
void checkForTetris() {
  bool foundScore = false;
  byte rowsPastFirstMatching = 0;
  for (byte row = 0; row < BOARD_HEIGHT; row++) {
    // Tiny optimization: no need to scan more than four rows for line clears
    if (foundScore && (++rowsPastFirstMatching > 4)) {
      return;
    }

    for (byte col = 0; col < BOARD_WIDTH; col++) {
      if (grid[col][row] == COLOR_BLACK) {
        break;
      }

      // If detected full line
      if (col == (BOARD_WIDTH - 1)) {
        score += LINE_SCORE_VALUE;
        foundScore = true;

        for (byte i = 0; i < BOARD_WIDTH; i++) {
          fillBlock(i, row, COLOR_BLACK);
        }

        for (byte r = row; r > 1; r--) {
          for (byte c = 0; c < BOARD_WIDTH; c++) {
            swap(grid[c][r], grid[c][r - 1]);
            fillBlock(c, r, grid[c][r]);
          }
        }

      }
    }
  }

  if (foundScore) {
    redrawScore();
  }
}

void detectCurrentShapeCollision() {
  if (hittingBottom() || isShapeColliding()) {
    if (returnToMenu) {
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
        if (bitRead(shapes[currentShape][currentRotation][i], j) == 1) {
          fillBlock((k == 0 ? lastXoffset : xOffset) + x, yOffset + i, k == 0 ? COLOR_BLACK : getCurrentShapeColor());
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
    tft.drawFastHLine(BOARD_OFFSET_X, BOARD_OFFSET_Y + (i * BLOCK_SIZE), (BLOCK_SIZE * BOARD_WIDTH), BOARD_COLOR);
  }

  for (int i = 0; i < BOARD_WIDTH + 1; i++) {
    tft.drawFastVLine(BOARD_OFFSET_X + (i * BLOCK_SIZE), BOARD_OFFSET_Y, (BLOCK_SIZE * BOARD_HEIGHT), BOARD_COLOR);
  }
}

byte getShapeWidth() {
  static byte lastShape = currentShape;
  static byte lastRotation = currentRotation;
  static byte lastWidth = 0;

  if (currentShape == lastShape && lastRotation == currentRotation && lastWidth != 0) {
    // If shape hasn't changed, return cached value.
    return lastWidth;
  } else {
    lastWidth = 0;
    lastShape = currentShape;
    lastRotation = currentRotation;
  }

  for (byte i = 0; i < 4; i++) {
    for (short j = 3, x = 0; j != -1; j--, x++) {
      if (bitRead(shapes[currentShape][currentRotation][i], j) == 1) {
        if (j == 0) {
          // Found largest possible value.
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
  byte predictedShapePositions[4];
  for (byte i = 0; i < 4; i++) {
    byte shiftedPiece = (left ? shapes[currentShape][currentRotation][i] >> 1 : shapes[currentShape][currentRotation][i] << 1) & B00001111;
    predictedShapePositions[i] = shapes[currentShape][currentRotation][i] - (shiftedPiece & shapes[currentShape][currentRotation][i]);
  }

  for (byte i = 0; i < 4; i++) {
    for (short j = 3, x = 0; j != -1; j--, x++) {
      if ((yOffset + i) >= 0) {
        if ((bitRead(predictedShapePositions[i], j) == 1) && (grid[xOffset + x + (left ? -1 : 1)][yOffset + i] != COLOR_BLACK)) {
          return false;
        }
      }
    }
  }

  return true;
}
byte getNextRotation() {
  return (shapeRotations[currentShape] - 1) == currentRotation ? 0 : currentRotation + 1;
}

bool canRotate() {
  byte nextRotation = getNextRotation();

  for (byte k = 0; k < 1; k++) {
    for (byte i = 0; i < 4; i++) {
      for (short j = 3, x = 0; j != -1; j--, x++) {
        if (bitRead(shapes[currentShape][nextRotation][i], j) == 1) {
          if ((xOffset + x) >= BOARD_WIDTH) {
            // will rotate off grid
            return false;
          }

          // 보드 위쪽(아직 내려오는 중)은 항상 비어있으므로 검사 불필요.
          // 검사하면 grid 배열 밖을 읽어서 회전이 엉뚱하게 막힌다.
          if ((yOffset + i) < 0) {
            continue;
          }

          if (grid[xOffset + x][yOffset + i] != COLOR_BLACK) {
            if (bitRead(shapes[currentShape][currentRotation][i], j) != 1) {
              return false;
            }
          }
        }
      }
    }
  }

  return true;
}

void rotate() {
  if (shapeRotations[currentShape] == 1) {
    return;
  }

  if (!canRotate()) {
    return;
  }

  for (byte k = 0; k < 1; k++) {
    for (byte i = 0; i < 4; i++) {
      for (short j = 3, x = 0; j != -1; j--, x++) {
        if (bitRead(shapes[currentShape][currentRotation][i], j) == 1) {
          fillBlock(xOffset + x, yOffset + i, k == 0 ? COLOR_BLACK : getCurrentShapeColor());
        }
      }
    }

    currentRotation = getNextRotation();
  }
}

void joystickMovement() {
  // 측정된 중립값 기준으로 판정. (양수면 왼쪽/아래쪽)
  short leftward = joyLeftward();
  short downward = joyDownward();
  unsigned long now = millis();

  static unsigned long lastMove = now;
  static short lastYoffset = yOffset;
  static bool hasClicked = false;

  // left
  if (leftward > JOY_DEADZONE && xOffset > 0 && (now - lastMove) > (MOVE_DELAY + (leftward > JOY_FULL ? 0 : MOVE_DELAY * 5))) {
    if (canMove(true)) {
      lastMove = now;
      xOffset--;
    }
  }

  // right
  if (leftward < -JOY_DEADZONE && xOffset < (BOARD_WIDTH - getShapeWidth()) && (now - lastMove) > (MOVE_DELAY + (leftward < -JOY_FULL ? 0 : MOVE_DELAY * 5))) {
    if (canMove(false)) {
      lastMove = now;
      xOffset++;
    }
  }

  // down
  if (yOffset < lastYoffset && abs(downward) > JOY_DEADZONE) {
    return;
  }

  lastYoffset = yOffset;
  if (downward > JOY_FULL && (now - lastDown) > DOWN_DELAY) {
    stamp -= level;
    lastDown = now;
  }

  // click
  bool isClicked = (digitalRead(PUSH_BUTTON) == LOW);
  if (isClicked) {
    delay(50);
    isClicked = (digitalRead(PUSH_BUTTON) == LOW);
  }

  hasClicked = hasClicked && isClicked;
  if (!hasClicked && isClicked) {
    hasClicked = true;
    rotate();
  }
}

void centerWrite(String text, byte yPos, uint16_t color) {
  tft.setCursor((tft.width() / 2) - (text.length() * 3), yPos);
  tft.setTextColor(color);
  tft.print(text);
}

void setup_tetris() {
  pinMode(JOY_X, INPUT);
  pinMode(JOY_Y, INPUT);

  randomSeed(analogRead(A2));

  // 새 게임을 위해 상태 초기화. (게임 선택 화면에서 다시 들어올 수 있음)
  score = 0;
  level = 300;
  currentRotation = 0;
  yOffset = lastY = -4;
  xOffset = lastX = 0;
  lastDown = 0;
  currentNote = 0;
  toneStamp = millis();

  shapeColors[SHAPE_I] = SHAPE_I_COLOR;
  shapeColors[SHAPE_J] = SHAPE_J_COLOR;
  shapeColors[SHAPE_L] = SHAPE_L_COLOR;
  shapeColors[SHAPE_O] = SHAPE_O_COLOR;
  shapeColors[SHAPE_S] = SHAPE_S_COLOR;
  shapeColors[SHAPE_T] = SHAPE_T_COLOR;
  shapeColors[SHAPE_Z] = SHAPE_Z_COLOR;

  for (byte i = 0; i < BOARD_WIDTH; i++) {
    for (byte j = 0; j < BOARD_HEIGHT; j++) {
      fillBlock(i, j, COLOR_BLACK);
    }
  }

  unsigned short txtYpos = (tft.height() / 2) - 30;
  centerWrite("TETRIS", txtYpos, COLOR_RED);
  centerWrite("THE SOVIET", txtYpos + 20, COLOR_WHITE);
  centerWrite("MIND GAME", txtYpos + 30, COLOR_WHITE);
  centerWrite("PUSH START", txtYpos + 70, COLOR_WHITE);
  centerWrite("(c) 1989 Jonas Jensen", txtYpos + 100, COLOR_WHITE);

  waitForClick();

  // 게임 시작. 최고 기록을 1초간 보여준다.
  showHighScore("TETRIS", EEPROM_TETRIS_ADDR);

  tft.setCursor(SCORE_X, SCORE_Y);
  tft.print("SCORE");
  tft.setCursor(SCORE_X, SCORE_Y + 10);
  tft.print("0");

  drawGrid();
  nextShapeIndex = random(SHAPE_COUNT);
  nextShape();
  stamp = millis();
}

void loop_tetris(){

  unsigned long now = millis();
  unsigned int noteDuration = 1000 / pgm_read_byte_near(noteDurations + currentNote);

  if ((now - toneStamp) > noteDuration) {
    noTone(BUZZER);
  }

  if ((now - toneStamp) > (noteDuration * 1.3)) {
    toneStamp = now;
    if (++currentNote > (sizeof(melody) / 2)) {
      currentNote = 0;
    }

    tone(BUZZER, pgm_read_word_near(melody + currentNote), noteDuration);
  }

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

  if (returnToMenu) {
    return;
  }

  joystickMovement();
}