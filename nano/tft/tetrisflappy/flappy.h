#define PUSH_BUTTON       2     // PUSH BUTTON
#define TFTW            128     // screen width
#define TFTH            160     // screen height
#define TFTW2            64     // half screen width
#define TFTH2            80     // half screen height
// game constant
#define SPEED             1
#define GRAVITY         9.8
#define JUMP_FORCE     2.15
#define SKIP_TICKS     20.0     // 1000 / 50fps
#define MAX_FRAMESKIP     5
// bird size
#define BIRDW             8     // bird width
#define BIRDH             8     // bird height
#define BIRDW2            4     // half width
#define BIRDH2            4     // half height
// pipe size
#define PIPEW            12     // pipe width
#define GAPHEIGHT        36     // pipe gap height
// floor size
#define FLOORH           20     // floor height (from bottom of the screen)
// grass size
#define GRASSH            4     // grass height (inside floor, starts at floor y)

// background
const unsigned int BCKGRDCOL = tft.Color565(138,235,244);
// bird
const unsigned int BIRDCOL = tft.Color565(255,254,174);
// pipe
const unsigned int PIPECOL  = tft.Color565(99,255,78);
// pipe highlight
const unsigned int PIPEHIGHCOL  = tft.Color565(250,255,250);
// pipe seam
const unsigned int PIPESEAMCOL  = tft.Color565(0,0,0);
// floor
const unsigned int FLOORCOL = tft.Color565(246,240,163);
// grass (col2 is the stripe color)
const unsigned int GRASSCOL  = tft.Color565(141,225,87);
const unsigned int GRASSCOL2 = tft.Color565(156,239,88);

// bird sprite
// bird sprite colors (Cx name for values to keep the array readable)
#define C0 BCKGRDCOL
#define C1 tft.Color565(195,165,75)
#define C2 BIRDCOL
#define C3 ST7735_WHITE
#define C4 ST7735_RED
#define C5 tft.Color565(251,216,114)
static unsigned int birdcol[] =
{ C0, C0, C1, C1, C1, C1, C1, C0,
  C0, C1, C2, C2, C2, C1, C3, C1,
  C0, C2, C2, C2, C2, C1, C3, C1,
  C1, C1, C1, C2, C2, C3, C1, C1,
  C1, C2, C2, C2, C2, C2, C4, C4,
  C1, C2, C2, C2, C1, C5, C4, C0,
  C0, C1, C2, C1, C5, C5, C5, C0,
  C0, C0, C1, C5, C5, C5, C0, C0};

// bird structure
static struct BIRD {
  unsigned char x, y, old_y;
  unsigned int col;
  float vel_y;
} bird;
// pipe structure
static struct PIPE {
  char x, gap_y;
  unsigned int col;
} pipe;

// scoreFlappy
static short scoreFlappy;
// temporary x and y var
static short tmpx, tmpy;

// ---------------
// draw pixel
// ---------------
// faster drawPixel method by inlining calls and using setAddrWindow and pushColor
// using macro to force inlining
#define drawPixel(a, b, c) tft.setAddrWindow(a, b, a, b); tft.pushColor(c)

