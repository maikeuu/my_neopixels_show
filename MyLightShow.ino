/**
 * Name: Mike Lin Chu
 * Date: December 2, 2017
 * Program: Creating my own light show by messing around with Adafruit libraries
 * and seeing what kind of patterns I should make.
 */


// IMPORTANT: To reduce NeoPixel burnout risk, add 1000 uF capacitor across
// pixel power leads, add 300 - 500 Ohm resistor on first pixel's data input
// and minimize distance between Arduino and first pixel.  Avoid connecting
// on a live circuit...if you must, connect GND first.

#include <Adafruit_NeoPixel.h>
#include <stdlib.h>
#include <Chrono.h>
#ifdef __AVR__ //Dunno what this does
  #include <avr/power.h>
#endif

#define PIN 6
extern const uint8_t gamma8[]; //used for declaring the tabel at the bottom of this program

class foo : public Adafruit_NeoPixel {

   void setPixelColor(uint16_t n, uint8_t r, uint8_t g, uint8_t b) {
     this -> setPixelColor(n, 
     pgm_read_byte(&gamma8[r]),
     pgm_read_byte(&gamma8[g]),
     pgm_read_byte(&gamma8[b]));
   }
  
  
};



// Parameter 1 = number of pixels in strip
// Parameter 2 = Arduino pin number (most are valid)
// Parameter 3 = pixel type flags, add together as needed:
//   NEO_KHZ800  800 KHz bitstream (most NeoPixel products w/WS2812 LEDs)
//   NEO_KHZ400  400 KHz (classic 'v1' (not v2) FLORA pixels, WS2811 drivers)
//   NEO_GRB     Pixels are wired for GRB bitstream (most NeoPixel products)
//   NEO_RGB     Pixels are wired for RGB bitstream (v1 FLORA pixels, not v2)
//   NEO_RGBW    Pixels are wired for RGBW bitstream (NeoPixel RGBW products)
Adafruit_NeoPixel strip = Adafruit_NeoPixel(150, PIN, NEO_GRB + NEO_KHZ800);


//COLORS
//RGB = (R*65536)+(G*256)+B , (when R is RED, G is GREEN and B is BLUE)
uint32_t red = strip.Color(255, 0, 0);
uint32_t orange = strip.Color(255, 128, 0);
uint32_t yellow = strip.Color(255, 255, 0);
uint32_t green = strip.Color(0, 255, 0);
uint32_t blue = strip.Color(0, 0, 255);
uint32_t purple = strip.Color(153, 51, 255);
uint32_t dark_purple = strip.Color(51, 0, 102);
uint32_t teal = strip.Color(0, 255, 255);
uint32_t black = strip.Color(0, 0, 0);
uint32_t white = strip.Color(255, 255, 255);

//Bools to turn on and off shows in animationController
//RANDOM COLOR SHOWS
bool randomShow = false;  //Changes random pixels to random colors
bool randomFade = true;
//RAINBOW SHOWS
bool rainbowCycleShow = true;  //Cycle through the rainbow in segments 
bool rainbowStripShow = true; //Simultaneously cycle all leds through all 256 colors 
bool rainbowChaseShow = true;  //Cycle through the rainbow by turning every third pixel off creating chase effect
bool rainbowPulseShow = true;
//SPLIT LED SHOWS
bool splitShineShow = true;
bool splitChaseShow = true;
bool splitFlashShow = true;

//TODO: FIGURE OUT HOW TO IMPLEMENT TIMER PROGRAM
//Timers
Chrono myTimer(Chrono::SECONDS); //http://sofapirate.github.io/Chrono/
bool programOn = true;  //Bools to turn on and off animation controller based off of elapsed time


/* SET UP */

void setup() {
  strip.begin();
  strip.setBrightness(100);
  strip.show();
}

void loop() {
  animationController();

}






//Handles which shows to turn on based off of global boolean variable conditions for each show
void animationController() {
  if (programOn) {
    //RANDOM SHOWS
    if (randomShow) {
      randomize(false);
    }
    if (randomFade) {
      randomFadeShow(5);
    }
    //RAINBOW BASED SHOWS
    if (rainbowStripShow) {
      int count = 0;
      while (count < 5) {
        colorCycle(100);
        count++;
      }
    }
    if (rainbowCycleShow) {
      rainbowCycle(100);
    }
    if (rainbowChaseShow) {
      theaterChaseRainbow(50);
    }
    if (rainbowPulseShow) {
      rainbowPulse(5, 8, 20, true);
      rainbowPulse(5, 16, 20, false);
    }
    if (randomFade) {
      randomFadeShow(8);
    }
    //split shows
    if (splitShineShow) {
      int count = 0;
      while (count < 150) {
        splitShine(6, 20);
        count++;
      }
    }
  }
}
//////////////////////////////////
//////////////////////////////////
//    /* RAINBOW SHOWS */
//////////////////////////////////
//////////////////////////////////


