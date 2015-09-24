// Adafruit_NeoMatrix example for single NeoPixel Shield.
// Scrolls 'Howdy' across the matrix in a portrait (vertical) orientation.

#include <Adafruit_GFX.h>
#include <Adafruit_NeoPixel.h>
#include <Adafruit_NeoMatrix.h>
#include <gamma.h>

#ifndef PSTR
 #define PSTR // Make Arduino Due happy
#endif

#define PIN D6

// MATRIX DECLARATION:
// Parameter 1 = width of NeoPixel matrix
// Parameter 2 = height of matrix
// Parameter 3 = pin number (most are valid)
// Parameter 4 = matrix layout flags, add together as needed:
//   NEO_MATRIX_TOP, NEO_MATRIX_BOTTOM, NEO_MATRIX_LEFT, NEO_MATRIX_RIGHT:
//     Position of the FIRST LED in the matrix; pick two, e.g.
//     NEO_MATRIX_TOP + NEO_MATRIX_LEFT for the top-left corner.
//   NEO_MATRIX_ROWS, NEO_MATRIX_COLUMNS: LEDs are arranged in horizontal
//     rows or in vertical columns, respectively; pick one or the other.
//   NEO_MATRIX_PROGRESSIVE, NEO_MATRIX_ZIGZAG: all rows/columns proceed
//     in the same order, or alternate lines reverse direction; pick one.
//   See example below for these values in action.
// Parameter 5 = pixel type flags, add together as needed:
//   NEO_KHZ800  800 KHz bitstream (most NeoPixel products w/WS2812 LEDs)
//   NEO_KHZ400  400 KHz (classic 'v1' (not v2) FLORA pixels, WS2811 drivers)
//   NEO_GRB     Pixels are wired for GRB bitstream (most NeoPixel products)
//   NEO_RGB     Pixels are wired for RGB bitstream (v1 FLORA pixels, not v2)


// Example for NeoPixel Shield.  In this application we'd like to use it
// as a 5x8 tall matrix, with the USB port positioned at the top of the
// Arduino.  When held that way, the first pixel is at the top right, and
// lines are arranged in columns, progressive order.  The shield uses
// 800 KHz (v2) pixels that expect GRB color data.
Adafruit_NeoMatrix neoMatrix = Adafruit_NeoMatrix(8, 8, 4, 1, PIN, // 8x8 panel, 4 panels horizontal, 1 panel high
  NEO_MATRIX_TOP    + NEO_MATRIX_LEFT +
  NEO_MATRIX_ROWS + NEO_MATRIX_PROGRESSIVE,
  NEO_GRB            + NEO_KHZ800);

const uint16_t colors[] = {neoMatrix.Color(255, 0, 0), neoMatrix.Color(0, 255, 0), neoMatrix.Color(0, 0, 255) };

void NeoPixelInit() {
  Serial.println("neo init");
  neoMatrix.begin();
  neoMatrix.setTextWrap(false);
  neoMatrix.setBrightness(20);
  neoMatrix.setTextColor(colors[0]);
}

void DisplayText(const char * text){
  int colour = 0;
  neoMatrix.setCursor(0, 0);
  neoMatrix.setTextColor(colors[1]);
  neoMatrix.print((text));
  neoMatrix.show();  
}

void ScrollText(String text) {
  int colour = 1;
  int x    = neoMatrix.width();
  int pass = 0;
  if (colour > 2) {colour = 0;}
  neoMatrix.setTextColor(colors[colour]);

      Serial.println(String(x));

  int len = (text.length() + 3)* 5;

  while (--x > -len) {
    Serial.println(String(x));
    neoMatrix.fillScreen(0);
    neoMatrix.setCursor(x, 0);
    neoMatrix.print(text);
    neoMatrix.show();
    delay(100);
  }
}

void ScrollText() {
  int colour = 0;
  int x    = neoMatrix.width();
  int pass = 0;
  if (colour > 2) {colour = 0;}
  neoMatrix.setTextColor(colors[colour]);

      Serial.println(String(x));

  while (--x > -55) {
    Serial.println(String(x));
    neoMatrix.fillScreen(0);
    neoMatrix.setCursor(x, 0);
    neoMatrix.print(F("Maker Den"));
    neoMatrix.show();
    delay(100);
  }
}


