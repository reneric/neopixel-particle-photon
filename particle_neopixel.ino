/* ======================= variables ================================ */
#define PIXEL_COUNT 150
#define PIXEL_PIN D2
#define PIXEL_TYPE WS2812B
#define BallCount 6


/* ======================= includes ================================= */
#include "neopixel_effects.h"
#include "application.h"


/* ======================= prototypes =============================== */
void runEffectsLoop();


SYSTEM_MODE(AUTOMATIC);

void setup() {
  strip.begin();
  strip.show(); // Initialize all pixels to 'off'
  pinMode(led, OUTPUT);
  Particle.function("color", color); // Expose the color function
  Particle.function("clear", clear);
}

void loop() {
  runEffectsLoop();
}

void runEffectsLoop() {
  if(bounce) {
    bouncingColoredBalls();
  }
  if(sparkle) {
    sparkleDisplay(0xff, 0xff, 0xff, 0);
  };
  if(twinkle) {
    twinkleRandom(20, 100, false);
  };
  if(strobeOn) {
    strobe(0xff, 0xff, 0xff, 10, 50, 1000);
  };
  if(fireOn) {
    fire(55,120,1);
  };
  if(eyes) {
    halloweenEyes(0xff, 0x00, 0x00,
                1, 4,
                true, random(5,50), random(50,150),
                random(1000, 5000));
  };
  if(running) {
    runningLights(0xff,0xff,0x00, 50);
  };
  if(kitt) {
    newKITT(0xff, 0, 0, 8, 10, 50);
  };
}
