#include <Wire.h> // WIRE 라이브러리 추가
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x3F, 16, 2);

const int UpPIN = 2; //상향버튼 7PIN에 입력
const int DownPIN = 4; // 하향 번튼 8PIN에 입력
const int MAXPOT = 800; // 1023까지 돌릴 필요 없이 800까지 가능
const int RANDSEEDPIN = 0; //초기화
const int MAXSTEPDURATION = 400;
const int MINSTEPDURATION = 10;
const int NGLYPHS = 6;

//주의할 점은 glyphs 는 1부터 시작하여 정의
byte glyphs[NGLYPHS][8] = {
  //사용자의 차가 1,3차선에 있을 때
  { B00000,
    B01110,
    B11111,
    B01010,
    B00000,
    B00000,
    B00000,
    B00000
  }
  //사용자의 차가 2,4차선에 있을 때
  , {
    B00000,
    B00000,
    B00000,
    B00000,
    B01110,
    B11111,
    B01010,
    B00000
  }
  //피해야 하는 차들이 1,3 차선에서 올 때
  , {
    B00000,
    B01110,
    B11111,
    B01010,
    B00000,
    B00000,
    B00000,
    B00000
  }
  //피해야 하는 차들이 2,4 차선에서 올때
  , {
    B00000,
    B00000,
    B00000,
    B00000,
    B01110,
    B11111,
    B01010,
    B00000
  }
  //충돌했을 때(1,3차선)
  , {
    B10101,
    B01110,
    B01110,
    B10101,
    B00000,
    B00000,
    B00000,
    B00000
  }
  //충돌했을 때 (2,4 차선)
  , {
    B00000,
    B00000,
    B00000,
    B10101,
    B01110,
    B01110,
    B10101,
    B00000
  }
};
const int NCARPOSITIONS = 4;
//하나의 glyph 열에 2개의 포지션 매핑
const char BLANK = 32;
char car2glyphs[NCARPOSITIONS][2] = {
  {1, BLANK}, {2, BLANK}, {BLANK, 1}, {BLANK, 2}
};
char truck2glyphs[NCARPOSITIONS][2] = {
  {3, BLANK}, {4, BLANK}, {BLANK, 3}, {BLANK, 4}
};
char crash2glyphs[NCARPOSITIONS][2] = {
  {5, BLANK}, {6, BLANK}, {BLANK, 5}, {BLANK, 6}
};

const int ROADLEN = 15; // lcd의 간격
int road[ROADLEN]; // 다가오는 차들의 포지션
char line_buff[2 + ROADLEN]; // drawRoad()의 보조 문자열
int road_index;
int car_pos = 0;
//grid 외부 포지션은 비어있는 열 의미,열에서 자동차의 확률 결정
const int MAXROADPOS = 3 * NCARPOSITIONS;
int step_duration;

int crash;
unsigned int crashtime;  // 충돌할때까지 걸린 시간 (초단위)
const int CRASHSOUNDDURATION = 250;

const char *INTRO1 = "  Game Start  ";
const int INTRODELAY = 2000;
unsigned long now = 0;

void setup() {
  Serial.begin(9600);
  pinMode(UpPIN, INPUT_PULLUP);
  pinMode(DownPIN, INPUT_PULLUP);

  reset();
}

void loop() {

  if (!crash) {
    getSteeringWheel();
    crash = (car_pos == road[road_index]);
  }
  if (crash) {

    unsigned long elapsedTime = millis() - now;

    if (!crashtime) {
      crashtime = now;
      drawRoad();
      // FINISH 문자열
      // 충돌한 열의 상태를 그대로 유지
      lcd.setCursor(3, 0);
      lcd.print("  FINISH  ");
      lcd.setCursor(2, 1);
      lcd.print(elapsedTime / 1000);
      lcd.print("seconds  ");
    }
    if ((now - crashtime) < CRASHSOUNDDURATION) {

    }

    //다시 시작
    if (digitalRead(UpPIN) == LOW || digitalRead(DownPIN) == LOW) {
      reset();
    }

    delay(10);
  }
  else {

    int prev_pos = road[(road_index - 1) % ROADLEN];
    int this_pos = random(MAXROADPOS);
    while (abs(this_pos - prev_pos) < 2) {
      this_pos = random(MAXROADPOS);
    }
    road[road_index] = this_pos;
    road_index = (road_index + 1) % ROADLEN;
    drawRoad();

    delay(step_duration);

    if (step_duration > MINSTEPDURATION) {
      step_duration--;
    }
  }
}

void getSteeringWheel() {
  if (digitalRead(UpPIN) == LOW) {
    if (car_pos > 0) {

      car_pos = car_pos - 1;

    }
  }
  if (digitalRead(DownPIN) == LOW) {

    if (car_pos < 3) {
      car_pos = car_pos + 1;
    }
  }
}

void drawRoad() {
  for (int i = 0; i < 2; i++) {
    if (crash) {
      line_buff[0] = crash2glyphs[car_pos][i];
    }
    else {
      line_buff[0] = car2glyphs[car_pos][i];
    }
    for (int j = 0; j < ROADLEN; j++) {
      int pos = road[(j + road_index) % ROADLEN];
      line_buff[j + 1] = pos >= 0 && pos < NCARPOSITIONS ? truck2glyphs[pos][i] : BLANK;
    }
    lcd.setCursor(0, i);
    lcd.print(line_buff);
  }
}

void reset() {
  lcd.begin();
  lcd.clear();
  crash = crashtime = road_index = 0;
  step_duration = MAXSTEPDURATION;
  line_buff[1 + ROADLEN] = '\0'; // NULL 종료
  randomSeed(analogRead(RANDSEEDPIN));
  for (int i = 0; i < NGLYPHS; i++) {
    lcd.createChar(i + 1, glyphs[i]);
  }
  for (int i = 0; i < ROADLEN; i++) {
    road[i] = -1;
  }


  getSteeringWheel();
  drawRoad();

  lcd.setCursor(2, 0);
  lcd.print(INTRO1);
  delay(INTRODELAY);

  now = millis() - INTRODELAY;
}
