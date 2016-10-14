/* ======================= variables ================================ */

#define PIXEL_COUNT 150
#define PIXEL_PIN D2
#define PIXEL_TYPE WS2812B
#define BallCount 6


/* ======================= includes ================================= */

#include "neopixel_effects.h"
#include "application.h"


SYSTEM_MODE(AUTOMATIC);

void setup() {
  strip.begin();
  strip.show(); // Initialize all pixels to 'off'
  pinMode(led, OUTPUT);
  Particle.function("color", color); // Expose the color function
  Particle.function("clear", clear);
}

void loop() {
  if(bounce) {
    bouncingColoredBalls();
  }
  if(sparkle) {
    sparkleDisplay(0xff, 0xff, 0xff, 0);
  };
  if(twinkle) {
    twinkleRandom(20, 100, false);
  };
}
