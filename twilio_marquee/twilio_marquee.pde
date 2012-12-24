#include <HT1632.h>

#include <SPI.h>
#include <WiFly.h>

#include "Credentials.h"

#define DATA 2
#define WR   3
#define CS   4
#define CS2  5
#define CS3  6
#define CS4  7
#define CS5  A0
#define CS6  A1
#define CS7  A2
#define CS8  A3

byte server[] = { 70,32,87,92 };
String host = String("sign.willmason.me");
String endpoint = String("/");
String response = String();
String sms = String();
boolean isTransferring = false;
boolean significantData = false;
unsigned long lastPollMillis = 0;
unsigned long charCounter = 0;
//api poll interval
const int POLL_INTERVAL = 2000;
//response flag chars
const char DATA_BEGIN = '?';

// use this line for single matrix
//HT1632LEDMatrix matrix = HT1632LEDMatrix(DATA, WR, CS, CS2, CS3, CS4);
HT1632LEDMatrix matrix = HT1632LEDMatrix(DATA, WR, CS, CS2, CS3, CS4, CS5, CS6, CS7, CS8);

Client client(server, 80);

void setup() {
    Serial.begin(9600);
    
    pinMode(CS, OUTPUT);
    pinMode(CS2, OUTPUT);
    pinMode(CS3, OUTPUT);
    pinMode(CS4, OUTPUT);
    pinMode(CS5, OUTPUT);
    pinMode(CS6, OUTPUT);
    pinMode(CS7, OUTPUT);
    pinMode(CS8, OUTPUT);
    
    matrix.begin(HT1632_COMMON_16NMOS);
  
    matrix.clearScreen();
    matrix.setTextSize(2);
    matrix.setTextColor(1);
    matrix.setCursor(0, 0);   // start at top left, with one pixel of spacing
    matrix.print("TWILIO SCALES!!!");
    matrix.writeScreen();
    
    matrix.setTextSize(1);
    
    matrix.clearScreen();
    matrix.setCursor(0, 0);    
    matrix.print("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnop");
    matrix.writeScreen();
    
    //start the WiFly connection
    WiFly.begin();
  
    Serial.print("Attempting to join: ");
    Serial.println(ssid);
  
    if (!WiFly.join(ssid, passphrase, true)) {
        Serial.println("Association failed.");
        matrix.print("NTWRK ERR");
        while (1) {
          // Hang on failure.
        }
    }
}

void loop() {
    
    //if not transferring, try to connect
    if(!isTransferring && (lastPollMillis == 0 || millis() - lastPollMillis >= POLL_INTERVAL))
    {
        if (client.connect())
        {
            Serial.println("connected!");
            
            client.println("GET " + endpoint + " HTTP/1.1");
            client.println("Host: " + host);
            client.println();
            isTransferring = true;
            significantData = false;
            charCounter = 0;
        }
        else
        {
            isTransferring = false; 
        }
    }

    //read data when once available (we will prob loop past this while request is happening)
    while (isTransferring && client.available())
    {
        // collect server response
        char c = client.read();
        
        //append to reponse for later parsing
        if (significantData)
        {
            if (charCounter++ % 32 == 0)
            {
                response += "\n";
            }
            response += c;
        }
        
        if (c == DATA_BEGIN)
        {
            significantData = true;
        }
    }

    //if the client disconnects while transferring, we are done...process data and reset
    if (isTransferring && !client.connected())
    {
        client.stop();
        
        isTransferring = false;
        significantData = false;

        Serial.println("disconnected...");
        
        Serial.println(response);

        response = response.trim();
        
        Serial.println("-----------------");
        
        if (response != sms)
        {
            sms = response;
            
            Serial.println("Clearing screen");
            matrix.clearScreen();
            Serial.println("Setting cursor");
            matrix.setCursor(0, 0);
            
            Serial.println("Printing");
            matrix.print(sms);
            
            Serial.println("Writing...");
            matrix.writeScreen();
        }

        //reset response data
        response = String();
        
        //set a time reference or async-operation
        lastPollMillis = millis();
    }
    
    //handle millis() overflow
    if (millis() < lastPollMillis)
        lastPollMillis = 0;
}
