#include <SPI.h>
#include <WiFly.h>
#include <NotifyrClient.h>
#include "Credentials.h"

#define PIN_POWER 2
#define PIN_STATUS_RED 3
#define PIN_STATUS_GRN 5
#define PIN_STATUS_BLU 6
#define PIN_SENSOR_LIQUID A0
#define PIN_SENSOR_LIGHT A5

NotifyrClient notifyr;

//colors
int colorPurple[] = { 0xff, 0x00, 0xff };
int colorBlue[] = { 0x00, 0x6f, 0xff };
int colorGreen[] = { 0xa0, 0xff, 0x00 };
int colorYellow[] = { 0xff, 0xd0, 0x00 };
int colorOrange[] = { 0xff, 0xa1, 0x00 };
int colorRed[] = { 0xff, 0x00, 0x00 };
int colorPink[] = { 0xff, 0x4a, 0x9e };

bool wiflyConnected = false;
bool notifyrConnected = false;

int readingLight = 0;
int readingLiquid = 0;

void setup() {
  pinMode(PIN_POWER, OUTPUT);
  pinMode(PIN_STATUS_RED, OUTPUT);
  pinMode(PIN_STATUS_GRN, OUTPUT);
  pinMode(PIN_STATUS_BLU, OUTPUT);
  
  pinMode(PIN_SENSOR_LIQUID, INPUT);
  pinMode(PIN_SENSOR_LIGHT, INPUT);

  Serial.begin(9600);

  lightColor(colorPurple);
  Serial.println("Initializing WiFi...");
  WiFly.begin();

  while (!wiflyConnected) {
    Serial.print("Attempting to join: ");
    Serial.println(SSID);

    if (WiFly.join(SSID, PASSPHRASE, true)) {  
      Serial.println("Joined!");
      wiflyConnected = true;
    } 
    else {
      lightColor(colorOrange);
      Serial.println("Association failed, trying again in 3 seconds...");
      Serial.println();
      delay(3000);
    }
  }

  Serial.println();

  NotifyrClient::debug();

  while (!notifyrConnected) {
    Serial.print("Connecting to Notifyr (");
    Serial.print(NOTIFYR_KEY);
    Serial.println(")...");
    if (notifyr.connect(NOTIFYR_KEY, "sms")) {
      lightColor(colorGreen);
      Serial.println("Connected!");
      notifyrConnected = true;
      notifyr.bind(doPower);
//      notifyr.bind(doLight);
    } 
    else {
      lightColor(colorPink);
      Serial.println("Connection failed, trying again in 3 seconds...");
      Serial.println();
      delay(3000);
    }
  }
}

void loop() {
  notifyr.listen();
  
  readingLiquid = analogRead(PIN_SENSOR_LIQUID);
  readingLight = analogRead(PIN_SENSOR_LIGHT);
  
//  Serial.println("Liquid: " + String(readingLiquid));
//  Serial.println("Light: " + String(readingLight));
}

void doPower(String data) {
  Serial.print("doPower: ");
  Serial.println(data);
  if (data == "true" || data == "1" || data == "on" || data == "high") {
    digitalWrite(PIN_POWER, HIGH);
    lightColor(colorBlue);
  } 
  else {
    digitalWrite(PIN_POWER, LOW);
    lightColor(colorGreen);
  }
}

void doLight(String data) {
  Serial.print("doLight: ");
  Serial.println(data);
  if (data.charAt(0) == '0') {
    analogWrite(PIN_STATUS_RED, 255);
    analogWrite(PIN_STATUS_GRN, 0);
    analogWrite(PIN_STATUS_BLU, 0);
  } else if (data.charAt(0) == '1') {
    analogWrite(PIN_STATUS_RED, 0);
    analogWrite(PIN_STATUS_GRN, 255);
    analogWrite(PIN_STATUS_BLU, 0);
  } else {
    analogWrite(PIN_STATUS_RED, 0);
    analogWrite(PIN_STATUS_GRN, 0);
    analogWrite(PIN_STATUS_BLU, 255);
  }
}

void lightColor(int color[]) {
  analogWrite(PIN_STATUS_RED, gammaCorrect(color[0]));
  analogWrite(PIN_STATUS_GRN, gammaCorrect(color[1]));
  analogWrite(PIN_STATUS_BLU, gammaCorrect(color[2]));
}

int gammaCorrect(int channel) {
  return channel == 0 ? 0 : pow(255, ((channel + 1) / 4) / 64.0) + 0.5;
}

