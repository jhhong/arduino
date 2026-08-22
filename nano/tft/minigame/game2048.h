#ifndef game2048_h
#define game2048_h

// ---------------
// 2048
// ---------------
// 조이스틱을 한 번 튕기면 그 방향으로 판이 밀린다. (중립으로 돌아와야 다음 입력)
// 버튼: 길게(1초) 누르면 처음부터 다시.
//
// 턴제라서 프레임률 걱정이 없다. 판이 바뀐 칸만 다시 그린다.

#include "core.h"

namespace Game2048 {

const int EEPROM_ADDR = 16;

// 칸 배치
const int16_t TILE     = 28;
const int16_t TILE_GAP = 2;
const int16_t BOARD_X  = 5;    // (128 - (4*28 + 3*2)) / 2
const int16_t BOARD_Y  = 34;

const unsigned long RESTART_HOLD_MS = 1000;

const uint16_t EMPTY_COLOR = RGB565(58, 58, 52);
const uint16_t BOARD_BG    = RGB565(30, 30, 28);
const uint16_t DARK_TEXT   = RGB565(70, 60, 50);
const uint16_t TITLE_COLOR = RGB565(237, 194, 46);

// 지수(1=2, 2=4, ... 11=2048, 12=4096)별 칸 색.
const uint16_t tileColors[] PROGMEM = {
  EMPTY_COLOR,
  RGB565(238, 228, 218),  // 2
  RGB565(237, 224, 200),  // 4
  RGB565(242, 177, 121),  // 8
  RGB565(245, 149,  99),  // 16
  RGB565(246, 124,  95),  // 32
  RGB565(246,  94,  59),  // 64
  RGB565(237, 207, 114),  // 128
  RGB565(237, 204,  97),  // 256
  RGB565(237, 200,  80),  // 512
  RGB565(237, 197,  63),  // 1024
  RGB565(237, 194,  46),  // 2048
  RGB565(60,   58, 200),  // 4096
};

const byte MAX_EXP = 12;

uint8_t board[16];
uint8_t shown[16];        // 지금 화면에 그려져 있는 상태. 바뀐 칸만 다시 그리려고 둔다.
unsigned int score;
bool won;

// ---------------
// 칸 그리기
// ---------------
int16_t tileX(byte col) {
  return BOARD_X + col * (TILE + TILE_GAP);
}

int16_t tileY(byte row) {
  return BOARD_Y + row * (TILE + TILE_GAP);
}

void drawTile(byte pos) {
  uint8_t e = board[pos];

  if (e > MAX_EXP) {
    e = MAX_EXP;
  }

  int16_t x = tileX(pos % 4);
  int16_t y = tileY(pos / 4);
  uint16_t color = pgm_read_word(&tileColors[e]);

  tft.fillRect(x, y, TILE, TILE, color);

  if (e == 0) {
    return;
  }

  // 숫자
  char buf[6];
  utoa((unsigned int)1 << e, buf, 10);
  byte len = strlen(buf);

  // 세 자리부터는 크기 2로 그리면 칸을 넘친다.
  byte size = (len <= 2) ? 2 : 1;

  tft.setTextSize(size);
  tft.setTextColor((e <= 2) ? DARK_TEXT : COLOR_WHITE);
  tft.setCursor(x + (TILE - len * 6 * size) / 2, y + (TILE - 8 * size) / 2);
  tft.print(buf);
}

// 바뀐 칸만 다시 그린다.
void drawChanged() {
  for (byte i = 0; i < 16; i++) {
    if (board[i] != shown[i]) {
      drawTile(i);
      shown[i] = board[i];
    }
  }
}

void drawScore() {
  tft.fillRect(0, 0, TFTW, BOARD_Y - 4, COLOR_BLACK);

  tft.setTextSize(2);
  tft.setTextColor(TITLE_COLOR);
  tft.setCursor(4, 8);
  tft.print("2048");

  tft.setTextSize(1);
  tft.setTextColor(COLOR_DARKGRAY);
  tft.setCursor(TFTW - 62, 6);
  tft.print("SCORE");

  tft.setTextColor(COLOR_WHITE);
  tft.setCursor(TFTW - 62, 18);
  tft.print(score);
}

// ---------------
// 새 숫자 놓기
// ---------------
void spawnTile() {
  uint8_t empty[16];
  byte n = 0;

  for (byte i = 0; i < 16; i++) {
    if (board[i] == 0) {
      empty[n++] = i;
    }
  }

  if (n == 0) {
    return;
  }

  byte pos = empty[random(n)];

  // 10% 확률로 4.
  board[pos] = (random(10) == 0) ? 2 : 1;
}

// ---------------
// 한 줄 밀기
// ---------------
// idx[0] 이 미는 방향의 벽에 가장 가까운 칸. 바뀌었으면 true.
bool slideLine(const uint8_t *idx) {
  uint8_t v[4];
  uint8_t out[4] = {0, 0, 0, 0};

  for (byte i = 0; i < 4; i++) {
    v[i] = board[idx[i]];
  }

  byte o = 0;
  byte i = 0;

  while (i < 4) {
    if (v[i] == 0) {
      i++;
      continue;
    }

    // 다음 숫자 칸을 찾는다.
    byte j = i + 1;

    while (j < 4 && v[j] == 0) {
      j++;
    }

    if (j < 4 && v[j] == v[i]) {
      // 같은 숫자면 합친다. 한 번 합쳐진 칸은 이 턴에 또 합쳐지지 않는다.
      out[o] = v[i] + 1;
      score += (unsigned int)1 << out[o];

      if (out[o] >= 11 && !won) {
        won = true;
      }

      i = j + 1;
    } else {
      out[o] = v[i];
      i = j;
    }

    o++;
  }

  bool changed = false;

  for (byte k = 0; k < 4; k++) {
    if (board[idx[k]] != out[k]) {
      board[idx[k]] = out[k];
      changed = true;
    }
  }

  return changed;
}

// 방향대로 판 전체를 민다. 한 줄이라도 바뀌면 true.
bool slide(byte dir) {
  bool changed = false;
  uint8_t idx[4];

  for (byte line = 0; line < 4; line++) {
    for (byte k = 0; k < 4; k++) {
      switch (dir) {
        case DIR_LEFT:  idx[k] = line * 4 + k;         break;
        case DIR_RIGHT: idx[k] = line * 4 + (3 - k);   break;
        case DIR_UP:    idx[k] = line + k * 4;         break;
        default:        idx[k] = line + (3 - k) * 4;   break;
      }
    }

    if (slideLine(idx)) {
      changed = true;
    }
  }

  return changed;
}

// ---------------
// 더 움직일 수 있나
// ---------------
bool canMove() {
  for (byte i = 0; i < 16; i++) {
    if (board[i] == 0) {
      return true;
    }

    byte col = i % 4;
    byte row = i / 4;

    if (col < 3 && board[i] == board[i + 1]) {
      return true;
    }

    if (row < 3 && board[i] == board[i + 4]) {
      return true;
    }
  }

  return false;
}

// ---------------
// 게임 준비
// ---------------
void resetGame() {
  score = 0;
  won = false;

  memset(board, 0, sizeof(board));

  spawnTile();
  spawnTile();

  tft.fillScreen(COLOR_BLACK);

  // 판 바탕
  tft.fillRect(BOARD_X - 3, BOARD_Y - 3,
               4 * TILE + 3 * TILE_GAP + 6, 4 * TILE + 3 * TILE_GAP + 6, BOARD_BG);

  drawScore();

  // 처음에는 전부 그린다.
  for (byte i = 0; i < 16; i++) {
    drawTile(i);
    shown[i] = board[i];
  }
}

// 2048 을 처음 만들었을 때 한 번 알려준다.
void showWin() {
  tft.fillRect(0, TFTH2 - 14, TFTW, 28, TITLE_COLOR);
  centerText("2048!", TFTH2 - 8, COLOR_BLACK, 2);

  beep(880, 100);
  delay(120);
  beep(1174, 100);
  delay(120);
  beep(1568, 200);
  delay(1200);

  // 덮었던 칸들을 다시 그린다.
  tft.fillRect(0, TFTH2 - 14, TFTW, 28, COLOR_BLACK);
  tft.fillRect(BOARD_X - 3, BOARD_Y - 3,
               4 * TILE + 3 * TILE_GAP + 6, 4 * TILE + 3 * TILE_GAP + 6, BOARD_BG);

  for (byte i = 0; i < 16; i++) {
    drawTile(i);
  }
}

// ---------------
// 입력 - 한 번 튕길 때마다 한 번만 움직인다
// ---------------
byte waitInput() {
  // 조이스틱이 중립으로 돌아올 때까지 기다린다.
  while (joyDir() != DIR_NONE) {
    delay(10);
  }

  unsigned long pressedAt = 0;

  while (true) {
    byte d = joyDir();

    if (d != DIR_NONE) {
      return d;
    }

    // 버튼을 오래 누르면 새 게임.
    if (buttonDown()) {
      if (pressedAt == 0) {
        pressedAt = millis();
      } else if (millis() - pressedAt > RESTART_HOLD_MS) {
        beep(400, 150);
        waitForRelease();
        return DIR_NONE;
      }
    } else {
      pressedAt = 0;
    }

    delay(10);
  }
}

void playGame() {
  resetGame();

  while (true) {
    byte d = waitInput();

    // 버튼 길게 누르기 = 처음부터.
    if (d == DIR_NONE) {
      resetGame();
      continue;
    }

    if (!slide(d)) {
      // 그 방향으로는 밀리는게 없다.
      beep(150, 30);
      continue;
    }

    beep(700, 20);
    spawnTile();
    drawChanged();
    drawScore();

    if (won) {
      showWin();
      won = false;   // 한 번만 알린다
    }

    if (!canMove()) {
      beep(200, 400);
      return;
    }
  }
}

void run() {
  playGame();
  gameOverScreen(score, EEPROM_ADDR);
}

}  // namespace Game2048

#endif
