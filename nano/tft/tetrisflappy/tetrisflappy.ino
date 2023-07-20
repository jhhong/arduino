#include "core.h"
#include "tetris.h"
#include "flappy.h"

void setup() {
  tft.initR(INITR_BLACKTAB);
  tft.fillScreen(COLOR_BLACK);

  Serial.begin(9600);
  setup_tetris();
  setup_flappy();
}

void loop() {
  loop_tetris();
  loop_flappy();
}