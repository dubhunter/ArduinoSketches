
// (Based on Ethernet's WebClient Example)

#include <SPI.h>
#include <WiFly.h>

#include "Credentials.h"

WiFlyClient client("ws.pusherapp.com", 80);

void setup() {
  
  Serial.begin(9600);

  WiFly.begin();
  
  if (!WiFly.join(SSID, PASSPHRASE)) {
    Serial.println("Association failed.");
    while (1) {
      // Hang on failure.
    }
  }  

  Serial.println("connecting...");

  if (client.connect()) {
    Serial.println("connected");
    client.println("GET /app/562d02c947852152616a?client=js&version=1.9.0 HTTP/1.1");
    client.println("Upgrade: WebSocket");
    client.println("Connection: Upgrade");
    client.println("Host: ws.pusherapp.com");
    client.println("Origin: ArduinoWiFlyWebSocketClient");
    client.println();
  } else {
    Serial.println("connection failed");
  }
  
}

void loop() {
  if (client.available()) {
    char c = client.read();
    Serial.print(c);
  }
  
  if (!client.connected()) {
    Serial.println();
    Serial.println("disconnecting.");
    client.stop();
    for(;;)
      ;
  }
}


