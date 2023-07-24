#include "core.h"
#include "tetris.h"
#include "flappy.h"

// 상태 정의
enum GameState {
  TETRIS,
  FLAPPY_BIRD
};

long loops = 0;
double current;

GameState currentGame = FLAPPY_BIRD;

void setup() {

  tft.initR(INITR_BLACKTAB);
  tft.fillScreen(COLOR_BLACK);
  pinMode(3, INPUT_PULLUP);

  Serial.begin(9600);

  if(digitalRead(3) == HIGH) {
    currentGame = TETRIS;
  }

  if(currentGame == TETRIS) {
    setup_tetris();
  } else {
    setup_flappy();
  }

  current = millis();
}

void loop() {
  loops++;

  if(currentGame == TETRIS) {
    loop_tetris();
  } else {
    loop_flappy();
  }

  // if(loops % 10000 == 0) {
  //   Serial.println("loops=");
  //   Serial.println(loops);
  //   Serial.println((millis() - current)/1000);
  //   current = millis();
  // }
}