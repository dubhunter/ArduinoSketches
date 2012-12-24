#include "HT1632.h"

#define DATA 2
#define WR   3
#define CS   4
#define CS2  5
#define CS3  6
#define CS4  7

// use this line for single matrix
HT1632LEDMatrix matrix = HT1632LEDMatrix(DATA, WR, CS, CS2, CS3, CS4);

void setup() {
  Serial.begin(9600);
  matrix.begin(HT1632_COMMON_16NMOS);  
//  matrix.fillScreen();
//  delay(500);

}

void loop() {
  matrix.clearScreen();
  // draw some text!
  matrix.setTextSize(1);    // size 1 == 8 pixels high
  matrix.setTextColor(1);   // 'lit' LEDs
  
  
    matrix.setCursor(0, 0);   // start at top left, with one pixel of spacing
    matrix.print("ABCDEFGH");
    matrix.print("IJKLMNOP");
    matrix.setCursor(0, 8);   // next line, 8 pixels down
    matrix.print("QRSTUVWX");
    matrix.print("YZ123456");
    matrix.writeScreen();
  
  for (;;) ;

  // whew!
}
