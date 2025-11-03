#include <FastLED.h>
#include <queue>

#define LED_PIN     D2
#define COLOR_ORDER GRB
#define CHIPSET     WS2812B
#define BRIGHTNESS  50

#define MATRIX_WIDTH  30
#define MATRIX_HEIGHT 10
#define NUM_LEDS (MATRIX_WIDTH * MATRIX_HEIGHT)

CRGB leds[NUM_LEDS];

// Point struct
struct Point {
  uint8_t x;
  uint8_t y;
  bool operator==(const Point& other) const {
    return x == other.x && y == other.y;
  }
};

// Zigzag matrix mapping
uint16_t XY(uint8_t x, uint8_t y) {
  if (y % 2 == 0) {
    return y * MATRIX_WIDTH + x;
  } else {
    return y * MATRIX_WIDTH + (MATRIX_WIDTH - 1 - x);
  }
}

// Snake state
#define MAX_SNAKE_LENGTH (NUM_LEDS)
Point snake[MAX_SNAKE_LENGTH];
int snakeLength = 4;
int dx = 1, dy = 0;
Point food;

// Font (5x7) for scroll text
const uint8_t font5x7[][5] = {
  {0x00,0x00,0x00,0x00,0x00}, // Space
  {0x3E,0x45,0x49,0x51,0x3E}, // 0
  {0x00,0x21,0x7F,0x01,0x00}, // 1
  {0x21,0x43,0x45,0x49,0x31}, // 2
  {0x42,0x41,0x51,0x69,0x46}, // 3
  {0x3E,0x41,0x49,0x49,0x7A}, // G
  {0x7E,0x09,0x09,0x09,0x7E}, // A
  {0x7F,0x06,0x18,0x06,0x7F}, // M (corrected)
  {0x7F,0x49,0x49,0x49,0x41}, // E
  {0x3E,0x41,0x41,0x41,0x3E}, // O
  {0x7F,0x20,0x10,0x08,0x07}, // V
  {0x7F,0x09,0x19,0x29,0x46}  // R
};

int charToFontIndex(char c) {
  switch (c) {
    case ' ': return 0;
    case '0': return 1;
    case '1': return 2;
    case '2': return 3;
    case '3': return 4;
    case 'G': return 5;
    case 'A': return 6;
    case 'M': return 7;
    case 'E': return 8;
    case 'O': return 9;
    case 'V': return 10;
    case 'R': return 11;
  }
  return 0;
}

const char gameOverMsg[] = " GAME OVER ";

void clearMatrix() {
  fill_solid(leds, NUM_LEDS, CRGB::Black);
}

void drawColumn(int x, uint8_t columnData, CRGB color) {
  for (int y = 0; y < 7; y++) {
    bool pixelOn = columnData & (1 << y);
    int py = y + 1;
    if (x >= 0 && x < MATRIX_WIDTH && py >= 0 && py < MATRIX_HEIGHT) {
      leds[XY(x, py)] = pixelOn ? color : CRGB::Black;
    }
  }
}

// Used for countdown digits only (flips vertically)
void drawFlippedColumn(int x, uint8_t columnData, CRGB color) {
  for (int y = 0; y < 7; y++) {
    bool pixelOn = columnData & (1 << y);
    int py = MATRIX_HEIGHT - 2 - y;
    if (x >= 0 && x < MATRIX_WIDTH && py >= 0 && py < MATRIX_HEIGHT) {
      leds[XY(x, py)] = pixelOn ? color : CRGB::Black;
    }
  }
}

void drawScrollText(const char* msg, int pos, CRGB color) {
  clearMatrix();
  int x = MATRIX_WIDTH - pos;
  for (int i = 0; msg[i] != 0; i++) {
    int idx = charToFontIndex(msg[i]);
    for (int col = 0; col < 5; col++) {
      drawColumn(x + col, font5x7[idx][col], color);
    }
    x += 6;
  }
  FastLED.show();
}

void scrollGameOver() {
  int msgLen = strlen(gameOverMsg) * 6;
  for (int scrollPos = 0; scrollPos <= msgLen + MATRIX_WIDTH; scrollPos++) {
    drawScrollText(gameOverMsg, scrollPos, CRGB::Red);
    delay(100);
  }
}

