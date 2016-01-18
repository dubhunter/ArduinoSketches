#include <SPI.h>
#include <Ethernet.h>
#include <Metro.h>
#include <AlphaNumeric_Driver.h>
#include <ArduinoJson.h>
#include <Time.h>
#include "pitches.h"

//app settings
const boolean DEBUG = false;
const String NAME = String("Hunter Light");
const String VERSION = String("1.0");
//local network settings
byte mac[] = { 0x90, 0xA2, 0xDB, 0x00, 0x00, 0x01 };
//server network settings
byte server[] = { 54,243,227,22 }; // dubhunter-aws 54.243.227.22
String host = String("hunter.willandchi.com");
int port = 80;
String endpoint = String("/v1/light");
//colors
int colorRed[] = { 0xff, 0x00, 0x00 };
int colorPink[] = { 0xff, 0x4a, 0x9e };
int colorPurple[] = { 0xff, 0x00, 0xff };
int colorBlue[] = { 0x00, 0x6f, 0xff };
int colorAqua[] = { 0xab, 0x9f, 0xbe };
int colorGreen[] = { 0xa0, 0xff, 0x00 };
int colorYellow[] = { 0xff, 0xd0, 0x00 };
int colorOrange[] = { 0xff, 0xa1, 0x00 };
int colorGray[] = { 0xc7, 0x94, 0x00 };
//tones
int notesNew[] = { NOTE_G6, NOTE_G7, NOTE_C8 };
int notesChange[] = { NOTE_A6, NOTE_A7, NOTE_A6, NOTE_A7 };
int noteLen = 1000 / 16;
//variables
String response = String();
String days = String();
String temp = String();
String lastColor = String();
boolean isBody = false;
boolean isTransferring = false;
boolean isRainbow = false;
int displayMode = 0;
int lightRed = 0;
int lightGreen = 0;
int lightBlue = 0;
int rainbowPhase = 0;

//define the BUZZER Pin
const int PIN_BUZZ = 9;
//define the LED Pins
const int PIN_R = 3;
const int PIN_G = 5;
const int PIN_B = 6;
//define ALPHA/NUM Pins
const int PIN_SDI = 8;
const int PIN_CLK = 7;
const int PIN_LE = 4;
const int PIN_OE = 2;
const int NUM_DISPLAYS = 7;
//api poll interval
const int FLASH_INTERVAL = 250;
const int FLASH_COUNT = 3;
const int SCROLL_INTERVAL = 200;
const int POLL_INTERVAL = 2000;
const int DISPLAY_INTERVAL = 3300;
const int RAINBOW_INTERVAL = 20;
//display modes
const int MODE_DAYS = 0;
const int MODE_CLOCK = 1;
const int MODE_TEMP = 2;
//response keys
const String DATA_COLOR = String("color");
const String DATA_DAYS = String("days");
const String DATA_TEMP = String("temp");
const String DATA_TIME = String("time");
const String DATA_OFFSET = String("offset");
//json settings
const int BUFFER_SIZE = JSON_OBJECT_SIZE(5);

Metro pollMetro = Metro(POLL_INTERVAL);
Metro displayMetro = Metro(DISPLAY_INTERVAL);
Metro rainbowMetro = Metro(RAINBOW_INTERVAL);

// Initialize the Ethernet client library
// with the IP address and port of the server 
// that you want to connect to (port 80 is default for HTTP):
EthernetClient client;

alphaNumeric myDisplay(PIN_SDI, PIN_CLK, PIN_LE, PIN_OE, NUM_DISPLAYS);

void setup() {
    //set outputs and turn off
    pinMode(PIN_BUZZ, OUTPUT);
    pinMode(PIN_R, OUTPUT);
    pinMode(PIN_G, OUTPUT);
    pinMode(PIN_B, OUTPUT);
    
    //start the serial library
    if (DEBUG) {
        Serial.begin(57600);
        while (!Serial) {
          ; // wait for serial port to connect. Needed for native USB port only
        }
    }
    
    sLog("Booting...");

    //give the ethernet module time to boot up:
    delay(1000);
    
    //start the Ethernet connection
    sLog("Initializing network...");
    if (Ethernet.begin(mac) == 0) {
        sLog("Failed to connect to the network via DHCP");
        scrollString("NTWKERR");
        for(;;) ;
    }
    
    //powerup cycle
    sLog("Showing off...");
//    playTone(notesNew, 3);
    scrollString(NAME + " v" + VERSION);
    handleColor("rainbow");
    
    sLog("Complete...");
}

