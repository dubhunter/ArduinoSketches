#include <Metro.h>
#include <Adafruit_NeoPixel.h>
#include <Adafruit_GFX.h>
#include <Adafruit_NeoMatrix.h>
#include "RGB.h"

#define PIN_MATRIX 1
#define PIN_CLOUD A1

Adafruit_NeoMatrix matrix = Adafruit_NeoMatrix(8, 8, PIN_MATRIX,
  NEO_MATRIX_TOP + NEO_MATRIX_LEFT + NEO_MATRIX_ROWS + NEO_MATRIX_PROGRESSIVE,
  NEO_GRB + NEO_KHZ800
);

int brightness = 10;
RGB textColor = white;
RGB textBackground = teal;

RGB colors[4] = {
  off,
  red,
  blue,
  white
};

// This 8x8 array represents the LED matrix pixels. 
// A value of 1 means we’ll fade the pixel to white
int board[8][8] = {  
 {0, 0, 3, 0, 0, 3, 0, 0},
 {0, 0, 3, 0, 0, 3, 0, 0},
 {3, 3, 3, 3, 3, 3, 3, 3},
 {0, 0, 3, 0, 0, 3, 0, 0},
 {0, 0, 3, 0, 0, 3, 0, 0},
 {3, 3, 3, 3, 3, 3, 3, 3},
 {0, 0, 3, 0, 0, 3, 0, 0},
 {0, 0, 3, 0, 0, 3, 0, 0}
};

int command = 0;
int playing = 0;
Metro timeout = Metro(300000); // 5 minutes
int flashTimes = 4;
int flashDelay = 100;
int demoDelay = 800;
String demoMsg = "TXT2PLAY 415.319.XOXO";

void setup() {
  Serial.begin(9600);
  Serial.println("Started");
  
  pinMode(PIN_CLOUD, INPUT);
  
  matrix.begin();
  matrix.setBrightness(brightness);
  matrix.setTextColor(matrix.Color(textColor.r, textColor.g, textColor.b));
  matrix.setTextWrap(false);
  
  drawBoard();
  delay(1000);
}

void loop() {
  if (!playing || timeout.check()) {
    demo();
    playing = 0;
  }
  
  command = round(analogRead(1) / 10.24);
  Serial.println(command);
  
  if (command > 10) {
    playing = 1;
    timeout.reset();
  }
}

void demo() {
  drawBoard();
  delay(demoDelay);
  
  makeMove(1, 1);
  drawBoard();
  delay(demoDelay);
  
  makeMove(2, 3);
  drawBoard();
  delay(demoDelay);
  
  makeMove(1, 5);
  drawBoard();
  delay(demoDelay);
  
  makeMove(2, 6);
  drawBoard();
  delay(demoDelay);
  
  makeMove(1, 9);
  drawBoard();
  delay(demoDelay);
  
  for (int i = 0; i < flashTimes; i++ ) {
    fillScreen(off);
    delay(flashDelay);
    drawBoard();
    delay(flashDelay);
  }
  
  clearMoves();
  
  fadeScreen(off, textBackground, 30, 3);
  scrollText(demoMsg);
  scrollText(demoMsg);
}

void drawBoard() {
  for(int row = 0; row < 8; row++) {
    for(int column = 0; column < 8; column++) {
      fillPixel(column, row, colors[board[row][column]], false);
    }
  }
  matrix.show();
}

void makeMove(int player, int cell) {
  switch (cell) {
    case 1:
      board[0][0] = player;
      board[0][1] = player;
      board[1][0] = player;
      board[1][1] = player;
      break;
    case 2:
      board[0][3] = player;
      board[0][4] = player;
      board[1][3] = player;
      board[1][4] = player;
      break;
    case 3:
      board[0][6] = player;
      board[0][7] = player;
      board[1][6] = player;
      board[1][7] = player;
      break;
    case 4:
      board[3][0] = player;
      board[3][1] = player;
      board[4][0] = player;
      board[4][1] = player;
      break;
    case 5:
      board[3][3] = player;
      board[3][4] = player;
      board[4][3] = player;
      board[4][4] = player;
      break;
    case 6:
      board[3][6] = player;
      board[3][7] = player;
      board[4][6] = player;
      board[4][7] = player;
      break;
    case 7:
      board[6][0] = player;
      board[6][1] = player;
      board[7][0] = player;
      board[7][1] = player;
      break;
    case 8:
      board[6][3] = player;
      board[6][4] = player;
      board[7][3] = player;
      board[7][4] = player;
      break;
    case 9:
      board[6][6] = player;
      board[6][7] = player;
      board[7][6] = player;
      board[7][7] = player;
      break;
  }
}

void clearMoves() {
  for (int cell = 1; cell <= 9; cell++) {
    makeMove(0, cell);
  }
}

void fillScreen(RGB color) {
  matrix.fillScreen(matrix.Color(color.r, color.g, color.b));
  matrix.show();
}

// Crossfade entire screen from startColor to endColor
void fadeScreen(RGB startColor, RGB endColor, int steps, int wait) {
  for(int i = 0; i <= steps; i++) {
    RGB newColor = colorStep(startColor, endColor, steps, i);
    fillScreen(newColor);
    delay(wait);
  }
}

void fillPixel(int x, int y, RGB color, int show) {
   matrix.drawPixel(x, y, matrix.Color(color.r, color.g, color.b));
   if (show) {
     matrix.show();
   }
}
  
// Fade pixel (x, y) from startColor to endColor
void fadePixel(int x, int y, RGB startColor, RGB endColor, int steps, int wait) {
  for(int i = 0; i <= steps; i++)  {
    RGB newColor = colorStep(startColor, endColor, steps, i);
    fillPixel(x, y, newColor, true);
    delay(wait);
  }
}

RGB colorStep(RGB startColor, RGB endColor, int steps, int step) {
  return {
    startColor.r + (endColor.r - startColor.r) * step / steps,
    startColor.g + (endColor.g - startColor.g) * step / steps,
    startColor.b + (endColor.b - startColor.b) * step / steps
  };
}

void scrollText(String textToDisplay) {
  int x = matrix.width();
  int pixelsInText = textToDisplay.length() * 7;
  
  matrix.setCursor(x, 0);
  matrix.print(textToDisplay);
  matrix.show();
  
  while(x > (matrix.width() - pixelsInText)) {
    matrix.fillScreen(matrix.Color(textBackground.r, textBackground.g, textBackground.b));
    matrix.setCursor(--x, 0);
    matrix.print(textToDisplay);
    matrix.show();
    delay(150);
  }
}