// Slightly different, this makes the rainbow equally distributed throughout
void rainbowCycle(uint16_t wait) {
  uint16_t i, j;
  for(j=0; j<256*5; j++) { // 5 cycles of all colors on wheel
    for(i=0; i< strip.numPixels(); i++) {
      strip.setPixelColor(i, Wheel(((i * 256 / strip.numPixels()) + j) & 255));
    }
    strip.show();
    delay(wait);
  }
}

void theaterChaseRainbow(uint8_t wait) {
  for (int j=0; j < 256; j++) {     // cycle all 256 colors in the wheel
    for (int q=0; q < 3; q++) {
      for (uint16_t i=0; i < strip.numPixels(); i=i+3) {
        strip.setPixelColor(i+q, Wheel( (i+j) % 255));    //turn every third pixel on
      }
      strip.show();
      delay(wait);
      for (uint16_t i=0; i < strip.numPixels(); i=i+3) {
        strip.setPixelColor(i+q, 0);        //turn every third pixel off
      }
    }
  }
}

//Creates a "pulse" which travels throughout the LED strip
void rainbowPulse(uint8_t cycles, uint8_t pulseLength, uint8_t wait, bool fadeOn) {
  for (int currentCycle = 0; currentCycle < cycles; currentCycle++) {

    int red = rand() % 255 + 1;
    int green = rand() % 255 + 1;
    int blue = rand() % 255 + 1;
    uint32_t color = strip.Color(red, green, blue);

    int r[pulseLength], g[pulseLength], b[pulseLength]  = {}; 
    for (int i = pulseLength - 1; i >= 0; i--) {
      if (fadeOn) {
        // Initialize rgb arrays such that the first index has the weakest brightness,
        // while the last index has the dimmest brightness
        if (i == pulseLength - 1) {
          r[i] = red;
          g[i] = green;
          b[i] = blue;
        } else {
          r[i] = r[i+1]/2;
          g[i] = g[i+1]/2;
          b[i] = b[i+1]/2;
        }
      }
      else { //fade off
        r[i] = red;
        g[i] = green;
        b[i] = blue;
      }
    }

    for (int i = 0; i < strip.numPixels(); i++) {
      for (int pulseLED = 0; pulseLED < pulseLength; pulseLED++) {
        color = strip.Color(r[pulseLED], g[pulseLED], b[pulseLED]);
        strip.setPixelColor(i + pulseLED, color);
      }
      strip.show();
      delay(wait);
      //Once the pulse has been shown, set i to be black for the next iteration
      //Has the effect of keeping the pulse the same length throughout the strip
      strip.setPixelColor(i, black);
    }
  }
}

//Cycle through all 255 colors in the wheel
void colorCycle(uint16_t wait) {
  for (int j = 0; j < 255; j++) {
    for (int i = 0; i < strip.numPixels(); i++) {
      strip.setPixelColor(i, Wheel((j) % 255));
    }
    strip.show();
    delay(wait);
  }
}

//////////////////////////////////
//////////////////////////////////
// /* RNG-BASED SHOWS */
//////////////////////////////////
//////////////////////////////////

//TODO: DOCUMENT
//TODO: FIGURE OUT FORMULA FOR HOW TO SLOWLY TRANSITION FADES
void randomFadeShow(uint8_t numCycles) {
  for (int curr = 0; curr < numCycles; curr++) {
    setColor(black);
    delay(100);
    // AMOUNT OF TIME THIS FUNCTION TAKES TO RUN 
    // EQUIVALENT TO INTENSITY MODIFIER * DELAY(X)
    //Initialize Colors
    int red = rand()% 255 + 1;
    int green = rand()% 255 + 1;
    int blue = rand()% 255 + 1;
    uint32_t originalColor = strip.Color(red, green, blue);
    
    int r, g, b = 0;
    uint32_t currentColor = strip.Color(r, g, b);
    //Constants
    const int intensityModifer = 40; //This is the number of cycles we expect to see before the light completely dissipates to black
    //These constants are the proportions to reduce their respective color values
    int redProportion = red/intensityModifer; 
    int greenProportion = green/intensityModifer;
    int blueProportion = blue/intensityModifer;
    //Fade to original color
    while (r != red && b != blue && g != green) {
      setColor(currentColor);
      if ((r + redProportion) <= red ) {
        r += redProportion;
      } else r = red;
      if ((g + greenProportion) <= green) {
        g += greenProportion;
      } else g = green;
      if ((b + blueProportion) <= blue) {
        b += blueProportion;
      } else b = blue;
      currentColor = strip.Color(r, g, b); //reduce the color for the next cycle
      delay(100);
    }
    delay(100);
    setColor(currentColor);
    //Fade to black
    while (r > 0 || b > 0 || g > 0) {
      setColor(currentColor);
      if ((r - redProportion) >= 0 ) {
        r -= redProportion;
      } else r = 0;
      if ((g - greenProportion) >= 0) {
        g -= greenProportion;
      } else g = 0;
      if ((b - blueProportion) >= 0) {
        b -= blueProportion;
      } else b = 0;
      currentColor = strip.Color(r, g, b); //reduce the color for the next cycle
      delay(100);
    }
  setColor(black);
  delay(100);
  }
}

