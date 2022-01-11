#include <Adafruit_Protomatter.h>
#include <Fonts/FreeMono9pt7b.h> // Large friendly font

#define HEIGHT  32 // Matrix height (pixels)
#define WIDTH   64 // Matrix width (pixels)
uint8_t addrPins[] = {17, 18, 19, 20};
uint8_t rgbPins[]  = {7, 8, 9, 10, 11, 12};
uint8_t clockPin   = 14;
uint8_t latchPin   = 15;
uint8_t oePin      = 16;

Adafruit_Protomatter matrix(
  WIDTH,          // Matrix width in pixels
  5,           // Bit depth -- 6 here provides maximum color options
  1, rgbPins,  // # of matrix chains, array of 6 RGB pins for each
  4, addrPins, // # of address pins (height is inferred), array of pins
  clockPin, latchPin, oePin, // Other matrix control pins
  true);       // HERE IS THE MAGIG FOR DOUBLE-BUFFERING!

int16_t  textX = matrix.width(), // Current text position (X)
         textY,                  // Current text position (Y)
         textMin,                // Text pos. (X) when scrolled off left edge
         hue = 0;
char     str[50];                // Buffer to hold scrolling message text

void setup() {
  // put your setup code here, to run once:

  Serial.begin(9600);

  // Initialize matrix...
  ProtomatterStatus status = matrix.begin();
  Serial.print("Protomatter begin() status: ");
  Serial.println((int)status);
  if(status != PROTOMATTER_OK) {
    // DO NOT CONTINUE if matrix setup encountered an error.
    for(;;);
  }

  // Unlike the "simple" example, we don't do any drawing in setup().
  // But we DO initialize some things we plan to animate...

  // Set up the scrolling message...
  sprintf(str, "Inari Medical Automation",
    matrix.width(), matrix.height());
  matrix.setFont(&FreeMono9pt7b); // Use nice bitmap font
  matrix.setTextWrap(false);           // Allow text off edge
  matrix.setTextColor(0x88FE);         // Purple
  int16_t  x1, y1;
  uint16_t w, h;
  matrix.getTextBounds(str, 0, 0, &x1, &y1, &w, &h); // How big is it?
  textMin = -w; // All text is off left edge when it reaches this point
  textY = matrix.height() + (y1 + h / 2); // Center text vertically
  // Note: when making scrolling text like this, the setTextWrap(false)
  // call is REQUIRED (to allow text to go off the edge of the matrix),
  // AND it must be BEFORE the getTextBounds() call (or else that will
  // return the bounds of "wrapped" text).

}

void loop() {
  // put your main code here, to run repeatedly:

  // Every frame, we clear the background and draw everything anew.
  // This happens "in the background" with double buffering, that's
  // why you don't see everything flicker. It requires double the RAM,
  // so it's not practical for every situation.

  matrix.fillScreen(0); // Fill background black

  // Draw the big scrolly text
  matrix.setCursor(textX, textY);
  matrix.print(str);

  // Update text position for next frame. If text goes off the
  // left edge, reset its position to be off the right edge.
  if((--textX) < textMin) textX = matrix.width();

  // AFTER DRAWING, A show() CALL IS REQUIRED TO UPDATE THE MATRIX!

  matrix.show();

  delay(20); // 20 milliseconds = ~50 frames/second

}
