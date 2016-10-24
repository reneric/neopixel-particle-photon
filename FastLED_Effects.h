#include <math.h>

/* ======================= prototypes =============================== */
void clearVariables();


/* ======================= variables ================================ */
uint8_t thisdelay = 5;
uint8_t thishue = 0;
uint8_t deltahue = 10;

// DotBeat variables
uint8_t   count =   0;
uint8_t fadeval = 224;
uint8_t bpmRate = 30;

// Lightning variables
uint8_t frequency = 100;
uint8_t flashes = 10;
unsigned int dimmer = 1;
uint8_t ledstart;
uint8_t ledlen;

// Fast Circ
int fcCount = 0;
int fcHue = 0;
int fcSat = 255;
int fcBright = 255;
int fcDir = 1;

// Popfade
int ranamount = 50;
bool boolcolours = 1;
uint8_t numcolours = 2;
unsigned long colours[10] = {0xff0000, 0x00ff00, 0x0000ff, 0xffffff};
uint8_t maxbar = 1;

// Ripple effect
uint8_t colour;        // Ripple colour is randomized.
int center = 0;        // Center of the current ripple.
int step = -1;         // -1 is the initializing step.
uint8_t myfade = 255;  // Starting brightness.
#define maxsteps 16    // Case statement wouldn't allow a variable.

uint8_t bgcol = 0;

// Rainbow March variables
uint8_t maxChanges = 24;


// Effect booleans
bool demo = false;
bool rainbowMarch = false;
bool colorBeat = false;
bool lightning = false;
bool dotBeat = false;
bool popFade = false;
bool bounce = false;
bool fire = false;
bool ripple = false;
bool juggle = false;
bool fastCirc = false;


// Bouncing ball colors
#define BallCount 5
float Gravity = -8.81;
int StartHeight = 30;

float Height[BallCount];
float ImpactVelocityStart = sqrt( -2 * Gravity * StartHeight );
float ImpactVelocity[BallCount];
float TimeSinceLastBounce[BallCount];
int   Position[BallCount];
long  ClockTimeSinceLastBounce[BallCount];
float Dampening[BallCount];
byte colors[BallCount][3] = { {0xff, 0,0},        // red
                              {0, 0xff, 0},       // green
                              {0, 0, 0xff},       // blue
                              {0xff, 0xff, 0xff}, // white
                              {0, 0xff, 0xff}};    // teal


// List of patterns to cycle through.  Each is defined as a separate function below.
typedef void (*SimplePatternList[])();
// SimplePatternList gPatterns = { rainbow, rainbowWithGlitter, confetti, sinelon, bpm, fireEffect, lightningEffect, rainbowMarchEffect, dotBeatEffect };
SimplePatternList gPatterns = { fireEffect, lightningEffect };

uint8_t gCurrentPatternNumber = 0; // Index number of which pattern is current
uint8_t gHue = 0; // rotating "base color" used by many of the patterns


/* ========================= effects =============================== */

void rippleEffect() {
  for (int i = 0; i < NUM_LEDS; i++) leds[i] = CHSV(bgcol++, 255, 15);  // Rotate background colour.

  switch (step) {

    case -1:                                                          // Initialize ripple variables.
      center = random(NUM_LEDS);
      colour = random8();
      step = 0;
      break;

    case 0:
      leds[center] = CHSV(colour, 255, 255);                          // Display the first pixel of the ripple.
      step ++;
      break;

    case maxsteps:                                                    // At the end of the ripples.
      step = -1;
      break;

    default:                                                             // Middle of the ripples.
      leds[(center + step + NUM_LEDS) % NUM_LEDS] += CHSV(colour, 255, myfade/step*2);       // Simple wrap from Marc Miller
      leds[(center - step + NUM_LEDS) % NUM_LEDS] += CHSV(colour, 255, myfade/step*2);
      step ++;                                                         // Next step.
      break;
  }
}