// ---------------
// game loop
// ---------------
void game_loop() {
  // ===============
  // prepare game variables
  // draw floor
  // ===============
  // instead of calculating the distance of the floor from the screen height each time store it in a variable
  unsigned char GAMEH = TFTH - FLOORH;
  // draw the floor once, we will not overwrite on this area in-game
  // black line
  tft.drawFastHLine(0, GAMEH, TFTW, ST7735_BLACK);
  // grass and stripe
  tft.fillRect(0, GAMEH+1, TFTW2, GRASSH, GRASSCOL);
  tft.fillRect(TFTW2, GAMEH+1, TFTW2, GRASSH, GRASSCOL2);
  // black line
  tft.drawFastHLine(0, GAMEH+GRASSH, TFTW, ST7735_BLACK);
  // mud
  tft.fillRect(0, GAMEH+GRASSH+1, TFTW, FLOORH-GRASSH, FLOORCOL);
  // grass x position (for stripe animation)
  char grassx = TFTW;
  // game loop time variables
  double delta, old_time, next_game_tick, current_time;
  next_game_tick = current_time = millis();
  int loops;
  // passed pipe flag to count scoreFlappy
  bool passed_pipe = false;
  // temp var for setAddrWindow
  unsigned char px;

  while (1) {
    loops = 0;
    while( millis() > next_game_tick && loops < MAX_FRAMESKIP) {
      // ===============
      // input
      // ===============
      if ( !(PIND & (1<<PD2)) ) {
        // if the bird is not too close to the top of the screen apply jump force
        if (bird.y > BIRDH2*0.5) bird.vel_y = -JUMP_FORCE;
        // else zero velocity
        else bird.vel_y = 0;
      }

      // ===============
      // update
      // ===============
      // calculate delta time
      // ---------------
      old_time = current_time;
      current_time = millis();
      delta = (current_time-old_time)/1000;

      // bird
      // ---------------
      bird.vel_y += GRAVITY * delta;
      bird.y += bird.vel_y;

      // pipe
      // ---------------
      pipe.x -= SPEED;
      // if pipe reached edge of the screen reset its position and gap
      if (pipe.x < -PIPEW) {
        pipe.x = TFTW;
        pipe.gap_y = random(10, GAMEH-(10+GAPHEIGHT));
      }

      // ---------------
      next_game_tick += SKIP_TICKS;
      loops++;
    }

    // ===============
    // draw
    // ===============
    // pipe
    // ---------------
    // we save cycles if we avoid drawing the pipe when outside the screen
    if (pipe.x >= 0 && pipe.x < TFTW) {
      // pipe color
      tft.drawFastVLine(pipe.x+3, 0, pipe.gap_y, PIPECOL);
      tft.drawFastVLine(pipe.x+3, pipe.gap_y+GAPHEIGHT+1, GAMEH-(pipe.gap_y+GAPHEIGHT+1), PIPECOL);
      // highlight
      tft.drawFastVLine(pipe.x, 0, pipe.gap_y, PIPEHIGHCOL);
      tft.drawFastVLine(pipe.x, pipe.gap_y+GAPHEIGHT+1, GAMEH-(pipe.gap_y+GAPHEIGHT+1), PIPEHIGHCOL);
      // bottom and top border of pipe
      drawPixel(pipe.x, pipe.gap_y, PIPESEAMCOL);
      drawPixel(pipe.x, pipe.gap_y+GAPHEIGHT, PIPESEAMCOL);
      // pipe seam
      drawPixel(pipe.x, pipe.gap_y-6, PIPESEAMCOL);
      drawPixel(pipe.x, pipe.gap_y+GAPHEIGHT+6, PIPESEAMCOL);
      drawPixel(pipe.x+3, pipe.gap_y-6, PIPESEAMCOL);
      drawPixel(pipe.x+3, pipe.gap_y+GAPHEIGHT+6, PIPESEAMCOL);
    }
    // erase behind pipe
    if (pipe.x <= TFTW) tft.drawFastVLine(pipe.x+PIPEW, 0, GAMEH, BCKGRDCOL);

    // bird
    // ---------------
    tmpx = BIRDW-1;
    do {
          px = bird.x+tmpx+BIRDW;
          // clear bird at previous position stored in old_y
          // we can't just erase the pixels before and after current position
          // because of the non-linear bird movement (it would leave 'dirty' pixels)
          tmpy = BIRDH - 1;
          do {
            drawPixel(px, bird.old_y + tmpy, BCKGRDCOL);
          } while (tmpy--);
          // draw bird sprite at new position
          tmpy = BIRDH - 1;
          do {
            drawPixel(px, bird.y + tmpy, birdcol[tmpx + (tmpy * BIRDW)]);
          } while (tmpy--);
    } while (tmpx--);
    // save position to erase bird on next draw
    bird.old_y = bird.y;

    // grass stripes
    // ---------------
    grassx -= SPEED;
    if (grassx < 0) grassx = TFTW;
    tft.drawFastVLine( grassx    %TFTW, GAMEH+1, GRASSH-1, GRASSCOL);
    tft.drawFastVLine((grassx+64)%TFTW, GAMEH+1, GRASSH-1, GRASSCOL2);

    // ===============
    // collision
    // ===============
    // if the bird hit the ground game over
    if (bird.y > GAMEH-BIRDH) break;
    // checking for bird collision with pipe
    if (bird.x+BIRDW >= pipe.x-BIRDW2 && bird.x <= pipe.x+PIPEW-BIRDW) {
      // bird entered a pipe, check for collision
      if (bird.y < pipe.gap_y || bird.y+BIRDH > pipe.gap_y+GAPHEIGHT) break;
      else passed_pipe = true;
    }
    // if bird has passed the pipe increase scoreFlappy
    else if (bird.x > pipe.x+PIPEW-BIRDW && passed_pipe) {
      passed_pipe = false;
      // erase scoreFlappy with background color
      tft.setTextColor(BCKGRDCOL);
      tft.setCursor( TFTW2, 4);
      tft.print(scoreFlappy);
      // set text color back to white for new scoreFlappy
      tft.setTextColor(ST7735_WHITE);
      // increase scoreFlappy since we successfully passed a pipe
      scoreFlappy++;
    }

    // update scoreFlappy
    // ---------------
    tft.setCursor( TFTW2, 4);
    tft.print(scoreFlappy);
  }

  // add a small delay to show how the player lost
  delay(1200);
}


