#include "core.h"
#include "tetris.h"
#include "flappy.h"

// 상태 정의
enum GameState {
  TETRIS,
  FLAPPY_BIRD
};

long loops = 0;
unsigned short gameSelectYpos = (tft.height() / 2) - 30;

GameState currentGame = FLAPPY_BIRD;

// 게임 선택 화면. 처음 시작할 때와 게임이 끝날 때마다 호출된다.
void selectGame() {

  // 직전 화면(GAME OVER 등)에서 누른 버튼이 그대로 넘어오지 않게 대기.
  waitForRelease();

  tft.fillScreen(COLOR_BLACK);
  tft.setTextSize(1);

  currentGame = TETRIS;
  centerWrite("TETRIS", gameSelectYpos, COLOR_YELLOW);
  centerWrite("FLAPPY BIRD", gameSelectYpos + 10, COLOR_WHITE);
  centerWrite("FROM SEOBIN", gameSelectYpos + 90, COLOR_WHITE);

  // 첫 화면이 뜬 시점의 값을 조이스틱 중립값으로 삼는다. (맨 처음 한 번만)
  static bool joyCalibrated = false;
  if (!joyCalibrated) {
    joyCalibrated = true;
    calibrateJoystick();
  }

  while (true) {
    // 측정된 중립값 기준으로 판정. (양수면 아래쪽)
    short downward = joyDownward();

    if (downward > JOY_DEADZONE && currentGame != FLAPPY_BIRD) {
      centerWrite("TETRIS", gameSelectYpos, COLOR_WHITE);
      centerWrite("FLAPPY BIRD", gameSelectYpos + 10, COLOR_YELLOW);
      currentGame = FLAPPY_BIRD;
    }

    if (downward < -JOY_DEADZONE && currentGame != TETRIS) {
      centerWrite("TETRIS", gameSelectYpos, COLOR_YELLOW);
      centerWrite("FLAPPY BIRD", gameSelectYpos + 10, COLOR_WHITE);
      currentGame = TETRIS;
    }

    if (digitalRead(PUSH_BUTTON) == LOW) {
      break;
    }
  }

  // 선택할 때 누른 버튼이 게임 시작 화면까지 넘어가지 않게 대기.
  waitForRelease();

  tft.fillScreen(COLOR_BLACK);
  Serial.println(currentGame);

  returnToMenu = false;

  if (currentGame == TETRIS) {
    setup_tetris();
  } else {
    setup_flappy();
  }
}

void setup() {

  tft.initR(INITR_BLACKTAB);
  tft.fillScreen(COLOR_BLACK);
  pinMode(PUSH_BUTTON, INPUT_PULLUP);
  pinMode(JOY_X, INPUT);
  pinMode(JOY_Y, INPUT);

  Serial.begin(9600);

  selectGame();
}

void loop() {

  loops++; // do not remove.

  if (currentGame == TETRIS) {
    loop_tetris();
  } else {
    loop_flappy();
  }

  // 게임이 끝나면 다시 게임 선택 화면으로.
  if (returnToMenu) {
    selectGame();
  }
}
