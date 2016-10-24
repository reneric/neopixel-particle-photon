/* ======================= includes ================================= */

#include "neopixel/neopixel.h"
#include <math.h>



/* ======================= variables ================================ */

int led = D7;
bool bounce = false;
bool sparkle = false;
bool twinkle = false;
bool eyes = false;
bool strobeOn = false;
bool fireOn = false;
bool running = false;
bool kitt = false;
float Gravity = -9.81;
int StartHeight = 1;
float ImpactVelocityStart = sqrt( -2 * Gravity * StartHeight );
float Height[BallCount];
float ImpactVelocity[BallCount];
float TimeSinceLastBounce[BallCount];
int   Position[BallCount];
long  ClockTimeSinceLastBounce[BallCount];
float Dampening[BallCount];

// Bouncing ball colors
byte colors[BallCount][3] = { {0xff, 0,0},        // red
                              {0, 0xff, 0},       // green
                              {0, 0, 0xff},       // blue
                              {0xff, 0xff, 0xff}, // white
                              {0, 0xff, 0xff},    // teal
                              {0xff, 0xff, 0} };  // yellow



/* ======================= prototypes =============================== */

void colorAll(uint32_t c, uint8_t wait);
void colorWipe(uint32_t c, uint8_t wait);
void bouncingColoredBalls();
void rainbow(uint8_t wait);
void rainbowCycle(uint8_t wait);
void Sparkle(byte red, byte green, byte blue, int SpeedDelay);
void setPixel(int Pixel, byte red, byte green, byte blue);
void setAll(byte red, byte green, byte blue);
int setBounce(bool bounce);
int setSparkle(bool sparkle);
int setTwinkle(bool twinkle);
int setStrobe(bool strobe);
int setEyes(bool eyes);
int setFire(bool fire);
int setRunning(bool running);
int setKitt(bool kitt);
void clearVariables();
void showStrip();
uint32_t Wheel(byte WheelPos);
void strobe(byte red, byte green, byte blue, int StrobeCount, int FlashDelay, int EndPause);
void halloweenEyes(byte red, byte green, byte blue,
                   int EyeWidth, int EyeSpace,
                   boolean Fade, int Steps, int FadeDelay,
                   int EndPause);
void fire(int Cooling, int Sparking, int SpeedDelay);
void setPixelHeatColor (int Pixel, byte temperature);
void runningLights(byte red, byte green, byte blue, int WaveDelay);
void centerToOutside(byte red, byte green, byte blue, int EyeSize, int SpeedDelay, int ReturnDelay);
void outsideToCenter(byte red, byte green, byte blue, int EyeSize, int SpeedDelay, int ReturnDelay);
void leftToRight(byte red, byte green, byte blue, int EyeSize, int SpeedDelay, int ReturnDelay);
void rightToLeft(byte red, byte green, byte blue, int EyeSize, int SpeedDelay, int ReturnDelay);
void newKITT(byte red, byte green, byte blue, int EyeSize, int SpeedDelay, int ReturnDelay);



/* ======================= initialize ================================ */

// Parameter 1 = number of pixels in strip
// Parameter 2 = pin number (most are valid)
// Parameter 3 = pixel type [ WS2812, WS2812B, WS2811, TM1803 ]
Adafruit_NeoPixel strip = Adafruit_NeoPixel(PIXEL_COUNT, PIXEL_PIN, PIXEL_TYPE);



/* ========================= effects =============================== */

