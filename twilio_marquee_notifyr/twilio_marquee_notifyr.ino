#include <HT1632.h>
#include <SPI.h>
#include <Ethernet.h>
#include <NotifyrClient.h>
#include "Credentials.h"

//#define DATA A5
//#define WR   A4
//#define CS   A3
//#define CS2  A2
//#define CS3  A1
//#define CS4  A0
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

byte mac[] = { 0x90, 0xA2, 0xDB, 0x00, 0x00, 0x02 };
String response = String();
String sms = String();
unsigned long charCounter = 0;
bool notifyrConnected = false;

const int CHAR_WIDTH = 32;

NotifyrClient notifyr;

HT1632LEDMatrix matrix1 = HT1632LEDMatrix(DATA1, WR1, CS1, CS2, CS3, CS4, CS5, CS6, CS7);
HT1632LEDMatrix matrix2 = HT1632LEDMatrix(DATA2, WR2, CS9, CS10, CS11, CS12, CS13, CS14, CS15);
HT1632LEDMatrix matrix3 = HT1632LEDMatrix(DATA3, WR3, CS17, CS18, CS19, CS20, CS21, CS22, CS23);

void setup() {
  Serial.begin(9600);
  
//  pinMode(CS1, OUTPUT);
//  pinMode(CS2, OUTPUT);
//  pinMode(CS3, OUTPUT);
//  pinMode(CS4, OUTPUT);
//  pinMode(CS5, OUTPUT);
//  pinMode(CS6, OUTPUT);
//  pinMode(CS7, OUTPUT);
//  pinMode(CS8, OUTPUT);
//  pinMode(CS9, OUTPUT);
//  pinMode(CS10, OUTPUT);
//  pinMode(CS11, OUTPUT);
//  pinMode(CS12, OUTPUT);
//  pinMode(CS13, OUTPUT);
//  pinMode(CS14, OUTPUT);
//  pinMode(CS15, OUTPUT);
//  pinMode(CS16, OUTPUT);
//  pinMode(CS17, OUTPUT);
//  pinMode(CS18, OUTPUT);
//  pinMode(CS19, OUTPUT);
//  pinMode(CS20, OUTPUT);
//  pinMode(CS21, OUTPUT);
//  pinMode(CS22, OUTPUT);
//  pinMode(CS23, OUTPUT);
//  pinMode(CS24, OUTPUT);

  matrix1.begin(HT1632_COMMON_16NMOS);
  matrix2.begin(HT1632_COMMON_16NMOS);
  matrix3.begin(HT1632_COMMON_16NMOS);
  
  Serial.println("Writing to matrix1");
  matrix1.clearScreen();
  matrix1.setTextSize(2);
  matrix1.setTextColor(1);
  matrix1.setCursor(0, 0);
  matrix1.print("TWILIO SCALES...");
//  matrix1.print("1234567890ABCDEF");
  matrix1.writeScreen();  
  matrix1.setTextSize(1);
  
  Serial.println("Writing to matrix2");
  matrix2.clearScreen();
  matrix2.setTextSize(2);
  matrix2.setTextColor(1);
  matrix2.setCursor(0, 0);
  matrix2.print("ROW 2 OH YEA");
//  matrix2.print("1234567890ABCDEF");
  matrix2.writeScreen();
  matrix2.setTextSize(1);
  
  Serial.println("Writing to matrix3");
  matrix3.clearScreen();
  matrix3.setTextSize(2);
  matrix3.setTextColor(1);
  matrix3.setCursor(0, 0);
  matrix3.print("ROW 3 BOOM");
  matrix3.writeScreen();
  matrix3.setTextSize(1);
  
  Serial.println("Initializing Ethernet...");
  Ethernet.begin(mac);
  
  NotifyrClient::debug();
  
  while (!notifyrConnected) {
    Serial.print("Connecting to Notifyr (");
    Serial.print(NOTIFYR_KEY);
    Serial.println(")...");
    if (notifyr.connect(NOTIFYR_KEY, "hyduino")) {
      Serial.println("Connected!");
      notifyrConnected = true;
      notifyr.bind(displayData);
    } else {
      Serial.println("Connection failed, trying again in 3 seconds...");
//      matrix.setCursor(0, 0);  
//      matrix.print("NETWORK ERROR");
//      matrix.writeScreen();
      Serial.println();
      delay(3000);
    }
  }
  
  Serial.println("Joined: " + Ethernet.localIP());
}

void loop() {
  notifyr.listen();
}

void displayData(String data) {
  Serial.println("data: " + data);
//  Serial.println("Clearing screen");
//  matrix.clearScreen();
//  Serial.println("Setting cursor");
//  matrix.setCursor(0, 0);
//  
//  Serial.println("Printing: " + data);
//  matrix.print(data);
//  
//  Serial.println("Writing...");
//  matrix.writeScreen();
}
