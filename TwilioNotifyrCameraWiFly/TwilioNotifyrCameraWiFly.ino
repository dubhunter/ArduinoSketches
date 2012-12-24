#include <SPI.h>
#include <WiFly.h>
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

NotifyrClient notifyr;

bool wiflyConnected = false;
bool notifyrConnected = false;

void setup() {
  Serial.begin(9600);
  
  lcd.begin(16, 2);
//  lcd.autoscroll();
  
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
  lcd.print("Twilio SMS Cam");
  
  Serial.println("Initializing WiFi...");
  lcd.setCursor(0,1);
  lcd.print("Initializing... ");
  WiFly.begin();
  
  while (!wiflyConnected) {
    Serial.print("Attempting to join: ");
    Serial.println(SSID);
    
    if (WiFly.join(SSID, PASSPHRASE, true)) {  
      Serial.println("Joined!");
      lcd.setCursor(0,1);
      lcd.print("WiFy Connected! ");
      wiflyConnected = true;
    } else {
      Serial.println("Association failed, trying again in 3 seconds...");
      Serial.println();
      delay(3000);
    }
  }
  
  NotifyrClient::debug();
  
  while (!notifyrConnected) {
    Serial.print("Connecting to Notifyr (");
    Serial.print(NOTIFYR_KEY);
    Serial.println(")...");
    if (notifyr.connect(NOTIFYR_KEY, "sms")) {
      Serial.println("Connected!");
      lcd.setCursor(0,1);
      lcd.print("Ready for SMS...");
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
  } else {
    line1 = data;
  }
  line1.trim();
  line2.trim();
//  lcd.clear();
//  lcd.print(line1);
//  lcd.setCursor(0,1);
//  lcd.print(line2);
  camera(line1.charAt(0));
  lcd.setCursor(0,1);
  lcd.print("Ready for SMS...");
}

void camera(char cmd) {
  switch (cmd)
  {
    case 'f':
    case 'F':
      Serial.println("Focus");
      lcd.setCursor(0,1);
      lcd.print("Focusing...     ");
      digitalWrite(FOCUS, HIGH);
      delay(800); // May want to adjust this depending on focus time
      digitalWrite(FOCUS, LOW);
      break;
    case 's':
    case 'S':
      Serial.println("Shutter");
      lcd.setCursor(0,1);
      lcd.print("Shooting...     ");
      digitalWrite(SHUTTER, HIGH);
      delay(2000); // May want to adjust this depending on shot type
      digitalWrite(SHUTTER, LOW);
      break;
    default:
      Serial.println("Send 'F' for focus and 'S' for shutter");
  }
  
}
