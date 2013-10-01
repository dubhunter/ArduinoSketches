#include <HT1632.h>
#include <RPiSerialClient.h>

#define DEBUG true

#define DATA1 22
#define WR1   23
#define CS1   28
#define CS2   29
#define CS3   30
#define CS4   31
#define CS5   32
#define CS6   33
#define CS7   34
#define CS8   35

#define DATA2 24
#define WR2   25
#define CS9   36
#define CS10  37
#define CS11  38
#define CS12  39
#define CS13  40
#define CS14  41
#define CS15  42
#define CS16  43

#define DATA3 26
#define WR3   27
#define CS17  44
#define CS18  45
#define CS19  46
#define CS20  47
#define CS21  48
#define CS22  49
#define CS23  8
#define CS24  9

String response = String();
String sms = String();
unsigned long charCounter = 0;

const int CHAR_WIDTH = 28;
const int LARGE_CHAR_WIDTH = 14;
const int LARGE_CHAR_MAX = 42;

RPiSerialClient rpi;

HT1632LEDMatrix matrix1 = HT1632LEDMatrix(DATA1, WR1, CS1, CS2, CS3, CS4, CS5, CS6, CS7);
HT1632LEDMatrix matrix2 = HT1632LEDMatrix(DATA2, WR2, CS9, CS10, CS11, CS12, CS13, CS14, CS15);
HT1632LEDMatrix matrix3 = HT1632LEDMatrix(DATA3, WR3, CS17, CS18, CS19, CS20, CS21, CS22, CS23);

void setup() {
  rpi.begin();

  matrix1.begin(HT1632_COMMON_16NMOS);
  matrix2.begin(HT1632_COMMON_16NMOS);
  matrix3.begin(HT1632_COMMON_16NMOS);

//  Serial.println("Writing to matrix1");
  matrix1.clearScreen();
  matrix1.setTextSize(2);
  matrix1.setTextColor(1);
  matrix1.setCursor(0, 0);
  matrix1.print("<<TWILIO SMS>>");
  matrix1.writeScreen();  
  matrix1.setTextSize(1);

//  Serial.println("Writing to matrix2");
  matrix2.clearScreen();
  matrix2.setTextSize(2);
  matrix2.setTextColor(1);
  matrix2.setCursor(0, 0);
  matrix2.print("<415.689.7448>");
  matrix2.writeScreen();
  matrix2.setTextSize(1);

//  Serial.println("Writing to matrix3");
  matrix3.clearScreen();
  matrix3.setTextSize(2);
  matrix3.setTextColor(1);
  matrix3.setCursor(0, 0);
  matrix3.print("INITIALIZING..");
  matrix3.writeScreen();

  if (DEBUG) {
    RPiSerialClient::debug();
  }
  
  rpi.bind(displayData);
  
  matrix3.clearScreen();
  matrix3.setCursor(0, 0);
  matrix3.print("READY!        ");
  matrix3.writeScreen();
  
  matrix1.setTextSize(1);
  matrix2.setTextSize(1);
  matrix3.setTextSize(1);
}

void loop() {
  rpi.listen();
}

void displayData(String data) {
  String line1 = data;
  String line2 = "";
  String line3 = "";
  String line4 = "";
  String line5 = "";
  String line6 = "";
  int space = 0;
  int newline = 0;
  boolean largePrint = line1.length() <= LARGE_CHAR_MAX;
  int textSize = largePrint ? 2 : 1;
  int lineWidth = largePrint ? LARGE_CHAR_WIDTH : CHAR_WIDTH;
  
  matrix1.setTextSize(textSize);
  matrix2.setTextSize(textSize);
  matrix3.setTextSize(textSize);

  if (line1.length() > lineWidth) {
    space = line1.lastIndexOf(" ", lineWidth);
    if ((newline = line1.indexOf("\\n", space)) > 0) {
      space = newline;
    }
    line2 = line1.substring(space + 1 + (newline > 0 ? 1 : 0));
    line1 = line1.substring(0, space);
  }

  if (line2.length() > lineWidth) {
    space = line2.lastIndexOf(" ", lineWidth);
    if ((newline = line2.indexOf("\\n", space)) > 0) {
      space = newline;
    }
    line3 = line2.substring(space + 1 + (newline > 0 ? 1 : 0));
    line2 = line2.substring(0, space);
  }

  if (line3.length() > lineWidth) {
    space = line3.lastIndexOf(" ", lineWidth);
    if ((newline = line3.indexOf("\\n", space)) > 0) {
      space = newline;
    }
    line4 = line3.substring(space + 1 + (newline > 0 ? 1 : 0));
    line3 = line3.substring(0, space);
  }

  if (line4.length() > lineWidth) {
    space = line4.lastIndexOf(" ", lineWidth);
    if ((newline = line4.indexOf("\\n", space)) > 0) {
      space = newline;
    }
    line5 = line4.substring(space + 1 + (newline > 0 ? 1 : 0));
    line4 = line4.substring(0, space);
  }

  if (line5.length() > lineWidth) {
    space = line5.lastIndexOf(" ", lineWidth);
    if ((newline = line5.indexOf("\\n", space)) > 0) {
      space = newline;
    }
    line6 = line5.substring(space + 1 + (newline > 0 ? 1 : 0));
    line5 = line5.substring(0, space);
  }
  
  if ((newline = line6.indexOf("\\n")) > 0) {
    line6 = line6.substring(0, newline + 2);
  }
  
  if (largePrint) {
    line5 = line3;
    line3 = line2;
    line2 = "";
    line4 = "";
    line6 = "";
  }

  sLog();
  sLog("Clearing screen...");
  matrix1.clearScreen();
  matrix2.clearScreen();
  matrix3.clearScreen();

  sLog();
  sLog("Printing...");
  sLog(line1);
  sLog(line2);
  sLog(line3);
  sLog(line4);
  sLog(line5);
  sLog(line6);
  
  matrix1.setCursor(0, 0);
  matrix1.print(line1);
  if (!largePrint) {
    matrix1.setCursor(0, 8);
    matrix1.print(line2);
  }

  matrix2.setCursor(0, 0);
  matrix2.print(line3);
  if (!largePrint) {
    matrix2.setCursor(0, 8);
    matrix2.print(line4);
  }

  matrix3.setCursor(0, 0);
  matrix3.print(line5);
  if (!largePrint) {
    matrix3.setCursor(0, 8);
    matrix3.print(line6);
  }

  sLog();
  sLog("Writing...");
  matrix1.writeScreen();
  matrix2.writeScreen();
  matrix3.writeScreen();
}

void sLog() {
  sLog("");
}

void sLog(String s) {
  if (DEBUG) {
    Serial.println(s);
  }
}

