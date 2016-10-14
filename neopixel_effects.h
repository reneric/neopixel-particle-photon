/* ======================= includes ================================= */

#include "neopixel/neopixel.h"
#include <math.h>



/* ======================= variables ================================ */

int led = D7;
bool bounce = false;
bool sparkle = false;
bool twinkle = false;
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
void clearVariables();
void showStrip();
uint32_t Wheel(byte WheelPos);



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
}

int setBounce(bool b){
    bounce = b;
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

