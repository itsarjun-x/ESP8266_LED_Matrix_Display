#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <NTPClient.h>
#include <FastLED.h>

#define LED_PIN     D2
#define COLOR_ORDER GRB
#define CHIPSET     WS2812B
#define BRIGHTNESS  50

#define MATRIX_WIDTH 30
#define MATRIX_HEIGHT 10
#define NUM_LEDS (MATRIX_WIDTH * MATRIX_HEIGHT)

CRGB leds[NUM_LEDS];

const char* ssid = "Roronoa_Zoro";
const char* password = "6060n04_";

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org", 19800, 1000); // +5:30 IST

// Custom improved font: 5x7
const byte font5x7[][5] = {
  {0x3E,0x51,0x49,0x45,0x3E}, // 0
  {0x00,0x42,0x7F,0x40,0x00}, // 1
  {0x42,0x61,0x51,0x49,0x46}, // 2
  {0x21,0x41,0x45,0x4B,0x31}, // 3
  {0x18,0x14,0x12,0x7F,0x10}, // 4
  {0x27,0x45,0x45,0x45,0x39}, // 5
  {0x3E,0x49,0x49,0x49,0x30}, // 6
  {0x01,0x71,0x09,0x05,0x03}, // 7
  {0x36,0x49,0x49,0x49,0x36}, // 8
  {0x06,0x49,0x49,0x29,0x1E}, // 9
  {0x00,0x36,0x36,0x00,0x00}  // :
};

int getFontIndex(char c) {
  if (c >= '0' && c <= '9') return c - '0';
  if (c == ':') return 10;
  return 0;
}

int getPixelIndex(int x, int y) {
  if (y % 2 == 0)
    return y * MATRIX_WIDTH + x;
  else
    return y * MATRIX_WIDTH + (MATRIX_WIDTH - 1 - x);
}

void drawChar(char c, int xOffset, int yOffset = 1) {
  int index = getFontIndex(c);
  for (int col = 0; col < 5; col++) {
    byte line = font5x7[index][col];
    for (int row = 0; row < 7; row++) {
      bool pixelOn = line & (1 << row);
      int x = xOffset + col;
      int y = yOffset + row;
      if (x >= 0 && x < MATRIX_WIDTH && y >= 0 && y < MATRIX_HEIGHT) {
        leds[getPixelIndex(x, y)] = pixelOn ? CRGB::White : CRGB::Black;
      }
    }
  }
}

void drawText(String text, int xOffset) {
  for (int i = 0; i < text.length(); i++) {
    drawChar(text[i], xOffset + i * 6); // 5px + 1px space
  }
}

void setup() {
  FastLED.addLeds<CHIPSET, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);
  FastLED.setBrightness(BRIGHTNESS);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) delay(1000);
  timeClient.begin();
}

void loop() {
  timeClient.update();

  int hours = timeClient.getHours();
  int minutes = timeClient.getMinutes();

  // Convert to 12-hour format
  if (hours >= 12) {
    if (hours > 12) hours -= 12;
  } else if (hours == 0) {
    hours = 12;
  }

  String timeStr = (hours < 10 ? " " : "") + String(hours) + ":" + (minutes < 10 ? "0" : "") + String(minutes);

  // Total text width = characters * 6
  int textPixelWidth = timeStr.length() * 6 - 1;
  int xOffset = (MATRIX_WIDTH - textPixelWidth) / 2;

  FastLED.clear();
  drawText(timeStr, xOffset);
  FastLED.show();
  delay(1000);
}