// ---------------
// game init
// ---------------
void game_init() {
  // clear screen
  tft.fillScreen(BCKGRDCOL);
  // reset scoreFlappy
  scoreFlappy = 0;
  // init bird
  bird.x = 20;
  bird.y = bird.old_y = TFTH2 - BIRDH;
  bird.vel_y = -JUMP_FORCE;
  tmpx = tmpy = 0;
  // generate new random seed for the pipe gape
  randomSeed(analogRead(0));
  // init pipe
  pipe.x = TFTW;
  pipe.gap_y = random(20, TFTH-60);
}

// ---------------
// game start
// ---------------
void game_start() {
  tft.fillScreen(ST7735_BLACK);
  tft.fillRect(10, TFTH2 - 20, TFTW-20, 1, ST7735_WHITE);
  tft.fillRect(10, TFTH2 + 32, TFTW-20, 1, ST7735_WHITE);
  tft.setTextColor(ST7735_WHITE);
  tft.setTextSize(3);
  // half width - num char * char width in pixels
  tft.setCursor( TFTW2-(6*9), TFTH2 - 16);
  tft.println("FLAPPY");
  tft.setTextSize(3);
  tft.setCursor( TFTW2-(6*9), TFTH2 + 8);
  tft.println("-BIRD-");
  tft.setTextSize(0);
  tft.setCursor( 10, TFTH2 - 28);
  tft.println("ATMEGA328");
  tft.setCursor( TFTW2 - (12*3) - 1, TFTH2 + 34);
  tft.println("press button");
  while (1) {
    // wait for push button
    if ( !(PIND & (1<<PD2)) ) break;
  }

  // init game settings
  game_init();
}

// ---------------
// game over
// ---------------
void game_over() {
  tft.fillScreen(ST7735_BLACK);
  tft.setTextColor(ST7735_WHITE);
  tft.setTextSize(2);
  // half width - num char * char width in pixels
  tft.setCursor( TFTW2 - (9*6), TFTH2 - 4);
  tft.println("GAME OVER");
  tft.setTextSize(0);
  tft.setCursor( 10, TFTH2 - 14);
  tft.print("scoreFlappy: ");
  tft.print(scoreFlappy);
  tft.setCursor( TFTW2 - (12*3), TFTH2 + 12);
  tft.println("press button");
  while (1) {
    // wait for push button
    if ( !(PIND & (1<<PD2)) ) break;
  }
}

// ---------------
// initial setup
// ---------------
void setup_flappy() {
  // initialize the push button on pin 2 as an input
  // DDRD &= ~(1<<PD2);
  pinMode(PUSH_BUTTON, INPUT_PULLUP);
  // initialize a ST7735S chip, black tab
  tft.initR(INITR_BLACKTAB);
}

// ---------------
// main loop
// ---------------
void loop_flappy() {
  game_start();
  game_loop();
  game_over();
}