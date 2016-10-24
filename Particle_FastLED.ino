#define NUM_LEDS    300
#define FIRE_SIZE   100

/* ======================= includes ================================= */

#include "FastLED_Effects.h"



/* ======================= variables ================================ */
#define DATA_PIN    D2
#define LED_TYPE    WS2812B
#define COLOR_ORDER GRB
#define BRIGHTNESS         150
#define FRAMES_PER_SECOND  120


void setup() {
  delay(3000); // 3 second delay for recovery
  FastLED.addLeds<LED_TYPE,DATA_PIN,COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);
  FastLED.setBrightness(BRIGHTNESS);
  set_max_power_in_volts_and_milliamps(5, 2200);
  currentPalette = RainbowColors_p;
  targetPalette = RainbowColors_p;
  currentBlending = LINEARBLEND;

  fill_solid(leds, NUM_LEDS, CRGB::DarkOrange);
  delay(3000);
  fill_solid(leds, NUM_LEDS, CRGB::Black);

  for (int i = 0 ; i < BallCount ; i++) {
    ClockTimeSinceLastBounce[i] = millis();
    Height[i] = StartHeight;
    Position[i] = 0;
    ImpactVelocity[i] = ImpactVelocityStart;
    TimeSinceLastBounce[i] = 0;
    Dampening[i] = 0.90 - float(i)/pow(BallCount,2);
  }
  Particle.function("color", color);
  Particle.function("clear", clear);
}


void loop()
{
  show_at_max_brightness_for_power();
  if(demo) {
    gPatterns[gCurrentPatternNumber]();
    FastLED.show();
    // FastLED.delay(1000/FRAMES_PER_SECOND);
    // EVERY_N_MILLISECONDS( 20 ) { gHue++; }
    EVERY_N_SECONDS( 8 ) { nextPattern(); }
  };
  if(rainbowMarch) {
    EVERY_N_MILLISECONDS(thisdelay) {
      rainbowMarchEffect();
    }
  };
  if(colorBeat) {
    colorBeatEffect();
  };
  if(lightning) {
    lightningEffect();
  };
  if(dotBeat) {
    dotBeatEffect();
  };
  if(popFade) {
    popFadeEffect();
  };
  if(bounce) {
    bouncingColoredBalls();
  };
  if(fire) {
    fireEffect();
  }
  if(juggle) {
    juggleEffect();
  };
  if(fastCirc) {
    fastCircLoop();
  };
  if(ripple) {
    EVERY_N_MILLISECONDS(thisdelay) {
      rippleEffect();
    }
  };

}




int color(String command){
    clearVariables();
    Particle.publish("COLOR", command);

    if(command == "demo"){
        demo = true;
    }else if(command == "rainbowMarch"){
        rainbowMarch = true;
    }else if(command == "colorBeat"){
        colorBeat = true;
    }else if(command == "lightning"){
        lightning = true;
    }else if(command == "dotBeat"){
        dotBeat = true;
    }else if(command == "popFade"){
        popFade = true;
    }else if(command == "bounce"){
        bounce = true;
    }else if(command == "fire"){
        fire = true;
    }else if(command == "ripple"){
        ripple = true;
    }else if(command == "juggle"){
        juggle = true;
    }else if(command == "fastCirc"){
        fastCirc = true;
    }else if(command == "clear"){
        fadeToBlack();
    }
    return 1;
}
