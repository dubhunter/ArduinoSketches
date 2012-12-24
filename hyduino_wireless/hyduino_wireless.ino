#include <SPI.h>
#include <WiFly.h>
#include <NotifyrClient.h>
#include "Credentials.h"

NotifyrClient notifyr;

const int PIN_POWER = 7;
const int PIN_POWER_STATUS = 6;

bool wiflyConnected = false;
bool notifyrConnected = false;

void setup() {
  pinMode(PIN_POWER, OUTPUT);
  pinMode(PIN_POWER_STATUS, OUTPUT);
  
  Serial.begin(9600);
  
  Serial.println("Initializing WiFi...");
  WiFly.begin();
  
  while (!wiflyConnected) {
    Serial.print("Attempting to join: ");
    Serial.println(SSID);
    
    if (WiFly.join(SSID, PASSPHRASE, true)) {  
      Serial.println("Joined!");
      wiflyConnected = true;
    } else {
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
