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

int value = 0;
RGB colors[3] = {
  red,
  green,
  blue
};

void setup() {
  Serial.begin(9600);
  Serial.println("Started");
  
  pinMode(PIN_CLOUD, INPUT);
  
  matrix.begin();
  matrix.setBrightness(40);
  matrix.setTextColor(matrix.Color(white.r, white.g, white.b));
  matrix.setTextWrap(false);
}

void loop() {
  value = round(analogRead(1) / 10.24);
  Serial.println(value);
  
  if (value > 10) {
    RGB color = off;
    if (value >= 10 && value < 40) {
        color = colors[0];
    } else if (value >= 40 && value < 70) {
        color = colors[1];
    } else if (value >= 70 && value < 100) {
        color = colors[2];
    }
    
    crossFade(off, color, 50, 3);
  }
}

// Fill the dots one after the other with a color
void colorWipe(RGB color, uint8_t wait) {
  for(uint16_t row=0; row < 8; row++) {
    for(uint16_t column=0; column < 8; column++) {
      matrix.drawPixel(column, row, matrix.Color(color.r, color.g, color.b));
      matrix.show();
      delay(wait);
    }
  }
}

// Fade pixel (x, y) from startColor to endColor
void fadePixel(int x, int y, RGB startColor, RGB endColor, int steps, int wait) {
  for(int i = 0; i <= steps; i++) 
  {
     int newR = startColor.r + (endColor.r - startColor.r) * i / steps;
     int newG = startColor.g + (endColor.g - startColor.g) * i / steps;
     int newB = startColor.b + (endColor.b - startColor.b) * i / steps;
     
     matrix.drawPixel(x, y, matrix.Color(newR, newG, newB));
     matrix.show();
     delay(wait);
  }
}

// Crossfade entire screen from startColor to endColor
void crossFade(RGB startColor, RGB endColor, int steps, int wait) {
  for(int i = 0; i <= steps; i++)
  {
     int newR = startColor.r + (endColor.r - startColor.r) * i / steps;
     int newG = startColor.g + (endColor.g - startColor.g) * i / steps;
     int newB = startColor.b + (endColor.b - startColor.b) * i / steps;
     
     matrix.fillScreen(matrix.Color(newR, newG, newB));
     matrix.show();
     delay(wait);
  }
}