void countdown() {
  const char* numbers = "321";
  for (int i = 0; i < 3; i++) {
    clearMatrix();
    int idx = charToFontIndex(numbers[i]);
    int startX = (MATRIX_WIDTH - 5) / 2;
    for (int col = 0; col < 5; col++) {
      drawFlippedColumn(startX + col, font5x7[idx][col], CRGB::White);
    }
    FastLED.show();
    delay(1000);
  }
}

void spawnFood() {
  bool valid = false;
  while (!valid) {
    food.x = random(MATRIX_WIDTH);
    food.y = random(MATRIX_HEIGHT);
    valid = true;
    for (int i = 0; i < snakeLength; i++) {
      if (snake[i].x == food.x && snake[i].y == food.y) {
        valid = false;
        break;
      }
    }
  }
}

void resetGame() {
  snakeLength = 4;
  for (int i = 0; i < snakeLength; i++) {
    snake[i].x = i;
    snake[i].y = MATRIX_HEIGHT / 2;
  }
  dx = 1;
  dy = 0;
  spawnFood();
}

bool checkSelfCollision(Point p) {
  for (int i = 0; i < snakeLength - 1; i++) {
    if (snake[i] == p) return true;
  }
  return false;
}

void drawSnakeAndFood() {
  clearMatrix();
  for (int i = 0; i < snakeLength; i++) {
    leds[XY(snake[i].x, snake[i].y)] = CRGB::Green;
  }
  leds[XY(food.x, food.y)] = CRGB::Red;
  FastLED.show();
}

void gameOverEffect() {
  scrollGameOver();
  countdown();
}

// Direction movement using BFS
void updateDirection() {
  std::queue<Point> q;
  bool visited[MATRIX_WIDTH][MATRIX_HEIGHT] = {};
  Point parent[MATRIX_WIDTH][MATRIX_HEIGHT];

  Point start = snake[snakeLength - 1];
  q.push(start);
  visited[start.x][start.y] = true;

  int dxs[] = {0, 1, 0, -1};
  int dys[] = {-1, 0, 1, 0};

  bool found = false;
  while (!q.empty()) {
    Point current = q.front(); q.pop();
    if (current == food) {
      found = true;
      break;
    }
    for (int i = 0; i < 4; i++) {
      int nx = current.x + dxs[i];
      int ny = current.y + dys[i];

      if (nx >= 0 && nx < MATRIX_WIDTH && ny >= 0 && ny < MATRIX_HEIGHT) {
        Point next = { (uint8_t)nx, (uint8_t)ny };
        bool isSnake = false;
        for (int j = 0; j < snakeLength; j++) {
          if (snake[j] == next) {
            isSnake = true;
            break;
          }
        }
        if (!visited[nx][ny] && !isSnake) {
          visited[nx][ny] = true;
          parent[nx][ny] = current;
          q.push(next);
        }
      }
    }
  }

  if (!found) return;

  Point step = food;
  while (!(parent[step.x][step.y] == start)) {
    step = parent[step.x][step.y];
  }

  dx = step.x - start.x;
  dy = step.y - start.y;
}

void setup() {
  FastLED.addLeds<CHIPSET, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS);
  FastLED.setBrightness(BRIGHTNESS);
  randomSeed(analogRead(0));
  resetGame();
}

void loop() {
  updateDirection();

  Point head = snake[snakeLength - 1];
  Point newHead = {(uint8_t)(head.x + dx), (uint8_t)(head.y + dy)};

  // Wrap if out of bounds
  if (newHead.x >= MATRIX_WIDTH) newHead.x = 0;
  if (newHead.y >= MATRIX_HEIGHT) newHead.y = 0;
  if (newHead.x < 0) newHead.x = MATRIX_WIDTH - 1;
  if (newHead.y < 0) newHead.y = MATRIX_HEIGHT - 1;

  if (checkSelfCollision(newHead)) {
    gameOverEffect();
    resetGame();
    return;
  }

  for (int i = 0; i < snakeLength - 1; i++) {
    snake[i] = snake[i + 1];
  }
  snake[snakeLength - 1] = newHead;

  if (newHead == food) {
    if (snakeLength < MAX_SNAKE_LENGTH) {
      snake[snakeLength] = newHead;
      snakeLength++;
    }
    spawnFood();
  }

  drawSnakeAndFood();

  if (snakeLength == MAX_SNAKE_LENGTH) {
    gameOverEffect();
    resetGame();
  }

  delay(150);
}