int color(String command){
    digitalWrite(led, LOW);
    clearVariables();

    Particle.publish("COLOR", command);
    if(command == "red"){
        digitalWrite(led, HIGH);
        colorWipe(strip.Color(255, 0, 0), 50); // Red
    }else if(command == "blue"){
        digitalWrite(led, HIGH);
        colorWipe(strip.Color(0, 0, 255), 50);
    }else if(command == "green"){
        digitalWrite(led, HIGH);
        colorWipe(strip.Color(0, 255, 0), 50);
    }else if(command == "rainbow"){
        digitalWrite(led, HIGH);
        rainbow(20);
    }else if(command == "colorbounce"){
        digitalWrite(led, HIGH);
        for (int i = 0; i < BallCount ; i++) {
          ClockTimeSinceLastBounce[i] = millis();
          Height[i] = StartHeight;
          Position[i] = 0;
          ImpactVelocity[i] = ImpactVelocityStart;
          TimeSinceLastBounce[i] = 0;
          Dampening[i] = 0.90 - float(i)/pow(BallCount,2);
        }
        setBounce(true);
    }else if(command == "sparkle"){
         digitalWrite(led, HIGH);
         setSparkle(true);
    }else if(command == "twinkle"){
         digitalWrite(led, HIGH);
         setTwinkle(true);
    }else if(command == "strobe"){
         digitalWrite(led, HIGH);
         setStrobe(true);
    }else if(command == "eyes"){
         digitalWrite(led, HIGH);
         setEyes(true);
    }else if(command == "fire"){
         digitalWrite(led, HIGH);
         setFire(true);
    }else if(command == "running"){
         digitalWrite(led, HIGH);
         setRunning(true);
    }else if(command == "running"){
         digitalWrite(led, HIGH);
         setRunning(true);
    }
    return 1;

}

void twinkleRandom(int Count, int SpeedDelay, boolean OnlyOne) {
  setAll(0,0,0);

  for (int i=0; i<Count; i++) {
     setPixel(random(PIXEL_COUNT),random(0,255),random(0,255),random(0,255));
     showStrip();
     delay(SpeedDelay);
     if(OnlyOne) {
       setAll(0,0,0);
     }
   }

  delay(SpeedDelay);
}

void sparkleDisplay(byte red, byte green, byte blue, int SpeedDelay) {
  int Pixel = random(PIXEL_COUNT);
  setPixel(Pixel,red,green,blue);
  showStrip();
  delay(SpeedDelay);
  setPixel(Pixel,0,0,0);
}

void rainbow(uint8_t wait) {
  uint16_t i, j;

  for(j=0; j<256; j++) {
    for(i=0; i<strip.numPixels(); i++) {
      strip.setPixelColor(i, Wheel((i+j) & 255));
    }
    strip.show();
    delay(wait);
  }
}

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
      Position[i] = round( Height[i] * (PIXEL_COUNT - 1) / StartHeight);
    }

    for (int i = 0 ; i < BallCount ; i++) {
      setPixel(Position[i],colors[i][0],colors[i][1],colors[i][2]);
    }

    showStrip();
    setAll(0,0,0);
}


void strobe(byte red, byte green, byte blue, int StrobeCount, int FlashDelay, int EndPause){
  for(int j = 0; j < StrobeCount; j++) {
    setAll(red,green,blue);
    showStrip();
    delay(FlashDelay);
    setAll(0,0,0);
    showStrip();
    delay(FlashDelay);
  }

 delay(EndPause);
}

void halloweenEyes(byte red, byte green, byte blue,
                   int EyeWidth, int EyeSpace,
                   boolean Fade, int Steps, int FadeDelay,
                   int EndPause){
  randomSeed(analogRead(0));

  int i;
  int StartPoint  = random( 0, PIXEL_COUNT - (2*EyeWidth) - EyeSpace );
  int Start2ndEye = StartPoint + EyeWidth + EyeSpace;

  for(i = 0; i < EyeWidth; i++) {
    setPixel(StartPoint + i, red, green, blue);
    setPixel(Start2ndEye + i, red, green, blue);
  }

  showStrip();

  if(Fade==true) {
    float r, g, b;

    for(int j = Steps; j >= 0; j--) {
      r = j*(red/Steps);
      g = j*(green/Steps);
      b = j*(blue/Steps);

      for(i = 0; i < EyeWidth; i++) {
        setPixel(StartPoint + i, r, g, b);
        setPixel(Start2ndEye + i, r, g, b);
      }

      showStrip();
      delay(FadeDelay);
    }
  }

  setAll(0,0,0); // Set all black

  delay(EndPause);
}