//Make strip be random colors
void randomize(bool turnOff) {
  for (int i = 0; i < 1000; i++) {
    uint32_t pixel = rand() % strip.numPixels() + 1;
    uint32_t color = rand() % 255 + 1;
    strip.setPixelColor(pixel, Wheel(color));
    if (turnOff) {
      uint32_t pixel2 = rand() % 150 + 1;
      strip.setPixelColor(pixel2, black);
    }
  strip.show();
  delay(100);
  }
}

//////////////////////////////////
//////////////////////////////////
//  /* LED SEGMENT-SPLIT SHOWS */
//////////////////////////////////
//////////////////////////////////

//Segments are split into half and run different colors back and forth to meet in the middle
void splitShine(uint8_t numHalves, uint8_t wait) {
  const int segmentLength = strip.numPixels()/numHalves;   
  int forwardColors [numHalves] = {}; //stores color to be used for each forward Segment
  int backColors [numHalves] = {}; //stores colors to be used for each back segment
  int segmentsArray[numHalves] = {}; //used to store starting position of each new segment
  //initialize arrays
  for (int i = 0; i < numHalves; i++) {
    segmentsArray[i] = i * segmentLength;
    forwardColors[i] = colorCorrect(randomColor());
    backColors[i] = colorCorrect(randomColor());
  }
  for (int LED = 0; LED <= segmentLength/2; LED++) { // 0 to 29, 30 to 59, 60 to 89, 90 to 119, 120 to 149
    for (int segment = 0; segment < sizeof(segmentsArray); segment++) { // {0, 30, 60, 90, 120}
      strip.setPixelColor(segmentsArray[segment] + LED, forwardColors[segment]);
      strip.setPixelColor(segmentLength * (segment+1) - LED, backColors[segment]);
    }
    strip.show();
    delay(wait);
  }
}

//Segments run one way revealing their colors from beginning to end
void splitChase(uint8_t numHalves, uint8_t wait) {
  //Case numHalves = 5
  int colors[numHalves] = {};
  int segmentLength = strip.numPixels()/numHalves;
  int segments[numHalves] = {}; //{30, 60, 90, 120, 150}
  //segmentLength = 150/5 = 30
  //initialize color array
  for (int i = 0; i < numHalves; i++) {
    colors[i] = rand() % 255 + 1;
  }
  //initialize segments array
  for (int i = 0; i < numHalves; i++) {
    segments[i] = i * segmentLength;
  }
  for (int i = 0; i < numHalves; i++) {
    for (int index = 0; index < segmentLength; index++) {
      strip.setPixelColor(index + segments[i], Wheel(colors[i]));
      strip.show();
      delay(wait);
    }
  }
}

//Segments flash new colors per iteration
void splitFlash(uint8_t numHalves, uint8_t wait) {
    //Case numHalves = 5
  int colors[numHalves] = {};
  int segmentLength = strip.numPixels()/numHalves;
  int segments[numHalves] = {}; //{30, 60, 90, 120, 150}
  //segmentLength = 150/5 = 30
  //initialize color array
  for (int i = 0; i < numHalves; i++) {
    colors[i] = rand() % 255 + 1;
  }
  //initialize segments array
  for (int i = 0; i < numHalves; i++) {
    segments[i] = i * segmentLength;
  }
  for (int i = 0; i < numHalves; i++) {
    for (int index = 0; index < segmentLength; index++) {
      strip.setPixelColor(index + segments[i], Wheel(colors[i]));
    }
  }
   strip.show();
   delay(wait*1000);
}