void loop() {
    pollServer();
    handleDisplay();
    handleRainbow();
}

void pollServer() {
    //if not transferring, try to connect
    if(!isTransferring && pollMetro.check()) {
        sLog("Connecting...");
        if (client.connect(server, port)) {
            sLog("Connected!");
            
            client.println("GET " + endpoint + " HTTP/1.1");
            client.println("Host: " + host);
            client.println();
            isTransferring = true;
        } else {
            sLog("Failed :(");
            isTransferring = false;
        }
    }

    //read data when once available (we will prob loop past this while request is happening)
    while (client.available()) {
        // collect server response
        char c = client.read();
        if (c == '{') {
            isBody = true;
        }
        if (isBody) {
            //append to reponse for later parsing
            response += c;
        }
        if (c == '}') {
            isBody = false;
            client.stop();
        }
    }

    //if the client disconnects while transferring, we are done...process data and reset
    if (isTransferring && !client.connected()) {
        client.stop();
        isTransferring = false;
        
        sLog("Disconnected...");
        
        handleResponse(response);
        response = String();
        
        pollMetro.reset();
    }
}

void handleResponse(String response) {
    sLog("handleResponse:");
    sLog(response);
    response.trim();
    
    StaticJsonBuffer<BUFFER_SIZE> jsonBuffer;
    JsonObject& data = jsonBuffer.parseObject(stringToChars(response));
    
    if (!data.success()) {
        sLog("parseObject() failed");
    }
    
    handleColor(data[DATA_COLOR]);
    setDays(data[DATA_DAYS]);
    setTemp(data[DATA_TEMP]);
    setTime(stringToTime(data[DATA_TIME]));
    adjustTime(data[DATA_OFFSET]);
}

void handleDisplay() {
    //swap between clock and users
    if (days.length() > 0 && displayMetro.check()) {
        switch (displayMode) {
            case MODE_DAYS:
                displayString(getDays() + "days");
                displayMode = MODE_CLOCK;
                break;
            case MODE_CLOCK:
                displayString(getClock());
                displayMode = MODE_TEMP;
                break;
            case MODE_TEMP:
                displayString(getTemp() + "~F ");
                displayMode = MODE_DAYS;
                break;
        }
        displayMetro.reset();
    }
}

void handleColor(String color) {
    sLog("handleColor:");
    sLog(color);
    
    if (color == lastColor) {
        return;
    }
    
    isRainbow = false;
    if (color == "rainbow") {
        isRainbow = true;
        lightRed = 255;
        lightGreen = 0;
        lightBlue = 0;
        rainbowPhase = 0;
        rainbowMetro.reset();
    } else if (color == "red") {
        lightRed = colorRed[0];
        lightGreen = colorRed[1];
        lightBlue = colorRed[2];
    } else if (color == "pink") {
        lightRed = colorPink[0];
        lightGreen = colorPink[1];
        lightBlue = colorPink[2];
    } else if (color == "purple") {
        lightRed = colorPurple[0];
        lightGreen = colorPurple[1];
        lightBlue = colorPurple[2];
    } else if (color == "blue") {
        lightRed = colorBlue[0];
        lightGreen = colorBlue[1];
        lightBlue = colorBlue[2];
    } else if (color == "aqua") {
        lightRed = colorAqua[0];
        lightGreen = colorAqua[1];
        lightBlue = colorAqua[2];
    } else if (color == "green") {
        lightRed = colorGreen[0];
        lightGreen = colorGreen[1];
        lightBlue = colorGreen[2];
    } else if (color == "yellow") {
        lightRed = colorYellow[0];
        lightGreen = colorYellow[1];
        lightBlue = colorYellow[2];
    } else if (color == "orange") {
        lightRed = colorOrange[0];
        lightGreen = colorOrange[1];
        lightBlue = colorOrange[2];
    } else if (color == "gray") {
        lightRed = colorGray[0];
        lightGreen = colorGray[1];
        lightBlue = colorGray[2];
    } else {
        lightRed = 0;
        lightGreen = 0;
        lightBlue = 0;
    }

    lightColor();
    
    lastColor = color;
}

