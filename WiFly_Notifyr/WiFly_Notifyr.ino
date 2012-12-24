
// (Based on Ethernet's WebClient Example)

#include <SPI.h>
#include <WiFly.h>

#include "Credentials.h"

WiFlyClient client("api.notifyr.io", 80);

const int PIN_POWER = 7;
const int PIN_POWER_STATUS = 6;

bool dataReceiving = false;
char lastChar;
String buffer;

void setup() {
  
  Serial.begin(9600);
  
  pinMode(PIN_POWER, OUTPUT);
  pinMode(PIN_POWER_STATUS, OUTPUT);
  
  Serial.println("Initializing WiFi...");
  WiFly.begin();
  
  Serial.print("Attempting to join: ");
  Serial.println(SSID);
  
  if (!WiFly.join(SSID, PASSPHRASE, true)) {
    Serial.println("Association failed...");
    while (1) {
      // Hang on failure.
    }
  }
  
  Serial.println("Joined!");
  Serial.println();
  
  Serial.println("Connecting to: Notifyr...");

  if (client.connect()) {
    Serial.println("Connected!\n");
    client.println("GET /subscribe/arduino?key=CpBTzZAqxDRDVlLJJEKqOih0JZhmGdPA4F8Zbyr2ByA&always_ok.1");
    client.println("Connection: keep-alive");
    client.println("Host: api.notifyr.io");
    client.println();
  } else {
    Serial.println("Connection failed");
  }
  
}

void loop() {
  if (client.available()) {
    char c = client.read();
    if (c == '\n') {
      buffer = "";
    }
    
    if (c != ' ' || c != lastChar) {
//      Serial.print(c);
      
      if (dataReceiving && c == '"') {
        //do thing
        Serial.print("Data: ");
        Serial.println(buffer);
        if (buffer == "true" || buffer == "1" || buffer == "on" || buffer == "high") {
          digitalWrite(PIN_POWER, HIGH);
          digitalWrite(PIN_POWER_STATUS, HIGH);
        } else {
          digitalWrite(PIN_POWER, LOW);
          digitalWrite(PIN_POWER_STATUS, LOW);
        }
        
        //reset
        buffer = "";
        dataReceiving = false;
      }
      
      buffer += c;
      
      if (buffer.endsWith("data: \"")) {
        Serial.println("\nReceiving Event...");
        buffer = "";
        dataReceiving = true;
      } 
    }
    lastChar = c;
  }
  
  if (!client.connected()) {
    Serial.println();
    Serial.println("disconnecting.");
    client.stop();
    for(;;)
      ;
  }
}