//////////////////////////////////
//////////////////////////////////
//       /* MISC SHOWS */
//////////////////////////////////
//////////////////////////////////

//Theatre-style crawling lights.
void theaterChase(uint32_t c, uint8_t wait) {
  for (int j=0; j<10; j++) {  //do 10 cycles of chasing
    for (int q=0; q < 3; q++) {
      for (uint16_t i=0; i < strip.numPixels(); i=i+3) {
        strip.setPixelColor(i+q, c);    //turn every third pixel on
      }
      strip.show();
      delay(wait);
      for (uint16_t i=0; i < strip.numPixels(); i=i+3) {
        strip.setPixelColor(i+q, 0);        //turn every third pixel off
      }
    }
  }
}

// Fill the dots one after the other with a color
void colorWipe(uint32_t c, uint8_t wait) {
  for(uint16_t i=0; i<strip.numPixels(); i++) {
    strip.setPixelColor(i, c);
    strip.show();
    delay(wait);
  }
}

//Turn on lights in a christmas fashion
void itsChristmas() {
  for (int i = 0; i < strip.numPixels(); i++) {
    if (i % 2 == 1) {
      strip.setPixelColor(i, red);
    }
    else if ( i % 3 == 0) {
      strip.setPixelColor(i, white);
    }
    else {
      strip.setPixelColor(i, green);
    }
    strip.show();
    delay(25);
  }
  delay(1000);
  for (int i = 0; i < strip.numPixels(); i++) {
    strip.setPixelColor(i, black);
  }
  strip.show();
  delay(1000);
}




//////////////////////////////////
//////////////////////////////////
//      /* HELPER METHODS *
//////////////////////////////////
//////////////////////////////////

// Input a value 0 to 255 to get a color value.
// The colours are a transition r - g - b - back to r.
uint32_t Wheel(byte WheelPos) {
  WheelPos = 255 - WheelPos;
  if(WheelPos < 85) {
    return strip.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  }
  if(WheelPos < 170) {
    WheelPos -= 85;
    return strip.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
  WheelPos -= 170;
  return strip.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
}


//Helper function that uses gamma correction to properly display colors
uint32_t colorCorrect(uint32_t c) {
  uint8_t r, g, b;
  r = (uint8_t)(c >> 16);
  g = (uint8_t)(c >> 8);
  b = (uint8_t)c;
  return strip.Color(pgm_read_byte(&gamma8[r]),
                     pgm_read_byte(&gamma8[g]),
                     pgm_read_byte(&gamma8[b]));
}

//Helper method to set color of entire strip to given color
void setColor(uint32_t c) {
  for (uint16_t i = 0; i < strip.numPixels(); i++) {
    strip.setPixelColor(i, c);
    
  }
  strip.show();
}

//Returns a random color with RGB values each ranging from 0 - 255 respectively
uint32_t randomColor() {
  uint8_t red = rand() % 100 + 1;
  return strip.Color(rand() % 255 + 1, rand() % 255 + 1, rand() % 255 + 1);
}


//GAMMA TABLE FOR COLOR CORECTION
const uint8_t PROGMEM gamma8[] = {
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  1,  1,  1,  1,
    1,  1,  1,  1,  1,  1,  1,  1,  1,  2,  2,  2,  2,  2,  2,  2,
    2,  3,  3,  3,  3,  3,  3,  3,  4,  4,  4,  4,  4,  5,  5,  5,
    5,  6,  6,  6,  6,  7,  7,  7,  7,  8,  8,  8,  9,  9,  9, 10,
   10, 10, 11, 11, 11, 12, 12, 13, 13, 13, 14, 14, 15, 15, 16, 16,
   17, 17, 18, 18, 19, 19, 20, 20, 21, 21, 22, 22, 23, 24, 24, 25,
   25, 26, 27, 27, 28, 29, 29, 30, 31, 32, 32, 33, 34, 35, 35, 36,
   37, 38, 39, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 50,
   51, 52, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64, 66, 67, 68,
   69, 70, 72, 73, 74, 75, 77, 78, 79, 81, 82, 83, 85, 86, 87, 89,
   90, 92, 93, 95, 96, 98, 99,101,102,104,105,107,109,110,112,114,
  115,117,119,120,122,124,126,127,129,131,133,135,137,138,140,142,
  144,146,148,150,152,154,156,158,160,162,164,167,169,171,173,175,
  177,180,182,184,186,189,191,193,196,198,200,203,205,208,210,213,
  215,218,220,223,225,228,231,233,236,239,241,244,247,249,252,255 };



