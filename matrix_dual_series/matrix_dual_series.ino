#include <HT1632.h>

//#define DATA A5
//#define WR   A4
//#define CS   A3
//#define CS2  A2
//#define CS3  A1
//#define CS4  A0
#define DATA1 3
#define WR1   5
#define CS   6
#define CS2  7
#define CS3  8
#define CS4  9


#define DATA2 A0
#define WR2   A1
#define CS5  A2
#define CS6  A3
#define CS7  A4
#define CS8  A5

HT1632LEDMatrix matrix1 = HT1632LEDMatrix(DATA1, WR1, CS, CS2, CS3, CS4);
HT1632LEDMatrix matrix2 = HT1632LEDMatrix(DATA2, WR2, CS5, CS6, CS7, CS8);

void setup() {
    Serial.begin(9600);
    
//    pinMode(CS, OUTPUT);
//    pinMode(CS2, OUTPUT);
//    pinMode(CS3, OUTPUT);
//    pinMode(CS4, OUTPUT);
//    pinMode(CS5, OUTPUT);
//    pinMode(CS6, OUTPUT);
//    pinMode(CS7, OUTPUT);
//    pinMode(CS8, OUTPUT);
    
    matrix1.begin(HT1632_COMMON_16NMOS);
    matrix2.begin(HT1632_COMMON_16NMOS);
//  
    matrix1.clearScreen();
    matrix1.setTextSize(2);
    matrix1.setTextColor(1);
    matrix1.setCursor(0, 0);   // start at top left, with one pixel of spacing
    matrix1.print("TWILIO...");
    matrix1.writeScreen();
    
    matrix2.clearScreen();
    matrix2.setTextSize(2);
    matrix2.setTextColor(1);
    matrix2.setCursor(0, 0);   // start at top left, with one pixel of spacing
    matrix2.print("SCALES...");
    matrix2.writeScreen();
    
//    matrix.clearScreen();
//    matrix.setCursor(0, 0);    
//    matrix.print("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnop");
//    matrix.writeScreen();
}

void loop() {
    
   
}
