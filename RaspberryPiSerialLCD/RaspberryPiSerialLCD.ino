#include <RPiSerialClient.h>
#include <LiquidCrystal.h>

#define REDLITE 3
#define GREENLITE 5
#define BLUELITE 6

// initialize the library with the numbers of the interface pins
LiquidCrystal lcd(A5, A4, A3, A2, A1, A0);

RPiSerialClient rpi;

void setup() {
  rpi.begin();
  
  lcd.begin(16, 2);
  
  pinMode(REDLITE, OUTPUT);
  pinMode(GREENLITE, OUTPUT);
  pinMode(BLUELITE, OUTPUT);
  
  analogWrite(REDLITE, 255 - 0xff);
  analogWrite(GREENLITE, 255 - 0xa1);
  analogWrite(BLUELITE, 255 - 0x00);
  
  lcd.clear();
  lcd.print("Raspberry Pi LCD");
  lcd.setCursor(0,1);
  lcd.print("Ready for Data...");
  
  rpi.debug();
  
  rpi.bind(handleEvent);
}

void loop() {
  rpi.listen();
}

void handleEvent(String data) {
  String line1 = "";
  String line2 = "";
  int newline = data.indexOf('\n');
  Serial.println(newline);
  if (newline > -1) {
    line1 = data.substring(0, newline);
    line2 = data.substring(newline + 2);
  } else if (data.length() > 16) {
    line1 = data.substring(0, 16);
    line2 = data.substring(16);
  } else {
    line1 = data;
  }
  line1.trim();
  line2.trim();
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print(line1);
  lcd.setCursor(0,1);
  lcd.print(line2);
}
