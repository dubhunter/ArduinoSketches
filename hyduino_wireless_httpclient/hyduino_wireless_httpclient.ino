#include <SPI.h>
#include <WiFly.h>
#include <HTTPClient.h>
//#include <NotifyrClient.h>
#include <QueueList.h>
#include <Metro.h>
#include <stdio.h>
#include "Credentials.h"

#define PIN_POWER 2
#define PIN_STATUS_RED 3
#define PIN_STATUS_GRN 5
#define PIN_STATUS_BLU 6
#define PIN_SENSOR_LIQUID A0
#define PIN_SENSOR_LIGHT A5

#define HOST "hyduino.willandchi.com"
#define ENDPOINT "/event"
//#define HOST "sign.willmason.me"
//#define ENDPOINT "/stashboard"

byte server[] = { 54, 243, 227, 22 };
//char host[] = "hyduino.willandchi.com";
//char endpoint[] = "/event";

//NotifyrClient notifyr;

WiFlyClient client = WiFlyClient(HOST, 80);

QueueList <String> events;

Metro sensorSampleMetro = Metro(100);
Metro sensorSendMetro = Metro(10000);

//colors
int colorPurple[] = { 0xff, 0x00, 0xff };
int colorBlue[] = { 0x00, 0x6f, 0xff };
int colorGreen[] = { 0xa0, 0xff, 0x00 };
int colorYellow[] = { 0xff, 0xd0, 0x00 };
int colorOrange[] = { 0xff, 0xa1, 0x00 };
int colorRed[] = { 0xff, 0x00, 0x00 };
int colorPink[] = { 0xff, 0x4a, 0x9e };

boolean wiflyConnected = false;
boolean notifyrConnected = false;
boolean clientConnected = false;
boolean loggin = false;

long readingLight = 0;
long readingLiquid = 0;

long readingLightCount = 0;
long readingLiquidCount = 0;

char buffer[40];

void setup() {
  pinMode(PIN_POWER, OUTPUT);

  pinMode(PIN_STATUS_RED, OUTPUT);
  pinMode(PIN_STATUS_GRN, OUTPUT);
  pinMode(PIN_STATUS_BLU, OUTPUT);

  pinMode(PIN_SENSOR_LIQUID, INPUT);
  pinMode(PIN_SENSOR_LIGHT, INPUT);

  Serial.begin(9600);

  lightOn(colorPink);
  Serial.println("Initializing WiFi...");
  WiFly.begin();

  while (!wiflyConnected) {
    Serial.print("Attempting to join: ");
    Serial.println(SSID);

    if (WiFly.join(SSID, PASSPHRASE, true)) {  
      lightOn(colorGreen);
      Serial.println("Joined!");
      wiflyConnected = true;
    } 
    else {
      lightOn(colorOrange);
      Serial.println("Association failed, trying again in 3 seconds...");
      Serial.println();
      delay(3000);
    }
  }

  Serial.println();

  //  NotifyrClient::debug();
  //
  //  while (!notifyrConnected) {
  //    Serial.print("Connecting to Notifyr (");
  //    Serial.print(NOTIFYR_KEY);
  //    Serial.println(")...");
  //    if (notifyr.connect(NOTIFYR_KEY, "sms")) {
  //      lightOn(colorGreen);
  //      Serial.println("Connected!");
  //      notifyrConnected = true;
  //      notifyr.bind(doPower);
  //    } else {
  //      lightOn(colorPink);
  //      Serial.println("Connection failed, trying again in 3 seconds...");
  //      Serial.println();
  //      delay(3000);
  //    }
  //  }
}

void loop() {
  //  notifyr.listen();

  if (sensorSampleMetro.check() == 1) {
    readingLight += analogRead(PIN_SENSOR_LIGHT);
    readingLiquid += analogRead(PIN_SENSOR_LIQUID);

    readingLightCount++;
    readingLiquidCount++;
  }

  if (sensorSendMetro.check() == 1) {
    logEvent("light", String(readingLight / readingLightCount));
//    logEvent("liquid", String(readingLiquid / readingLiquidCount));

    readingLight = 0;
    readingLiquid = 0;

    readingLightCount = 0;
    readingLiquidCount = 0;
  }

  processEvents();
}

void doPower(String data) {
  data.toLowerCase();
  Serial.print("doPower: ");
  Serial.println(data);
  if (data == "1" || data == "true" || data == "on") {
    digitalWrite(PIN_POWER, HIGH);
    lightOn(colorBlue);
    logEvent("power", "on");
  } 
  else {
    digitalWrite(PIN_POWER, LOW);
    lightOn(colorGreen);
    logEvent("power", "off");
  }
}

void logEvent(String event, String data) {
  String payload = "event=" + event + "&data=" + data;
  events.push(payload);
}

void processEvents() {
  if (!events.isEmpty() && !client.connected()) {
    Serial.println("Sending Event... ");
    String payload = events.pop();
    Serial.println(payload);
    payload.toCharArray(buffer, 40);
    HTTPClient client(HOST, server);
    FILE* result = client.postURI(ENDPOINT, NULL, buffer);
    int returnCode = client.getLastReturnCode();
    Serial.println(returnCode);
    if (result != NULL) {
        while (result.available()) {
        char c = result.read();
        Serial.print(c);
      }
      client.closeStream(result);
    } 
    else {
      Serial.println("Sending Event: Failed :(");
    }
  }
}

void lightOn(int color[]) {
  analogWrite(PIN_STATUS_RED, correctGamma(color[0]));
  analogWrite(PIN_STATUS_GRN, correctGamma(color[1]));
  analogWrite(PIN_STATUS_BLU, correctGamma(color[2]));
}

void lightOff() {
  analogWrite(PIN_STATUS_RED, 0);
  analogWrite(PIN_STATUS_GRN, 0);
  analogWrite(PIN_STATUS_BLU, 0);
}

int correctGamma(int channel) {
  return channel == 0 ? 0 : pow(255, ((channel + 1) / 4) / 64.0) + 0.5;
}


