// Catch a fly - NeoPixel 32x8 게임
// 파란 캐릭터가 지그재그로 움직이며 빨간 파리를 잡는 게임.

#include <Adafruit_GFX.h>
#include <Adafruit_NeoMatrix.h>
#include <Adafruit_NeoPixel.h>
#ifndef PSTR
 #define PSTR // Make Arduino Due happy
#endif

#define PIN 6          // NeoPixel 데이터 핀
#define SWITCH_PIN 7   // 스위치 입력 핀 (D7)

#define MAX_WIDTH 32
#define MAX_HEIGHT 8

// 매트릭스 선언 (좌표는 라이브러리가 논리 -> 물리 매핑 처리, (0,0) = 좌상단)
Adafruit_NeoMatrix matrix = Adafruit_NeoMatrix(MAX_WIDTH, MAX_HEIGHT, PIN,
  NEO_MATRIX_BOTTOM     + NEO_MATRIX_RIGHT +
  NEO_MATRIX_COLUMNS + NEO_MATRIX_ZIGZAG,
  NEO_GRB            + NEO_KHZ800);

// ===== 게임 설정 (변수처리) =====
const int FLIES_PER_ROW = 6;                   // 한 행에 표시할 파리 개수 (랜덤 위치)
const unsigned long BASE_MOVE_INTERVAL = 300;  // 캐릭터 기본 이동 속도 (ms, 클수록 느림)
const float SPEED_UP_STEP = 0.1;               // 파리 1마리당 기본속도의 0.1배씩 누적 가속
const unsigned long MIN_MOVE_INTERVAL = 20;    // 이동 속도 하한 (ms)

float moveInterval = BASE_MOVE_INTERVAL;       // 현재 이동 속도 (잡을 때마다 감소)

// ===== 점수 카운터 =====
int fliesCaught = 0;   // 이번 판에서 잡은 파리 수
int yellowCount = 0;   // 이번 판에서 헛스윙(노란색) 수

// ===== 색상 =====
const uint16_t COLOR_FLY    = matrix.Color(255, 0, 0);    // 빨강 - 파리
const uint16_t COLOR_CHAR   = matrix.Color(0, 0, 255);    // 파랑 - 캐릭터
const uint16_t COLOR_YELLOW = matrix.Color(255, 255, 0);  // 노랑 - 헛스윙 표시

// ===== 셀 상태 =====
#define CELL_EMPTY  0
#define CELL_FLY    1
#define CELL_YELLOW 2
uint8_t grid[MAX_WIDTH][MAX_HEIGHT];

// ===== 캐릭터 상태 =====
int charX = 0;
int charY = 0;
int charDir = 1;   // 이동 방향 (+1: 오른쪽, -1: 왼쪽)

// ===== 타이밍 / 입력 =====
unsigned long lastMove = 0;
int lastSwitchState = HIGH;   // 스위치는 INPUT_PULLUP, 눌리면 LOW

void setup() {
  matrix.begin();
  matrix.setBrightness(100);  // 밝기 조절 (0-255)
  matrix.setTextWrap(false);  // 텍스트 줄바꿈 방지 (점수/카운트다운 표시용)
  matrix.clear();

  pinMode(SWITCH_PIN, INPUT_PULLUP);

  randomSeed(analogRead(A0));  // 연결 안된 아날로그 핀 노이즈로 시드 생성

  initFlies();
  drawScene();
}

void loop() {
  handleSwitch();   // 스위치는 매 루프마다 체크 (반응성 확보)
  handleMovement(); // 이동은 일정 간격마다만
}

// 파리 초기 배치: 각 행마다 FLIES_PER_ROW 개를 서로 다른 랜덤 열에
void initFlies() {
  for (int x = 0; x < MAX_WIDTH; x++)
    for (int y = 0; y < MAX_HEIGHT; y++)
      grid[x][y] = CELL_EMPTY;

  for (int y = 0; y < MAX_HEIGHT; y++) {
    int placed = 0;
    while (placed < FLIES_PER_ROW) {
      int x = random(MAX_WIDTH);
      if (grid[x][y] == CELL_EMPTY) {   // 중복 위치 방지
        grid[x][y] = CELL_FLY;
        placed++;
      }
    }
  }
}

