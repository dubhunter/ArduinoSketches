#include <SPI.h>
#include <Ethernet.h>
#include <NotifyrClient.h>
#include <LiquidCrystal.h>
#include "Credentials.h"

#define REDLITE 3
#define GREENLITE 5
#define BLUELITE 6

// initialize the library with the numbers of the interface pins
LiquidCrystal lcd(A5, A4, A3, A2, A1, A0);

byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xB0, 0x0B };
NotifyrClient notifyr;

void setup() {
  Serial.begin(9600);
  
  lcd.begin(16, 2);
//  lcd.autoscroll();
  
  pinMode(REDLITE, OUTPUT);
  pinMode(GREENLITE, OUTPUT);
  pinMode(BLUELITE, OUTPUT);
  
  analogWrite(REDLITE, 255 - 0xff);
  analogWrite(GREENLITE, 255 - 0xa1);
  analogWrite(BLUELITE, 255 - 0x00);
  
  lcd.clear();
  lcd.print("Twilio SMS");
  
  Serial.println("Initializing Ethernet...");
  Ethernet.begin(mac);
  
  NotifyrClient::debug();
  
  Serial.print("Connecting to Notifyr (");
  Serial.print(NOTIFYR_KEY);
  Serial.println(")...");
  
  if (notifyr.connect(NOTIFYR_KEY, "sms")) {
    Serial.println("Connected!");
    lcd.setCursor(0,1);
    lcd.print("Connected...");
    notifyr.bind(handleEvent);
  } else {
    Serial.println("Connection failed...");
    for (;;) ; // do nothing forever
  }
}

void loop() {
  notifyr.listen();
}

void handleEvent(String data) {
  Serial.println("handleEvent()");
  String line1 = "";
  String line2 = "";
  int newline = data.indexOf("\\n");
  if (newline > -1) {
    line1 = data.substring(0, newline);
    line2 = data.substring(newline + 2);
  } else if (data.length() > 16) {
    line1 = data.substring(0, 16);
    line2 = data.substring(16);
  }
  line1.trim();
  line2.trim();
  lcd.clear();
  lcd.print(line1);
  lcd.setCursor(0,1);
  lcd.print(line2);
}