void popFadeEffect() {
  changeMe();
  EVERY_N_MILLISECONDS(thisdelay) {
    unsigned long thiscolour;
    if (ranamount >NUM_LEDS) ranamount = NUM_LEDS;               // Make sure we're at least utilizing ALL the LED's.
    int idex = random16(0, ranamount);

    if (idex < NUM_LEDS) {                                      // Only the lowest probability twinkles will do.
      boolcolours ? thiscolour = random(0, 0xffffff) : thiscolour =  colours[random16(0, numcolours)];
      int barlen = random16(1,maxbar);
      for (int i = 0; i <barlen; i++)
        if (idex+i < NUM_LEDS) leds[idex+i] = thiscolour;       // Make sure we don't overshoot the array.
    }
    for (int i = 0; i <NUM_LEDS; i++) leds[i].nscale8(fadeval);
  }
}

void rainbowMarchEffect() {
  thishue++;
  fill_rainbow(leds, NUM_LEDS, thishue, deltahue);
}

void colorBeatEffect() {
  uint8_t beatA = beatsin8(5, 0, 255);
  fillLEDsFromPaletteColors(beatA);

  EVERY_N_MILLISECONDS(100) {
    nblendPaletteTowardPalette( currentPalette, targetPalette, maxChanges);
  }

  EVERY_N_MILLISECONDS(5000) {
    setupRandomPalette();
  }
}

#define ARRAY_SIZE(A) (sizeof(A) / sizeof((A)[0]))

void nextPattern()
{
  fill_solid(leds, NUM_LEDS, CRGB::Black);
  // add one to the current pattern number, and wrap around at the end
  gCurrentPatternNumber = (gCurrentPatternNumber + 1) % ARRAY_SIZE( gPatterns);
}

void rainbow()
{
  // FastLED's built-in rainbow generator
  fill_rainbow( leds, NUM_LEDS, gHue, 7);
}

void addGlitter( NSFastLED::fract8 chanceOfGlitter)
{
  if( random8() < chanceOfGlitter) {
    leds[ random16(NUM_LEDS) ] += CRGB::White;
  }
}

void lightningEffect() {
  ledstart = random8(NUM_LEDS);           // Determine starting location of flash
  ledlen = random8(NUM_LEDS-ledstart);    // Determine length of flash (not to go beyond NUM_LEDS-1)
  for (int flashCounter = 0; flashCounter < random8(3,flashes); flashCounter++) {
    if(flashCounter == 0) dimmer = 5;     // the brightness of the leader is scaled down by a factor of 5
    else dimmer = random8(1,3);           // return strokes are brighter than the leader
    fill_solid(leds+ledstart,ledlen,CHSV(255, 0, 255/dimmer));
    FastLED.show();                       // Show a section of LED's
    delay(random8(4,10));                 // each flash only lasts 4-10 milliseconds
    fill_solid(leds+ledstart,ledlen,CHSV(255,0,0));   // Clear the section of LED's
    FastLED.show();
    if (flashCounter == 0) delay (150);   // longer delay until next flash after the leader
    delay(50+random8(100));               // shorter delay between strokes
  } // for()
  delay(random8(frequency)*100);          // delay between strikes

}

void rainbowWithGlitter()
{
  // built-in FastLED rainbow, plus some random sparkly glitter
  rainbow();
  addGlitter(80);
}

void fadeToBlack()
{
  fadeToBlackBy( leds, NUM_LEDS, 10);
}


void confetti()
{
  // random colored speckles that blink in and fade smoothly
  fadeToBlackBy( leds, NUM_LEDS, 10);
  int pos = random16(NUM_LEDS);
  leds[pos] += CHSV( gHue + random8(64), 200, 255);
}

void sinelon()
{
  // a colored dot sweeping back and forth, with fading trails
  fadeToBlackBy( leds, NUM_LEDS, 20);
  int pos = beatsin16(13,0,NUM_LEDS);
  leds[pos] += CHSV( gHue, 255, 192);
}