void handleRainbow() {
    if (!isRainbow || !rainbowMetro.check()) {
        return;
    }

    switch (rainbowPhase) {
        case 0: // ff0000 -> ff00ff
            if (lightBlue < 255) {
                lightBlue++;
            } else {
                rainbowPhase = 1;
            }
            break;
        case 1: // ff00ff -> 0000ff
            if (lightRed > 0) {
                lightRed--;
            } else {
                rainbowPhase = 2;
            }
            break;
        case 2: // 0000ff -> 00ffff
            if (lightGreen < 255) {
                lightGreen++;
            } else {
                rainbowPhase = 3;
            }
            break;
        case 3: // 00ffff -> 00ff00
            if (lightBlue > 0) {
                lightBlue--;
            } else {
                rainbowPhase = 4;
            }
            break;
        case 4: // 00ff00 -> ffff00
            if (lightRed < 255) {
                lightRed++;
            } else {
                rainbowPhase = 5;
            }
            break;
        case 5: // ffff00 -> ff0000
            if (lightGreen > 0) {
                lightGreen--;
            } else {
                rainbowPhase = 0;
            }
            break;
    }

    lightColor();
    rainbowMetro.reset();
}

void setDays(String d) {
    sLog("setDays:");
    sLog(d);
    days = d;
}

String getDays() {
    return days;
}

void setTemp(String t) {
    sLog("setTemp:");
    sLog(t);
    temp = t;
}

String getTemp() {
    return temp;
}

String getClock() {
    time_t t = now();
    String c = String();
    int tmp = hourFormat12(t);
//    if (tmp < 10) {
//        c += "0";
//    }
    c += String(tmp) + ":";
    tmp = minute(t);
    if (tmp < 10) {
        c += "0";
    }
    c += String(tmp) + " ";
    if (isAM(t)) {
        c += "A";
    } else {
        c += "P";
    }
    return c;
}

time_t stringToTime(String s) {
    sLog("stringToTime:");
    sLog(s);
    char arr[12];
    time_t t = 0;
    s.toCharArray(arr, sizeof(arr));
    for (int i = 0; i < 10; i++) {
        if (arr[i] >= '0' && arr[i] <= '9') {
            t = (10 * t) + (arr[i] - '0'); // convert digits to a number
        }
    }
//    sLog(t);
    return t;
}

long stringToLong(String s) {
    char arr[12];
    s.toCharArray(arr, sizeof(arr));
    return atol(arr);
}

char * stringToChars(String s) {
    char arr[128];
    s.toCharArray(arr, sizeof(arr));
    return arr;
}

void displayString(String s) {
    sLog("displayString:");
    sLog(s);
    myDisplay.off();
    myDisplay.clear();
    myDisplay.scroll(stringToChars(s), 0);
    myDisplay.on();
}

void scrollString(String s) {
    sLog("scrollString:");
    sLog(s);
    myDisplay.off();
    myDisplay.clear();
    myDisplay.on();
    myDisplay.scroll(stringToChars(s), SCROLL_INTERVAL);
}

void displayFlash() {
    for (int i = 0; i < FLASH_COUNT; i++) {
        myDisplay.off();
        delay(FLASH_INTERVAL);
        myDisplay.on();
        delay(FLASH_INTERVAL);
    }
}

void playTone(int *notes, int n) {
    for (int i = 0; i < n; i++) {
        tone(PIN_BUZZ, notes[i], noteLen);
        int breath = noteLen * 1.30;
        delay(breath);
        noTone(PIN_BUZZ);
    }
}

int gammaCorrect(int channel) {
    return channel == 0 ? 0 : pow(255, ((channel + 1) / 4) / 64.0) + 0.5;
}

void lightOff() {
    isRainbow = false;

    lightRed = 0;
    lightGreen = 0;
    lightBlue = 0;

    lightColor();
}

void lightColor() {
    analogWrite(PIN_R, gammaCorrect(lightRed));
    analogWrite(PIN_G, gammaCorrect(lightGreen));
    analogWrite(PIN_B, gammaCorrect(lightBlue));
}

void sLog(String s) {
    if (DEBUG) {
        Serial.println(s);
    }
}
