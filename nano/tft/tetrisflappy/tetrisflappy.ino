#include "core.h"
#include "tetris.h"
#include "flappy.h"

// 상태 정의
enum GameState {
  TETRIS,
  FLAPPY_BIRD
};

long loops = 0;
// long randNumber;
unsigned short gameSelectYpos = (tft.height() / 2) - 30;

GameState currentGame = FLAPPY_BIRD;

void setup() {

  // randomSeed(analogRead(2));
  // randNumber =  random(1, 10);

  tft.initR(INITR_BLACKTAB);
  tft.fillScreen(COLOR_BLACK);
  pinMode(2, INPUT_PULLUP);

  Serial.begin(9600);

  // if(randNumber % 2 == 0) {
  //   currentGame = TETRIS;
  // } else {
  //   currentGame = FLAPPY_BIRD;
  // }

    centerWrite("TETRIS", gameSelectYpos, COLOR_YELLOW);
    centerWrite("FLAPPY BIRD", gameSelectYpos + 10, COLOR_WHITE);
    currentGame = TETRIS;

  while(true) {
    int joyY = analogRead(JOY_Y);

    if (joyY < 490 ) {
      centerWrite("TETRIS", gameSelectYpos, COLOR_WHITE);
      centerWrite("FLAPPY BIRD", gameSelectYpos + 10, COLOR_YELLOW);
      currentGame = FLAPPY_BIRD;
    }

    if (joyY > 520) {
      centerWrite("TETRIS", gameSelectYpos, COLOR_YELLOW);
      centerWrite("FLAPPY BIRD", gameSelectYpos + 10, COLOR_WHITE);
      currentGame = TETRIS;
    }

    if (digitalRead(JOY_BTN) == LOW) {
      break;
    }
  }

  tft.fillScreen(COLOR_BLACK);
  Serial.println(currentGame);

  if(currentGame == TETRIS) {
    setup_tetris();
  } else {
    setup_flappy();
  }
}

void loop() {
  
  loops++; // do not remove.

  if(currentGame == TETRIS) {
    loop_tetris();
  } else {
    loop_flappy();
  }
}
