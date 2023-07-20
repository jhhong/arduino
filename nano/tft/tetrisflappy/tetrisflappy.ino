#include "core.h"
#include "tetris.h"
#include "flappy.h"

String games[2] = {"tetris","flappy"};
String game = games[0];

void setup() {
  
  randomSeed(analogRead(0));
  game = games[random(0,2)];

  tft.initR(INITR_BLACKTAB);
  tft.fillScreen(COLOR_BLACK);

  Serial.begin(9600);

  if(game == "tetris") {
    setup_tetris();
  } else {
    setup_flappy();
  }
}

void loop() {

  if(game == "tetris") {
    loop_tetris();
  } else {
    loop_flappy();
  }
}