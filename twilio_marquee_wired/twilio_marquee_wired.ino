#include <HT1632.h>

#include <SPI.h>
#include <Ethernet.h>

//#define DATA A5
//#define WR   A4
//#define CS   A3
//#define CS2  A2
//#define CS3  A1
//#define CS4  A0
#define DATA 3
#define WR   5
#define CS   6
#define CS2  7
#define CS3  8
#define CS4  9
#define CS5  A2
#define CS6  A3
#define CS7  A4
#define CS8  A5

byte mac[] = { 0x90, 0xA2, 0xDB, 0x00, 0x00, 0x02 };
byte server[] = { 70,32,87,92 };
String host = String("sign.willmason.me");
int port = 80;
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

const int CHAR_WIDTH = 32;


EthernetClient client;

// use this line for single matrix
//HT1632LEDMatrix matrix = HT1632LEDMatrix(DATA, WR, CS, CS2, CS3, CS4);
//HT1632LEDMatrix matrix = HT1632LEDMatrix(DATA, WR, CS, CS2, CS3, CS4, CS5);
//HT1632LEDMatrix matrix = HT1632LEDMatrix(DATA, WR, CS, CS2, CS3, CS4, CS5, CS6);
//HT1632LEDMatrix matrix = HT1632LEDMatrix(DATA, WR, CS, CS2, CS3, CS4, CS5, CS6, CS7);
HT1632LEDMatrix matrix = HT1632LEDMatrix(DATA, WR, CS, CS2, CS3, CS4, CS5, CS6, CS7, CS8);

void setup() {
    Serial.begin(9600);
    
//    pinMode(CS, OUTPUT);
//    pinMode(CS2, OUTPUT);
//    pinMode(CS3, OUTPUT);
//    pinMode(CS4, OUTPUT);
//    pinMode(CS5, OUTPUT);
//    pinMode(CS6, OUTPUT);
//    pinMode(CS7, OUTPUT);
//    pinMode(CS8, OUTPUT);
    
    matrix.begin(HT1632_COMMON_16NMOS);
//  
    matrix.clearScreen();
    matrix.setTextSize(2);
    matrix.setTextColor(1);
    matrix.setCursor(0, 0);   // start at top left, with one pixel of spacing
    matrix.print("TWILIO SCALES!!!");
    matrix.writeScreen();
    
    matrix.setTextSize(1);
    
//    matrix.clearScreen();
//    matrix.setCursor(0, 0);    
//    matrix.print("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnop");
//    matrix.writeScreen();

    Serial.println("Joining network");
    
    //start the Ethernet connection
    if (Ethernet.begin(mac) == 0)
    {
        Serial.println("Failed to connect to the network via DHCP");
//        matrix.setCursor(0, 0);    
//        matrix.print("NETWORK ERROR!");
//        matrix.writeScreen();
        for(;;) ;
    }
    
    Serial.println("Joined: " + Ethernet.localIP());
}

void loop() {
    
    //if not transferring, try to connect
    if(!isTransferring && (lastPollMillis == 0 || millis() - lastPollMillis >= POLL_INTERVAL))
    {
        if (client.connect(server, port))
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
            if (charCounter++ % CHAR_WIDTH == 0)
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

        response.trim();
        
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
