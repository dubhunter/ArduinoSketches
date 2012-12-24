
#include <SPI.h>
#include <Ethernet.h>

// Enter a MAC address for your controller below.
// Newer Ethernet shields have a MAC address printed on a sticker on the shield
byte mac[] = { 0x00, 0xAA, 0xBB, 0xCC, 0xDE, 0x02 };
IPAddress serverDev(174,129,254,120); // headsup.dev.twilio.com (public)
//IPAddress serverDev(10,72,137,140); // headsup.dev.twilio.com (private)
IPAddress serverStage(23,23,247,85); // headsup.stage.twilio.com (public)
//IPAddress serverStage(10,192,135,193); // headsup.stage.twilio.com (private)
IPAddress serverProd(23,23,203,170); // headsup.twilio.com (public)
//IPAddress serverProd(10,204,38,226); // headsup.twilio.com (private)

String hostDev = String("headsup.dev.twilio.com");
String hostStage = String("headsup.stage.twilio.com");
String hostProd = String("headsup.twilio.com");

EthernetClient clientDev;
EthernetClient clientStage;
EthernetClient clientProd;

const int LED_PIN = 6;
const int SWITCH_PIN = 7;

String ENDPOINT = String("/v1/curtain");

// Variables will change:
int switchState = -1;             // the current reading from the input pin
int lastSwitchState = -1;   // the previous reading from the input pin

boolean clientConnectedDev = false;
boolean clientConnectedStage = false;
boolean clientConnectedProd = false;


String method;

// the following variables are long's because the time, measured in miliseconds,
// will quickly become a bigger number than can be stored in an int.
long lastDebounceTime = 0;  // the last time the output pin was toggled
long debounceDelay = 200;    // the debounce time; increase if the output flickers

void setup() {
  pinMode(LED_PIN, OUTPUT);
  pinMode(SWITCH_PIN, INPUT);
  digitalWrite(SWITCH_PIN, HIGH);    // enable pull-up reisister
  
  Serial.begin(9600);
  
  Serial.println("Starting...");

  if (Ethernet.begin(mac) == 0) {
    Serial.println("Failed to configure Ethernet using DHCP");
    // no point in carrying on, so do nothing forevermore:
    for(;;)
      ;
  }
    
  // print your local IP address:
  Serial.print("Joined: ");
  Serial.print(Ethernet.localIP());
  Serial.println();
  
  // give the Ethernet shield a second to initialize:
  delay(3000);
  
  Serial.println("Started!");
}

void loop() {
  // read the state of the switch into a local variable:
  int reading = digitalRead(SWITCH_PIN);

//  // If the switch changed, due to noise or pressing:
//  if (reading != lastSwitchState) {
//    // reset the debouncing timer
//    lastDebounceTime = millis();
//  } 
//  
//  if ((millis() - lastDebounceTime) > debounceDelay) {
//    // whatever the reading is at, it's been there for longer
//    // than the debounce delay, so take it as the actual current state:
//    switchState = reading;
////    Serial.println(reading);
//  }
    switchState = reading;
  
  if (switchState != lastSwitchState) {
    if (switchState == HIGH) {
      Serial.println("Putting up the curtain...");
      digitalWrite(LED_PIN, LOW);
      method = "PUT";
    } else {
      Serial.println("Tearing down the curtain...");
      digitalWrite(LED_PIN, HIGH);
      method = "DELETE";
    }
    
    Serial.println(method + " " + ENDPOINT + " HTTP/1.1");
    
    if (!clientConnectedDev && clientDev.connect(serverDev, 80)) {
      Serial.println("[dev] connected!");
      clientConnectedDev = true;
      //Make the request
      clientDev.println(method + " " + ENDPOINT + " HTTP/1.1");
      clientDev.println("Host: " + hostDev);
      clientDev.println("Connection: close");
      clientDev.println("Content-length: 0");
      clientDev.println();
      if (method == "PUT") {
          clientDev.println();
      }
    } else {
      Serial.println("[dev] connection failed :(");
      clientConnectedDev = false;
    }
    
    if (!clientConnectedStage && clientStage.connect(serverStage, 80)) {
      Serial.println("[stage] connected!");
      clientConnectedStage = true;
      //Make the request
      clientStage.println(method + " " + ENDPOINT + " HTTP/1.1");
      clientStage.println("Host: " + hostStage);
      clientStage.println("Connection: close");
      clientStage.println("Content-length: 0");
      clientStage.println();
      if (method == "PUT") {
          clientStage.println();
      }
    } else {
      Serial.println("[stage] connection failed :(");
      clientConnectedStage = false;
    }
    
//    if (!clientConnectedProd && clientProd.connect(serverProd, 80)) {
//      Serial.println("[prod] connected!");
//      clientConnectedProd = true;
//      //Make the request
//      clientProd.println(method + " " + ENDPOINT + " HTTP/1.1");
//      clientProd.println("Host: " + hostProd);
//      clientProd.println("Connection: close");
//      clientProd.println("Content-length: 0");
//      clientProd.println();
//      if (method == "PUT") {
//          clientProd.println();
//      }
//    } else {
//      Serial.println("[prod] connection failed :(");
//      clientConnectedProd = false;
//    }
  }

  if (clientDev.available()) {
    char c = clientDev.read();
    Serial.print(c);
  }
  
  if (clientConnectedDev && !clientDev.connected()) {
    clientDev.stop();
    clientConnectedDev = false;
  }

  if (clientStage.available()) {
    char c = clientStage.read();
    Serial.print(c);
  }
  
  if (clientConnectedStage && !clientStage.connected()) {
    clientStage.stop();
    clientConnectedStage = false;
  }

//  if (clientProd.available()) {
//    char c = clientProd.read();
//    Serial.print(c);
//  }
//  
//  if (clientConnectedProd && !clientProd.connected()) {
//    clientProd.stop();
//    clientConnectedProd = false;
//  }

  // save the reading.  Next time through the loop,
  // it'll be the lastSwitchState:
  lastSwitchState = reading;
}

