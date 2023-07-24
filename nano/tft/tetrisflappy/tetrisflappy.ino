#include "core.h"
#include "tetris.h"
#include "flappy.h"

// 상태 정의
enum GameState {
  TETRIS,
  FLAPPY_BIRD
};

long loops = 0;
long randNumber;

GameState currentGame = FLAPPY_BIRD;

void setup() {

  randomSeed(analogRead(2));
  randNumber =  random(1, 10);

  tft.initR(INITR_BLACKTAB);
  tft.fillScreen(COLOR_BLACK);
  pinMode(3, INPUT_PULLUP);

  Serial.begin(9600);
  Serial.println(randNumber);

  if(randNumber % 2 == 0) {
    currentGame = TETRIS;
  } else {
    currentGame = FLAPPY_BIRD;
  }

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
