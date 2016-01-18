#include <Bounce2.h>
#include <Adafruit_NeoPixel.h>
#include <Adafruit_GFX.h>
#include <Adafruit_NeoMatrix.h>
#include "RGB.h"

#define PIN_MATRIX 1
#define PIN_BUTTON A1

Adafruit_NeoMatrix matrix = Adafruit_NeoMatrix(8, 8, PIN_MATRIX,
  NEO_MATRIX_TOP + NEO_MATRIX_LEFT + NEO_MATRIX_ROWS + NEO_MATRIX_PROGRESSIVE,
  NEO_GRB + NEO_KHZ800
);

Bounce bouncer = Bounce();

int button = 0;
int face = 0;
int total = 3;
RGB colors[3] = {
  off,
  yellow,
  red
};

int faces[3][8][8] = {
  {
     {0, 0, 0, 0, 0, 0, 0, 0},
     {0, 1, 1, 0, 0, 1, 1, 0},
     {0, 1, 1, 0, 0, 1, 1, 0},
     {0, 0, 0, 0, 0, 0, 0, 0},
     {0, 0, 0, 0, 0, 0, 0, 0},
     {0, 1, 0, 0, 0, 0, 1, 0},
     {0, 0, 1, 1, 1, 1, 0, 0},
     {0, 0, 0, 0, 0, 0, 0, 0}
  },
  {
     {0, 0, 0, 0, 0, 0, 0, 0},
     {0, 2, 2, 0, 0, 2, 2, 0},
     {0, 0, 2, 0, 0, 2, 0, 0},
     {0, 0, 0, 0, 0, 0, 0, 0},
     {0, 0, 0, 0, 0, 0, 0, 0},
     {0, 0, 2, 2, 2, 2, 0, 0},
     {0, 2, 0, 0, 0, 0, 2, 0},
     {0, 0, 0, 0, 0, 0, 0, 0}
  },
  {
     {0, 0, 0, 0, 0, 0, 0, 0},
     {0, 2, 2, 0, 0, 2, 2, 0},
     {2, 0, 0, 2, 2, 0, 0, 2},
     {2, 0, 0, 0, 0, 0, 0, 2},
     {2, 0, 0, 0, 0, 0, 0, 2},
     {0, 2, 0, 0, 0, 0, 2, 0},
     {0, 0, 2, 0, 0, 2, 0, 0},
     {0, 0, 0, 2, 2, 0, 0, 0}
  }
};

void setup() {
  Serial.begin(9600);
  Serial.println("Started");
  
  pinMode(PIN_BUTTON, INPUT);
  
  bouncer.attach(PIN_BUTTON);
  bouncer.interval(50);
  
  matrix.begin();
  matrix.setBrightness(40);
  
  drawFace();
}

void loop() {
  bouncer.update();
  
  if (bouncer.fell()) {
    face++;
    if (face >= total) {
      face = 0;
    }
    drawFace();
  }
}

void drawFace() {
  for(int row = 0; row < 8; row++) {
    for(int column = 0; column < 8; column++) {
      fillPixel(column, row, colors[faces[face][row][column]], false);
    }
  }
  matrix.show();
}

void fillPixel(int x, int y, RGB color, int show) {
   matrix.drawPixel(x, y, matrix.Color(color.r, color.g, color.b));
   if (show) {
     matrix.show();
   }
}
