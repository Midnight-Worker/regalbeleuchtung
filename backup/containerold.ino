#include <Adafruit_NeoPixel.h>

#define LED_PIN     2
#define LED_COUNT   72

#define BRIGHTNESS  50

Adafruit_NeoPixel strip(
  LED_COUNT,
  LED_PIN,
  NEO_GRB + NEO_KHZ800
);

void setup() {
  strip.begin();
  strip.setBrightness(BRIGHTNESS);
  strip.show();
}

void loop() {
  colorWipe(strip.Color(255, 0, 0), 50);   // Rot
  colorWipe(strip.Color(0, 255, 0), 50);   // Grün
  colorWipe(strip.Color(0, 0, 255), 50);   // Blau

  theaterChase(strip.Color(127, 127, 127), 50);
  theaterChase(strip.Color(127, 0, 0), 50);
  theaterChase(strip.Color(0, 0, 127), 50);

  rainbow(10);
}

void colorWipe(uint32_t color, int wait) {
  for (int i = 0; i < strip.numPixels(); i++) {
    strip.setPixelColor(i, color);
    strip.show();
    delay(wait);
  }
}

void theaterChase(uint32_t color, int wait) {
  for (int cycle = 0; cycle < 10; cycle++) {
    for (int offset = 0; offset < 3; offset++) {
      strip.clear();

      for (int i = offset; i < strip.numPixels(); i += 3) {
        strip.setPixelColor(i, color);
      }

      strip.show();
      delay(wait);
    }
  }
}

void rainbow(int wait) {
  for (long firstPixelHue = 0; firstPixelHue < 5 * 65536; firstPixelHue += 256) {
    for (int i = 0; i < strip.numPixels(); i++) {
      int pixelHue = firstPixelHue + (i * 65536L / strip.numPixels());
      strip.setPixelColor(i, strip.gamma32(strip.ColorHSV(pixelHue)));
    }

    strip.show();
    delay(wait);
  }
}
