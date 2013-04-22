#include <Metro.h>
#include <SPI.h>
#include <Ethernet.h>
#include <QueueList.h>
#include "Credentials.h"

#define DEBUG true

#define PIN_POWER 2
#define PIN_STATUS_RED 3
#define PIN_STATUS_GRN 5
#define PIN_STATUS_BLU 6
#define PIN_SENSOR_LIQUID A0
#define PIN_SENSOR_LIGHT A5

#define HOST "hyduino.willandchi.com"
#define POLL_ENDPOINT "/v1/poll"
#define EVENT_ENDPOINT "/v1/events"

byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };

EthernetClient client;

QueueList <String> events;

Metro sensorSampleMetro = Metro(100);
Metro sensorSendMetro = Metro(60000);
Metro pollMetro = Metro(6000);
Metro healthMetro = Metro(180000);

//colors
int colorPurple[] = { 0xff, 0x00, 0xff };
int colorBlue[] = { 0x00, 0x6f, 0xff };
int colorGreen[] = { 0xa0, 0xff, 0x00 };
int colorYellow[] = { 0xff, 0xd0, 0x00 };
int colorOrange[] = { 0xff, 0xa1, 0x00 };
int colorRed[] = { 0xff, 0x00, 0x00 };
int colorPink[] = { 0xff, 0x4a, 0x9e };

//boolean wiflyConnected = false;
boolean clientConnected = false;
boolean receiving = false;  

long readingLight = 0;
long readingLiquid = 0;

long readingLightCount = 0;
long readingLiquidCount = 0;

char lastChar;
String buffer = "";

void setup() {
  pinMode(PIN_POWER, OUTPUT);

  pinMode(PIN_STATUS_RED, OUTPUT);
  pinMode(PIN_STATUS_GRN, OUTPUT);
  pinMode(PIN_STATUS_BLU, OUTPUT);

  pinMode(PIN_SENSOR_LIQUID, INPUT);
  pinMode(PIN_SENSOR_LIGHT, INPUT);

  Serial.begin(9600);
  
  initEthernet();
}

void loop() {

  if (healthMetro.check()) {
    sLog("Ethernet Error: Attempting Ethernet Reboot");
    lightOn(colorRed);
    initEthernet();
    logEvent("ethernet", "error", true);
  }

  if (sensorSampleMetro.check()) {
    readingLight += analogRead(PIN_SENSOR_LIGHT);
    readingLiquid += analogRead(PIN_SENSOR_LIQUID);

    readingLightCount++;
    readingLiquidCount++;
  }

  if (sensorSendMetro.check()) {
    logEvent("light", String(readingLight / readingLightCount));
    //this coupled with the max() should give us ranges from 0-250 (0" - 8")
    readingLiquid = 500 - (readingLiquid / readingLiquidCount);
    logEvent("liquid", String(max(readingLiquid, 0)));

    readingLight = 0;
    readingLiquid = 0;

    readingLightCount = 0;
    readingLiquidCount = 0;
  }

  poll();
  processEvents();

  if (clientConnected && !client.connected()) {
    client.stop();
    clientConnected = false;
  }
}

void initEthernet() {
  lightOn(colorPink);
  sLog("Initializing Ethernet...");
  Ethernet.begin(mac);
  
  clientConnected = false;
  receiving = false;
  healthMetro.reset();
  
  lightOn(colorGreen);
  sLog();
  logEvent("ethernet", "connected", true);
}

void doPower(String data) {
  data.toLowerCase();
  sLog("doPower: " + data);
  if (data == "1" || data == "true" || data == "on") {
    digitalWrite(PIN_POWER, HIGH);
    lightOn(colorBlue);
    logEvent("power", "on", true);
  } 
  else {
    digitalWrite(PIN_POWER, LOW);
    lightOn(colorGreen);
    logEvent("power", "off", true);
  }
}

void logEvent(String event, String data) {
  logEvent(event, data, false);
}

void logEvent(String event, String data, boolean force) {
  if (events.count() <= 6 || force) {
    String payload = "event=" + event + "&data=" + data;
    events.push(payload);
  }
}

void poll() {
  if (pollMetro.check() && !clientConnected) {
    if (client.connect(HOST, 80)) {
      clientConnected = true;
      sLog("Polling: Connected!");
      //Make the request
      client.println("GET " + String(POLL_ENDPOINT) + " HTTP/1.1");
      client.println("Host: " + String(HOST));
      client.println("Authorization: Basic " + String(BASIC_AUTH));
      client.println("Connection: close");
      client.println();
      healthMetro.reset();
      delay(5000);
    } 
    else {
      sLog("Sending Event: Failed :(");
      clientConnected = false;
    }
  }
  
  if (client.available()) {
    char c = client.read();

    if (c != '\r') { //ignore \r (and dont put it in lastChar)
      if (c != lastChar || c != ' ') { //ignore double spaces
//        Serial.print(c);
        if (receiving && lastChar == '\n' && c == '\n') { //detect double newline
          //remove newline and quote
          buffer = buffer.substring(0, buffer.length() - 2);
          sLog("Data: " + buffer);
          doPower(buffer);
          //reset
          buffer = "";
          receiving = false;
          client.stop();
          clientConnected = false;
        }
        
        buffer += c;
        if (buffer.endsWith("data: \"")) {
          sLog("Receiving Event...");
          buffer = "";
          receiving = true;
        }
        
        if (!receiving && c == '\n') {
          buffer = "";
        }
      }
      lastChar = c;
    }
  }
}

void processEvents() {
  if (!events.isEmpty() && !clientConnected) {
    if (client.connect(HOST, 80)) {
      clientConnected = true;
      String payload = events.pop();
      sLog(payload);
      sLog("Sending Event: Connected!");
      //Make the request
      client.println("POST " + String(EVENT_ENDPOINT) + " HTTP/1.1");
      client.println("Host: " + String(HOST));
      client.println("Authorization: Basic " + String(BASIC_AUTH));
      client.println("Connection: close");
      client.println("Content-Type: application/x-www-form-urlencoded");
      client.println("Content-length: " + String(payload.length()));
      client.println();
      client.println(payload);
      client.stop();
      clientConnected = false;
      healthMetro.reset();
//      delay(5000);
    } 
    else {
      sLog("Sending Event: Failed :(");
      clientConnected = false;
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

void sLog() {
  sLog("");
}

void sLog(String s) {
  if (DEBUG) {
    Serial.println(s);
  }
}