void bpm()
{
  // colored stripes pulsing at a defined Beats-Per-Minute (BPM)
  uint8_t BeatsPerMinute = 62;
  CRGBPalette16 palette = PartyColors_p;
  uint8_t beat = beatsin8( BeatsPerMinute, 64, 255);
  for( int i = 0; i < NUM_LEDS; i++) { //9948
    leds[i] = ColorFromPalette(palette, gHue+(i*2), beat-gHue+(i*10));
  }
}

void juggleEffect() {
  // eight colored dots, weaving in and out of sync with each other
  fadeToBlackBy( leds, NUM_LEDS, 20);
  byte dothue = 0;
  for( int i = 0; i < 8; i++) {
    leds[beatsin16(i+7,0,NUM_LEDS)] |= CHSV(dothue, 200, 255);
    dothue += 32;
  }
}

void dotBeatEffect() {
  uint8_t inner = beatsin8(bpmRate, NUM_LEDS/4, NUM_LEDS/4*3);
  uint8_t outer = beatsin8(bpmRate, 0, NUM_LEDS-1);
  uint8_t middle = beatsin8(bpmRate, NUM_LEDS/3, NUM_LEDS/3*2);

  leds[middle] = CRGB::Purple;
  leds[inner] = CRGB::Blue;
  leds[outer] = CRGB::Aqua;

  nscale8(leds,NUM_LEDS,fadeval);
}

void popFadeLogic() {
  unsigned long thiscolour;
  if (ranamount >NUM_LEDS) ranamount = NUM_LEDS;               // Make sure we're at least utilizing ALL the LED's.
  int idex = random16(0, ranamount);

  if (idex < NUM_LEDS) {                                      // Only the lowest probability twinkles will do.
    boolcolours ? thiscolour = random(0, 0xffffff) : thiscolour =  colours[random16(0, numcolours)];
    int barlen = random16(1,maxbar);
    for (int i = 0; i <barlen; i++)
      if (idex+i < NUM_LEDS) leds[idex+i] = thiscolour;       // Make sure we don't overshoot the array.
  }
  for (int i = 0; i <NUM_LEDS; i++) leds[i].nscale8(fadeval); // Go through the array and reduce each RGB value by a percentage.
} // pop_fade()

void bouncingColoredBalls() {
  for (int i = 0 ; i < BallCount ; i++) {
    TimeSinceLastBounce[i] =  millis() - ClockTimeSinceLastBounce[i];
    Height[i] = 0.5 * Gravity * pow( TimeSinceLastBounce[i]/1000 , 2.0 ) + ImpactVelocity[i] * TimeSinceLastBounce[i]/1000;

    if ( Height[i] < 0 ) {
      Height[i] = 0;
      ImpactVelocity[i] = Dampening[i] * ImpactVelocity[i];
      ClockTimeSinceLastBounce[i] = millis();

      if ( ImpactVelocity[i] < 0.01 ) {
        ImpactVelocity[i] = ImpactVelocityStart;
      }
    }
    Position[i] = round( Height[i] * (NUM_LEDS - 1) / StartHeight);
  }

  for (int i = 0 ; i < BallCount ; i++) {
    setPixel(Position[i],colors[i][0],colors[i][1],colors[i][2]);
  }

  showStrip();
  setAll(0,0,0);
}

void fireEffect() {

  int Cooling = 55;
  int Sparking = 120;
  int SpeedDelay = 1;
  static byte heat[FIRE_SIZE];
  int cooldown;

  // Step 1.  Cool down every cell a little
  for( int i = 0; i < FIRE_SIZE; i++) {
    cooldown = random(0, ((Cooling * 10) / FIRE_SIZE) + 2);

    if(cooldown>heat[i]) {
      heat[i]=0;
    } else {
      heat[i]=heat[i]-cooldown;
    }
  }

  // Step 2.  Heat from each cell drifts 'up' and diffuses a little
  for( int k= FIRE_SIZE - 1; k >= 2; k--) {
    heat[k] = (heat[k - 1] + heat[k - 2] + heat[k - 2]) / 3;
  }

  // Step 3.  Randomly ignite new 'sparks' near the bottom
  if( random(255) < Sparking ) {
    int y = random(7);
    heat[y] = heat[y] + random(160,255);
    //heat[y] = random(160,255);
  }

  // Step 4.  Convert heat to LED colors
  for( int j = 0; j < FIRE_SIZE; j++) {
    setPixelHeatColor(j, heat[j] );
  }

  showStrip();
  delay(SpeedDelay);
}

