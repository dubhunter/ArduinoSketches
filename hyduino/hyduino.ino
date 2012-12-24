#include <SPI.h>
#include <Ethernet.h>
#include <NotifyrClient.h>
#include "Credentials.h"

byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
NotifyrClient notifyr;

const int PIN_POWER = 7;
const int PIN_POWER_STATUS = 6;

bool notifyrConnected = false;

void setup() {
  pinMode(PIN_POWER, OUTPUT);
  pinMode(PIN_POWER_STATUS, OUTPUT);
  
  Serial.begin(9600);
  
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
      notifyr.bind(doPower);
    } else {
      Serial.println("Connection failed, trying again in 3 seconds...");
      Serial.println();
      delay(3000);
    }
  }
}

void loop() {
  notifyr.listen();
}

void doPower(String data) {
  Serial.print("doPower: ");
  Serial.println(data);
  if (data == "true" || data == "1" || data == "on" || data == "high") {
    digitalWrite(PIN_POWER, HIGH);
    digitalWrite(PIN_POWER_STATUS, HIGH);
  } else {
    digitalWrite(PIN_POWER, LOW);
    digitalWrite(PIN_POWER_STATUS, LOW);
  }
}
