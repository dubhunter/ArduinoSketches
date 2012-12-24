#include <HT1632.h>

#include <SPI.h>
#include <Ethernet.h>

//#define DATA A5
//#define WR   A4
//#define CS   A3
//#define CS2  A2
//#define CS3  A1
//#define CS4  A0
#define DATA1 22
#define WR1   23
#define CS1   28
#define CS2   29
#define CS3   30
#define CS4   31
#define CS5   32
#define CS6   33
#define CS7   34
#define CS8   35

#define DATA2 24
#define WR2   25
#define CS9   36
#define CS10  37
#define CS11  38
#define CS12  39
#define CS13  40
#define CS14  41
#define CS15  42
#define CS16  43

#define DATA3 26
#define WR3   27
#define CS17  44
#define CS18  45
#define CS19  46
#define CS20  47
#define CS21  48
#define CS22  49
#define CS23  8
#define CS24  9

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
//HT1632LEDMatrix matrix = HT1632LEDMatrix(DATA, WR, CS, CS2, CS3, CS4, CS5, CS6, CS7, CS8);


HT1632LEDMatrix matrix1 = HT1632LEDMatrix(DATA1, WR1, CS1, CS2, CS3, CS4, CS5, CS6, CS7);
HT1632LEDMatrix matrix2 = HT1632LEDMatrix(DATA2, WR2, CS9, CS10, CS11, CS12, CS13, CS14, CS15);
HT1632LEDMatrix matrix3 = HT1632LEDMatrix(DATA3, WR3, CS17, CS18, CS19, CS20, CS21, CS22, CS23);

void setup() {
    Serial.begin(9600);
    
//    pinMode(CS1, OUTPUT);
//    pinMode(CS2, OUTPUT);
//    pinMode(CS3, OUTPUT);
//    pinMode(CS4, OUTPUT);
//    pinMode(CS5, OUTPUT);
//    pinMode(CS6, OUTPUT);
//    pinMode(CS7, OUTPUT);
//    pinMode(CS8, OUTPUT);
//    pinMode(CS9, OUTPUT);
//    pinMode(CS10, OUTPUT);
//    pinMode(CS11, OUTPUT);
//    pinMode(CS12, OUTPUT);
//    pinMode(CS13, OUTPUT);
//    pinMode(CS14, OUTPUT);
//    pinMode(CS15, OUTPUT);
//    pinMode(CS16, OUTPUT);
//    pinMode(CS17, OUTPUT);
//    pinMode(CS18, OUTPUT);
//    pinMode(CS19, OUTPUT);
//    pinMode(CS20, OUTPUT);
//    pinMode(CS21, OUTPUT);
//    pinMode(CS22, OUTPUT);
//    pinMode(CS23, OUTPUT);
//    pinMode(CS24, OUTPUT);

    matrix1.begin(HT1632_COMMON_16NMOS);
    matrix2.begin(HT1632_COMMON_16NMOS);
    matrix3.begin(HT1632_COMMON_16NMOS);
  
    Serial.println("Writing to matrix1");
    matrix1.clearScreen();
    matrix1.setTextSize(2);
    matrix1.setTextColor(1);
    matrix1.setCursor(0, 0);
    matrix1.print("TWILIO SCALES...");
//    matrix1.print("1234567890ABCDEF");
    matrix1.writeScreen();  
    matrix1.setTextSize(1);
  
    Serial.println("Writing to matrix2");
    matrix2.clearScreen();
    matrix2.setTextSize(2);
    matrix2.setTextColor(1);
    matrix2.setCursor(0, 0);
    matrix2.print("ROW 2 OH YEA");
//    matrix2.print("1234567890ABCDEF");
    matrix2.writeScreen();
    matrix2.setTextSize(1);
  
    Serial.println("Writing to matrix3");
    matrix3.clearScreen();
    matrix3.setTextSize(2);
    matrix3.setTextColor(1);
    matrix3.setCursor(0, 0);
    matrix3.print("ROW 3 BOOM");
    matrix3.writeScreen();
    matrix3.setTextSize(1);
    
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
//        matrix.print("NETWORK ERROR");
//        matrix.writeScreen();
        for(;;) ;
    }
    
    Serial.println("Joined: " + Ethernet.localIP());
}

void loop() {
    
    for(;;) ;
//    //if not transferring, try to connect
//    if(!isTransferring && (lastPollMillis == 0 || millis() - lastPollMillis >= POLL_INTERVAL))
//    {
//        if (client.connect(server, port))
//        {
//            Serial.println("connected");
//            
//            client.println("GET " + endpoint + " HTTP/1.1");
//            client.println("Host: " + host);
//            client.println();
//            isTransferring = true;
//            significantData = false;
//            charCounter = 0;
//        }
//        else
//        {
//            isTransferring = false; 
//        }
//    }
//
//    //read data when once available (we will prob loop past this while request is happening)
//    while (isTransferring && client.available())
//    {
//        // collect server response
//        char c = client.read();
//        
//        //append to reponse for later parsing
//        if (significantData)
//        {
//            if (charCounter++ % CHAR_WIDTH == 0)
//            {
//                response += "\n";
//            }
//            response += c;
//        }
//        
//        if (c == DATA_BEGIN)
//        {
//            significantData = true;
//        }
//    }
//
//    //if the client disconnects while transferring, we are done...process data and reset
//    if (isTransferring && !client.connected())
//    {
//        client.stop();
//        
//        isTransferring = false;
//        significantData = false;
//
//        Serial.println("disconnected...");
//        
//        Serial.println(response);
//
//        response.trim();
//        
//        Serial.println("-----------------");
//        
//        if (response != sms)
//        {
//            sms = response;
//            
//            Serial.println("Clearing screen");
//            matrix.clearScreen();
//            Serial.println("Setting cursor");
//            matrix.setCursor(0, 0);
//            
//            Serial.println("Printing");
//            matrix.print(sms);
//            
//            Serial.println("Writing...");
//            matrix.writeScreen();
//        }
//
//        //reset response data
//        response = String();
//        
//        //set a time reference or async-operation
//        lastPollMillis = millis();
//    }
//    
//    //handle millis() overflow
//    if (millis() < lastPollMillis)
//        lastPollMillis = 0;
}