void fastCircLoop() {
  uint8_t beatval = beatsin8(10,5,50);
  fastCircEffect();
  delay(beatval*2);
}

void fastCircEffect() {                                            // Fast Circle
  fcCount = (fcCount + fcDir)%NUM_LEDS;
  leds[fcCount] = CHSV(fcHue, fcSat, fcBright);
  fadeToBlackBy(leds, NUM_LEDS, 224);
}



/* ======================= utilities ================================ */

void showStrip() {
   FastLED.show();
}

void setPixel(int Pixel, byte red, byte green, byte blue) {
  // FastLED
  leds[Pixel].r = red;
  leds[Pixel].g = green;
  leds[Pixel].b = blue;
}

void setAll(byte red, byte green, byte blue) {
  for(int i = 0; i < NUM_LEDS; i++ ) {
    setPixel(i, red, green, blue);
  }
  showStrip();
}

void fillLEDsFromPaletteColors(uint8_t colorIndex) {
  uint8_t beatB = beatsin8(15, 10, 20);
  for (int i = 0; i < NUM_LEDS; i++) {
    leds[i] = ColorFromPalette(currentPalette, colorIndex, 255, currentBlending);
    colorIndex += beatB;
  }
}


void setupRandomPalette() {
  targetPalette = CRGBPalette16(CHSV(random8(), 255, 32), CHSV(random8(), random8(64)+192, 255), CHSV(random8(), 255, 32), CHSV(random8(), 255, 255));
}

void changeMe() {
  uint8_t secondHand = (millis() / 1000) % 30;
  static uint8_t lastSecond = 99;
  if (lastSecond != secondHand) {
    lastSecond = secondHand;
    switch(secondHand) {
      case  0: thisdelay = 50; colours[0] = 0xffffff; numcolours=1; boolcolours=0; maxbar = 1; break;          // Just white twinkles
      case  5: thisdelay = 20; colours[1] = 0xff0000; numcolours=2; boolcolours=0; maxbar = 4; break;          // Add red and make bars
      case 15: thisdelay = 50; boolcolours=1; maxbar=1; break;                                                 // A pile of colours, 1 pixel in length
      case 20: thisdelay = 50; fadeval = 128; break;                                                           // Slow down the fade
      case 25: thisdelay = 50; colours[2]= 0x0000ff; boolcolours=0; numcolours=3; fadeval = 192; maxbar = 6; break;
      case 30: break;
    }
  }
}


void setPixelHeatColor (int Pixel, byte temperature) {
  // Scale 'heat' down from 0-255 to 0-191
  byte t192 = round((temperature/255.0)*191);

  // calculate ramp up from
  byte heatramp = t192 & 0x3F; // 0..63
  heatramp <<= 2; // scale up to 0..252

  // figure out which third of the spectrum we're in:
  if( t192 > 0x80) {                     // hottest
    setPixel(Pixel, 255, 255, heatramp);
  } else if( t192 > 0x40 ) {             // middle
    setPixel(Pixel, 255, heatramp, 0);
  } else {                               // coolest
    setPixel(Pixel, heatramp, 0, 0);
  }
}

void clearVariables() {
  demo = false;
  rainbowMarch = false;
  colorBeat = false;
  lightning = false;
  dotBeat = false;
  popFade = false;
  bounce = false;
  fire = false;
  ripple = false;
  juggle = false;
  fastCirc = false;
  fill_solid(leds, NUM_LEDS, CRGB::Black);
}

int clear(String command){
    Particle.publish("CLEAR ALL", command);
    clearVariables();
    return 10;
}
