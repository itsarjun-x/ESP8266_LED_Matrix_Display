#include <FastLED.h>

#define LED_PIN    D2
#define COLOR_ORDER GRB
#define CHIPSET     WS2812B
#define BRIGHTNESS  50
#define WIDTH       30
#define HEIGHT      10
#define NUM_LEDS    (WIDTH * HEIGHT)

CRGB leds[NUM_LEDS];

// Game settings
int GAP_HEIGHT = 5;
#define PIPE_INTERVAL_MIN 7
#define BIRD_X       5
#define FRAME_DELAY  30

int pipeSpeed = 5;
int pipeMoveCounter = 0;
int PIPE_INTERVAL = 20;
int frameCount = 0;
int stageFrameCount = 0;

int pipeStage = 0;  // 0: slow, 1: medium, 2: fast
int pipeStageDuration = 700; // <<< Changed to 700 frame cycles

struct Pipe {
  int x;
  int gapY;
  int gapHeight;
  int width;
};

std::vector<Pipe> pipes;

float birdY = 4.0;
float birdVelocity = 0;
bool gameOver = false;
unsigned long collisionTime = 0;

int XY(int x, int y) {
  if (x < 0 || x >= WIDTH || y < 0 || y >= HEIGHT) return -1;
  if (y % 2 == 0)
    return y * WIDTH + x;
  else
    return y * WIDTH + (WIDTH - 1 - x);
}

void drawBird(int x, int y, CRGB color) {
  int coords[5][2] = {
    {x, y}, {x+1, y},
    {x-1, y+1}, {x, y+1}, {x+1, y+1}
  };
  for (auto& pt : coords) {
    int i = XY(pt[0], pt[1]);
    if (i >= 0 && i < NUM_LEDS) leds[i] = color;
  }
}

void drawPipe(const Pipe& pipe) {
  CRGB pipeColor = CRGB::Green;
  if (pipeStage == 1) pipeColor = CRGB::Yellow;
  if (pipeStage == 2) pipeColor = CRGB::Red;

  for (int xOff = 0; xOff < pipe.width; xOff++) {
    int px = pipe.x - xOff;
    if (px < 0 || px >= WIDTH) continue;
    for (int y = 0; y < HEIGHT; y++) {
      if (y < pipe.gapY || y > pipe.gapY + pipe.gapHeight - 1) {
        int i = XY(px, y);
        if (i >= 0 && i < NUM_LEDS) leds[i] = pipeColor;
      }
    }
  }
}

void spawnPipe() {
  Pipe p;
  p.x = WIDTH - 1;
  p.gapHeight = random(4, 8);
  p.width = random(1, 4);
  p.gapY = random(1, HEIGHT - p.gapHeight - 1);
  pipes.push_back(p);
}

void scrollText(const char* msg) {
  // placeholder for future scrolling implementation
}

void drawCountdown(int n) {
  FastLED.clear();
  CRGB color = CRGB::White;
  for (int x = 12; x < 18; x++) {
    int i = XY(x, 4);
    if (i >= 0 && i < NUM_LEDS) leds[i] = color;
  }
  FastLED.show();
  delay(700);
  FastLED.clear();
  FastLED.show();
  delay(300);
}

void resetGame() {
  pipes.clear();
  birdY = 4;
  birdVelocity = 0;
  gameOver = false;
  frameCount = 0;
  stageFrameCount = 0;
  pipeSpeed = 5;
  PIPE_INTERVAL = 20;
  pipeStage = 0;
  spawnPipe();
}

void setup() {
  delay(100);
  FastLED.addLeds<CHIPSET, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS);
  FastLED.setBrightness(BRIGHTNESS);
  resetGame();
  randomSeed(analogRead(A0));
}

void loop() {
  FastLED.clear();

  if (gameOver) {
    if (millis() - collisionTime < 3000) {
      drawBird(BIRD_X, (int)birdY, millis() % 500 < 250 ? CRGB::Red : CRGB::Black);
      FastLED.show();
      delay(FRAME_DELAY);
      return;
    }
    scrollText("Game Over");
    for (int i = 3; i >= 1; i--) drawCountdown(i);
    resetGame();
    return;
  }

  // === Pipe speed/difficulty cycling every 700 frames ===
  stageFrameCount++;
  if (stageFrameCount >= pipeStageDuration) {
    stageFrameCount = 0;
    pipeStage++;
    if (pipeStage > 2) pipeStage = 0;

    if (pipeStage == 0) {
      pipeSpeed = 5;
      PIPE_INTERVAL = 20;
    } else if (pipeStage == 1) {
      pipeSpeed = 4;
      PIPE_INTERVAL = 15;
    } else if (pipeStage == 2) {
      pipeSpeed = 3;
      PIPE_INTERVAL = 10;
    }
  }

  pipeMoveCounter++;
  if (pipeMoveCounter >= pipeSpeed) {
    for (auto& pipe : pipes) pipe.x--;
    pipeMoveCounter = 0;
  }

  if (pipes.empty() || pipes.back().x < WIDTH - PIPE_INTERVAL) {
    spawnPipe();
  }
  if (!pipes.empty() && pipes[0].x < -2) pipes.erase(pipes.begin());

  for (auto& pipe : pipes) drawPipe(pipe);

  // Auto-jump logic
  for (auto& pipe : pipes) {
    if (pipe.x >= BIRD_X - 1) {
      if (pipe.x - BIRD_X <= 6) {
        int targetY = pipe.gapY + pipe.gapHeight / 2 - 1;
        if ((int)birdY > targetY) {
          birdVelocity = -0.9;
        }
      }
      break;
    }
  }

  birdVelocity += 0.2;
  birdY += birdVelocity;
  if (birdY < 0) birdY = 0;
  if (birdY > HEIGHT - 2) birdY = HEIGHT - 2;

  drawBird(BIRD_X, (int)birdY, CRGB::White);

  for (auto& pipe : pipes) {
    for (int xOff = 0; xOff < 2; xOff++) {
      int px = pipe.x - xOff;
      if (px == BIRD_X || px == BIRD_X + 1 || px == BIRD_X - 1) {
        if ((int)birdY < pipe.gapY || (int)birdY + 1 > pipe.gapY + pipe.gapHeight - 1) {
          gameOver = true;
          collisionTime = millis();
        }
      }
    }
  }

  FastLED.show();
  delay(FRAME_DELAY);
  frameCount++;
}