// 스위치 falling edge(HIGH->LOW) 감지 후 판정
void handleSwitch() {
  int state = digitalRead(SWITCH_PIN);
  if (state == LOW && lastSwitchState == HIGH) {
    if (grid[charX][charY] == CELL_FLY) {
      catchFly(charX, charY);   // 위치 일치: 파리 잡기 (점멸 후 제거)
    } else if (grid[charX][charY] == CELL_EMPTY) {
      grid[charX][charY] = CELL_YELLOW;  // 헛스윙: 노란색 표시
      yellowCount++;
    }
    delay(20);  // 간단한 디바운스
  }
  lastSwitchState = state;
}

// 일정 간격마다 캐릭터를 지그재그로 이동
void handleMovement() {
  if (millis() - lastMove < (unsigned long)moveInterval) return;
  lastMove = millis();

  charX += charDir;

  // 행 끝에 도달하면 방향 전환 후 한 칸 아래로
  if (charX < 0 || charX >= MAX_WIDTH) {
    charDir = -charDir;
    charX += charDir;   // 범위 안으로 되돌림
    charY++;
    if (charY >= MAX_HEIGHT) {   // 한 바퀴 완주 -> 라운드 종료
      endRound();
      return;
    }
  }

  drawScene();
}

// 잡힌 파리를 점멸시킨 뒤 제거
void catchFly(int x, int y) {
  for (int i = 0; i < 4; i++) {
    matrix.drawPixel(x, y, (i % 2 == 0) ? 0 : COLOR_FLY);
    matrix.show();
    delay(100);
  }
  grid[x][y] = CELL_EMPTY;

  // 파리 1마리 처치 -> 기본속도의 0.2배씩 누적 가속
  // 속도배율 = 1 + 0.2 * 잡은수  (예: 1마리=1.2배, 5마리=2.0배)
  fliesCaught++;
  moveInterval = BASE_MOVE_INTERVAL / (1.0 + SPEED_UP_STEP * fliesCaught);
  if (moveInterval < MIN_MOVE_INTERVAL) moveInterval = MIN_MOVE_INTERVAL;
}

// 전체 화면 다시 그리기
void drawScene() {
  matrix.clear();

  for (int x = 0; x < MAX_WIDTH; x++) {
    for (int y = 0; y < MAX_HEIGHT; y++) {
      if (grid[x][y] == CELL_FLY) {
        matrix.drawPixel(x, y, COLOR_FLY);
      } else if (grid[x][y] == CELL_YELLOW) {
        matrix.drawPixel(x, y, COLOR_YELLOW);
      }
    }
  }

  // 캐릭터를 맨 위에 그림
  matrix.drawPixel(charX, charY, COLOR_CHAR);

  matrix.show();
}

// 한 바퀴 완주 시: 점수 표시 -> 카운트다운 -> 새 판 시작
void endRound() {
  int score = fliesCaught * 100 - yellowCount * 10;
  showScore(score);
  countdown();
  startNewRound();
}

// 점수를 왼쪽으로 스크롤하여 표시 (자릿수 상관없이 다 보임)
void showScore(int score) {
  char buf[16];
  snprintf(buf, sizeof(buf), "SCORE %d", score);

  int textWidth = strlen(buf) * 6;
  matrix.setTextColor(matrix.Color(0, 255, 255));  // 청록
  for (int pos = MAX_WIDTH; pos >= -textWidth; pos--) {
    matrix.clear();
    matrix.setCursor(pos, 0);
    matrix.print(buf);
    matrix.show();
    delay(100);  // 스크롤 속도 (클수록 느림)
  }
}

// 5,4,3,2,1 카운트다운 (각 1초)
void countdown() {
  for (int n = 5; n >= 1; n--) {
    matrix.clear();
    matrix.setTextColor(matrix.Color(0, 255, 0));  // 초록
    matrix.setCursor(14, 0);   // 한 자리 숫자 대략 가운데
    matrix.print(n);
    matrix.show();
    delay(1000);
  }
}

// 카운터/속도/위치 초기화 + 새 랜덤 파리 배치
void startNewRound() {
  fliesCaught = 0;
  yellowCount = 0;
  moveInterval = BASE_MOVE_INTERVAL;
  charX = 0;
  charY = 0;
  charDir = 1;

  initFlies();
  lastMove = millis();
  drawScene();
}
