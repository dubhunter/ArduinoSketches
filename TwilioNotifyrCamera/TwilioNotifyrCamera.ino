#include <SPI.h>
#include <Ethernet.h>
#include <NotifyrClient.h>
#include <LiquidCrystal.h>
#include "Credentials.h"

#define REDLITE 3
#define GREENLITE 5
#define BLUELITE 6
#define FOCUS 8
#define SHUTTER 9

// initialize the library with the numbers of the interface pins
LiquidCrystal lcd(A5, A4, A3, A2, A1, A0);

byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xB0, 0x0B };
NotifyrClient notifyr;

bool notifyrConnected = false;
unsigned long lastMillis = 0;
unsigned long timeInterval = 0;
unsigned long timeFocus = 800;
unsigned long timeShutter = 800;

void setup() {
  Serial.begin(9600);
  
  lcd.begin(16, 2);
  
  pinMode(REDLITE, OUTPUT);
  pinMode(GREENLITE, OUTPUT);
  pinMode(BLUELITE, OUTPUT);
  pinMode(FOCUS, OUTPUT);
  pinMode(SHUTTER, OUTPUT);
  digitalWrite(FOCUS, LOW);
  digitalWrite(SHUTTER, LOW);
  
  analogWrite(REDLITE, 255 - 0xff);
  analogWrite(GREENLITE, 255 - 0xa1);
  analogWrite(BLUELITE, 255 - 0x00);
  
  lcd.clear();
  lcd.print("<Twilio SMS Cam>");
  
  Serial.println("Initializing Ethernet...");
  lcd.setCursor(0,1);
  lcd.print("Initializing... ");
  Ethernet.begin(mac);
  
  NotifyrClient::debug();
  
  while (!notifyrConnected) {
    Serial.print("Connecting to Notifyr (");
    Serial.print(NOTIFYR_KEY);
    Serial.println(")...");
    if (notifyr.connect(NOTIFYR_KEY, "sms")) {
      Serial.println("Connected!");
      lcd.setCursor(0,1);
      lcd.print("SMS 415-689-7448");
      notifyrConnected = true;
      notifyr.bind(handleEvent);
    } else {
      Serial.println("Connection failed, trying again in 3 seconds...");
      Serial.println();
      lcd.setCursor(0,1);
      lcd.print("Failed...     :(");
      delay(3000);
      lcd.print("Trying Again... ");
    }
  }
}

void loop() {
  notifyr.listen();
  
  interval();
}

void handleEvent(String data) {
  Serial.println("handleEvent()");
  String line1 = "";
  String line2 = "";
  int colon = data.indexOf(":");
  if (colon > -1) {
    line1 = data.substring(0, colon);
    line2 = data.substring(colon + 1);
  } else {
    line1 = data;
  }
  line1.trim();
  line2.trim();
  camera(line1.charAt(0), line2);
  lcd.setCursor(0,1);
  lcd.print("SMS 415-689-7448");
}

void camera(char cmd, String extra) {
  timeInterval = 0;
  switch (cmd)
  {
    case 'f':
    case 'F':
      Serial.println("Focus");
      focus();
      break;
    case 's':
    case 'S':
      Serial.println("Shutter");
      shutter();
      break;
    case 'i':
    case 'I':
      Serial.println("Interval");
      timeInterval = (extra.toInt() * 1000);
      break;
    default:
      Serial.println("Send 'F' for focus and 'S' for shutter 'I:3' for a 3 second interval");
  }
  
}

void focus() {
  lcd.setCursor(0,1);
  lcd.print("Focusing...     ");
  digitalWrite(FOCUS, HIGH);
  delay(timeFocus);
  digitalWrite(FOCUS, LOW);
}

void shutter() {
  lcd.setCursor(0,1);
  lcd.print("Shooting...     ");
  digitalWrite(SHUTTER, HIGH);
  delay(timeShutter);
  digitalWrite(SHUTTER, LOW);
}

void interval() {
  if (timeInterval > 0) {
    if (millis() - lastMillis > timeInterval) {
      lastMillis = millis();
      shutter();
    }
    String countdown = String(timeInterval - (millis() - lastMillis));
    int pad = (7 - countdown.length());
    while (pad > 0) {
      countdown = " " + countdown;
      pad--;
    }
    lcd.setCursor(0,1);
    lcd.print("Interval:" + countdown);
  }
}