void fire(int Cooling, int Sparking, int SpeedDelay) {
  static byte heat[PIXEL_COUNT];
  int cooldown;

  // Step 1.  Cool down every cell a little
  for( int i = 0; i < PIXEL_COUNT; i++) {
    cooldown = random(0, ((Cooling * 10) / PIXEL_COUNT) + 2);

    if(cooldown>heat[i]) {
      heat[i]=0;
    } else {
      heat[i]=heat[i]-cooldown;
    }
  }

  // Step 2.  Heat from each cell drifts 'up' and diffuses a little
  for( int k= PIXEL_COUNT - 1; k >= 2; k--) {
    heat[k] = (heat[k - 1] + heat[k - 2] + heat[k - 2]) / 3;
  }

  // Step 3.  Randomly ignite new 'sparks' near the bottom
  if( random(255) < Sparking ) {
    int y = random(7);
    heat[y] = heat[y] + random(160,255);
    //heat[y] = random(160,255);
  }

  // Step 4.  Convert heat to LED colors
  for( int j = 0; j < PIXEL_COUNT; j++) {
    setPixelHeatColor(j, heat[j] );
  }

  showStrip();
  delay(SpeedDelay);
}

void runningLights(byte red, byte green, byte blue, int WaveDelay) {
  int Position=0;

  for(int i=0; i<PIXEL_COUNT*2; i++)
  {
      Position++; // = 0; //Position + Rate;
      for(int i=0; i<PIXEL_COUNT; i++) {
        // sine wave, 3 offset waves make a rainbow!
        //float level = sin(i+Position) * 127 + 128;
        //setPixel(i,level,0,0);
        //float level = sin(i+Position) * 127 + 128;
        setPixel(i,((sin(i+Position) * 127 + 128)/255)*red,
                   ((sin(i+Position) * 127 + 128)/255)*green,
                   ((sin(i+Position) * 127 + 128)/255)*blue);
      }

      showStrip();
      delay(WaveDelay);
  }
}

void newKITT(byte red, byte green, byte blue, int EyeSize, int SpeedDelay, int ReturnDelay){
  rightToLeft(red, green, blue, EyeSize, SpeedDelay, ReturnDelay);
  leftToRight(red, green, blue, EyeSize, SpeedDelay, ReturnDelay);
  outsideToCenter(red, green, blue, EyeSize, SpeedDelay, ReturnDelay);
  centerToOutside(red, green, blue, EyeSize, SpeedDelay, ReturnDelay);
  leftToRight(red, green, blue, EyeSize, SpeedDelay, ReturnDelay);
  rightToLeft(red, green, blue, EyeSize, SpeedDelay, ReturnDelay);
  outsideToCenter(red, green, blue, EyeSize, SpeedDelay, ReturnDelay);
  centerToOutside(red, green, blue, EyeSize, SpeedDelay, ReturnDelay);
}



/* ======================= utilities ================================ */

void showStrip() {
 strip.show();
}

void setPixel(int Pixel, byte red, byte green, byte blue) {
 strip.setPixelColor(Pixel, strip.Color(red, green, blue));
}

void setAll(byte red, byte green, byte blue) {
  for(int i = 0; i < PIXEL_COUNT; i++ ) {
    setPixel(i, red, green, blue);
  }
  showStrip();
}

uint32_t Wheel(byte WheelPos) {
  if(WheelPos < 85) {
   return strip.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
  } else if(WheelPos < 170) {
   WheelPos -= 85;
   return strip.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  } else {
   WheelPos -= 170;
   return strip.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
}

int clear(String command){
    Particle.publish("CLEAR ALL", command);
    clearVariables();
    digitalWrite(led, LOW);
    colorWipe(strip.Color(0, 0, 0), 50);
    return 10;
}

void clearVariables() {
  setBounce(false);
  setSparkle(false);
  setTwinkle(false);
  setStrobe(false);
  setEyes(false);
  setFire(false);
  setRunning(false);
  setKitt(false);
}

int setBounce(bool b){
    bounce = b;
    return 10;
}

int setStrobe(bool s){
    strobeOn = s;
    return 10;
}

int setKitt(bool s){
    kitt = s;
    return 10;
}

int setEyes(bool s){
    eyes = s;
    return 10;
}

int setRunning(bool s){
    running = s;
    return 10;
}

int setSparkle(bool s){
    sparkle = s;
    return 10;
}

int setTwinkle(bool s){
    twinkle = s;
    return 10;
}

int setFire(bool s){
    fireOn = s;
    return 10;
}

void colorAll(uint32_t c, uint8_t wait) {
  uint16_t i;

  for(i=0; i<strip.numPixels(); i++) {
    strip.setPixelColor(i, c);
  }
  strip.show();
  delay(wait);
}

void colorWipe(uint32_t c, uint8_t wait) {
  for(uint16_t i=0; i<strip.numPixels(); i++) {
    strip.setPixelColor(i, c);
    strip.show();
    delay(wait);
  }
}

void rainbowCycle(uint8_t wait) {
  uint16_t i, j;

  for(j=0; j<256; j++) { // 1 cycle of all colors on wheel
    for(i=0; i< strip.numPixels(); i++) {
      strip.setPixelColor(i, Wheel(((i * 256 / strip.numPixels()) + j) & 255));
    }
    strip.show();
    delay(wait);
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

void centerToOutside(byte red, byte green, byte blue, int EyeSize, int SpeedDelay, int ReturnDelay) {
  for(int i =((NUM_LEDS-EyeSize)/2); i>=0; i--) {
    setAll(0,0,0);

    setPixel(i, red/10, green/10, blue/10);
    for(int j = 1; j <= EyeSize; j++) {
      setPixel(i+j, red, green, blue);
    }
    setPixel(i+EyeSize+1, red/10, green/10, blue/10);

    setPixel(NUM_LEDS-i, red/10, green/10, blue/10);
    for(int j = 1; j <= EyeSize; j++) {
      setPixel(NUM_LEDS-i-j, red, green, blue);
    }
    setPixel(NUM_LEDS-i-EyeSize-1, red/10, green/10, blue/10);

    showStrip();
    delay(SpeedDelay);
  }
  delay(ReturnDelay);
}

void outsideToCenter(byte red, byte green, byte blue, int EyeSize, int SpeedDelay, int ReturnDelay) {
  for(int i = 0; i<=((NUM_LEDS-EyeSize)/2); i++) {
    setAll(0,0,0);

    setPixel(i, red/10, green/10, blue/10);
    for(int j = 1; j <= EyeSize; j++) {
      setPixel(i+j, red, green, blue);
    }
    setPixel(i+EyeSize+1, red/10, green/10, blue/10);

    setPixel(NUM_LEDS-i, red/10, green/10, blue/10);
    for(int j = 1; j <= EyeSize; j++) {
      setPixel(NUM_LEDS-i-j, red, green, blue);
    }
    setPixel(NUM_LEDS-i-EyeSize-1, red/10, green/10, blue/10);

    showStrip();
    delay(SpeedDelay);
  }
  delay(ReturnDelay);
}

void leftToRight(byte red, byte green, byte blue, int EyeSize, int SpeedDelay, int ReturnDelay) {
  for(int i = 0; i < NUM_LEDS-EyeSize-2; i++) {
    setAll(0,0,0);
    setPixel(i, red/10, green/10, blue/10);
    for(int j = 1; j <= EyeSize; j++) {
      setPixel(i+j, red, green, blue);
    }
    setPixel(i+EyeSize+1, red/10, green/10, blue/10);
    showStrip();
    delay(SpeedDelay);
  }
  delay(ReturnDelay);
}

void rightToLeft(byte red, byte green, byte blue, int EyeSize, int SpeedDelay, int ReturnDelay) {
  for(int i = NUM_LEDS-EyeSize-2; i > 0; i--) {
    setAll(0,0,0);
    setPixel(i, red/10, green/10, blue/10);
    for(int j = 1; j <= EyeSize; j++) {
      setPixel(i+j, red, green, blue);
    }
    setPixel(i+EyeSize+1, red/10, green/10, blue/10);
    showStrip();
    delay(SpeedDelay);
  }
  delay(ReturnDelay);
}
