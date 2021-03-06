#include <SPI.h>
#include <Ethernet.h>
//#define FLIPDISPLAY
#include <AlphaNumeric_Driver.h>
#include <Time.h>
#include "pitches.h"

//app settings
const boolean DEBUG = false;
const String NAME = String("Vibelight");
const String VERSION = String("1.6");
//local network settings
byte mac[] = { 0x90, 0xA2, 0xDB, 0x00, 0x00, 0x01 };
//server network settings
byte server[] = { 54,243,227,22 }; // vibefeelr 54.243.227.22
String host = String("api.vibe.me");
int port = 80;
String endpoint = String("/vibelight/");
String after = String();
String users = String();
String response = String();
//variables
boolean isTransferring = false;
boolean isClock = false;
unsigned long lastPollMillis = 0;
unsigned long lastDisplayMillis = 0;
//colors
int colorPurple[] = { 0xff, 0x00, 0xff };
int colorBlue[] = { 0x00, 0x6f, 0xff };
int colorGreen[] = { 0xa0, 0xff, 0x00 };
int colorYellow[] = { 0xff, 0xd0, 0x00 };
int colorOrange[] = { 0xff, 0xa1, 0x00 };
int colorRed[] = { 0xff, 0x00, 0x00 };
int colorBrown[] = { 0xd8, 0x9b, 0x4f };
int colorGray0[] = { 0xc7, 0x94, 0x00 };
int colorGray1[] = { 0xab, 0x7d, 0x4f };
int colorGray2[] = { 0xc7, 0xa2, 0x85 };
int colorPink[] = { 0xff, 0x4a, 0x9e };
int colorLightBlue[] = { 0xab, 0x9f, 0xbe };
//tones
int notesNew[] = { NOTE_G6, NOTE_G7, NOTE_C8 };
int notesSvn[] = { NOTE_A6, NOTE_A7, NOTE_A6, NOTE_A7 };
int noteLen = 1000 / 16;

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
const int POLL_INTERVAL = 2000;
const int FLASH_INTERVAL = 250;
const int FLASH_COUNT = 3;
const int SCROLL_INTERVAL = 200;
const int CLOCK_INTERVAL = 2000;
const int USERS_INTERVAL = 2000;
const time_t GMT_OFFSET = -25200;
//response flag chars
const char DATA_BEGIN = '?=';
const char DATA_DELIM = ';';
const char DATA_RECENT = '$';
const char DATA_AFTER_TIME = '@';
const char DATA_AGG = '&';
const char DATA_USERS = '#';
const char DATA_VER_WEB = '%';
const char DATA_VER_API = '*';
const char DATA_VER_DEL = '!';

// Initialize the Ethernet client library
// with the IP address and port of the server 
// that you want to connect to (port 80 is default for HTTP):
EthernetClient client;

alphaNumeric myDisplay(PIN_SDI, PIN_CLK, PIN_LE, PIN_OE, NUM_DISPLAYS);

void setup()
{
    //set outputs and turn off
    pinMode(PIN_BUZZ, OUTPUT);
    pinMode(PIN_R, OUTPUT);
    pinMode(PIN_G, OUTPUT);
    pinMode(PIN_B, OUTPUT);
    
    //start the serial library
    if (DEBUG)
        Serial.begin(57600);
    
    //start the Ethernet connection
    if (Ethernet.begin(mac) == 0)
    {
        sLog("Failed to connect to the network via DHCP");
        scrollString("NTWKERR");
        for(;;) ;
    }
    
    //powerup cycle
    scrollString(NAME + " v" + VERSION + " -> " + "vibe.me");
    for (int i = 1; i <= 12; i++)
    {
        vibeColor(i);
        delay(FLASH_INTERVAL);
    }
    allOff();
}

void loop()
{
    //if not transferring, try to connect
    if(!isTransferring && (lastPollMillis == 0 || millis() - lastPollMillis >= POLL_INTERVAL))
    {
        if (client.connect(server, port))
        {
//            sLog("connected!");
            
            client.println("GET " + endpoint + "?after=" + after + " HTTP/1.1");
            client.println("Host: " + host);
            client.println();
            isTransferring = true;
        }
        else
        {
            isTransferring = false; 
        }
    }

    //read data when once available (we will prob loop past this while request is happening)
    while (client.available())
    {
        // collect server response
        char c = client.read();
        //append to reponse for later parsing
        response += c;
    }

    //if the client disconnects while transferring, we are done...process data and reset
    if (isTransferring && !client.connected())
    {
        client.stop();
        isTransferring = false;

//        sLog(response);
//        sLog("disconnected...");

        response = response.substring(response.indexOf(DATA_BEGIN) + 1);
        response.trim();

        if (response.indexOf(DATA_RECENT) > -1)
        {
            int i1 = response.indexOf(DATA_RECENT);
            int i2 = response.indexOf(DATA_DELIM);
            long recentVibe = stringToLong(response.substring(i1 + 1, i2));
            sLog("Recent Vibe: " + recentVibe);
            for (int i = 0; i < FLASH_COUNT; i++)
            {
                vibeColor(recentVibe);
                delay(FLASH_INTERVAL);
                allOff();
                delay(FLASH_INTERVAL);
            }
            response = response.substring(i2 + 1);
        }

        if (response.indexOf(DATA_AFTER_TIME) > -1)
        {
            int i1 = response.indexOf(DATA_AFTER_TIME);
            int i2 = response.indexOf(DATA_DELIM);
            after = response.substring(i1 + 1, i2);
            setTime(stringToTime(after));
            sLog("After Time: " + after);
            response = response.substring(i2 + 1);
        }

        if (response.indexOf(DATA_AGG) > -1)
        {
            int i1 = response.indexOf(DATA_AGG);
            int i2 = response.indexOf(DATA_DELIM);
            long aggVibe = stringToLong(response.substring(i1 + 1, i2));
            sLog("Aggrigate Vibe: " + aggVibe);
            vibeColor(aggVibe);
            response = response.substring(i2 + 1);
        }

        if (response.indexOf(DATA_VER_WEB) > -1)
        {
            int i1 = response.indexOf(DATA_VER_WEB);
            int i2 = response.indexOf(DATA_VER_DEL);
            int i3 = response.indexOf(DATA_DELIM);
            String author = String(response.substring(i1 + 1, i2));
            String rev = String(response.substring(i2 + 1, i3));
            playTone(notesSvn, 4);
            scrollString(author + " WWW." + rev);
            delay(POLL_INTERVAL);
            displayString(users);
            response = response.substring(i3 + 1);
        }

        if (response.indexOf(DATA_VER_API) > -1)
        {
            int i1 = response.indexOf(DATA_VER_API);
            int i2 = response.indexOf(DATA_VER_DEL);
            int i3 = response.indexOf(DATA_DELIM);
            String author = String(response.substring(i1 + 1, i2));
            String rev = String(response.substring(i2 + 1, i3));
            playTone(notesSvn, 4);
            scrollString(author + " API." + rev);
            delay(POLL_INTERVAL);
            displayString(users);
            response = response.substring(i3 + 1);
        }

        while (response.indexOf(DATA_USERS) > -1)
        {
            int i1 = response.indexOf(DATA_USERS);
            int i2 = response.indexOf(DATA_DELIM);
            String totalUsers = String(response.substring(i1 + 1, i2));
            sLog("Total Users: " + totalUsers);
            if (totalUsers != users)
            {
                playTone(notesNew, 3);
                displayString(totalUsers);
                displayFlash();
            }
            users = totalUsers;
            response = response.substring(i2 + 1);
        }

        //reset response data
        response = String();
        
        //set a time reference or async-operation
        lastPollMillis = millis();
    }

    //swap between clock and users
//    if (lastPollMillis > 0 && (millis() - lastDisplayMillis) >= (isClock ? CLOCK_INTERVAL : USERS_INTERVAL))
//    {
//        switch (isClock)
//        {
//            case true:
//                isClock = false;
//                displayString(users);
//                break;
//            case false:
//                isClock = true;
//                displayString(getClock());
//                break;
//        }
//        lastDisplayMillis = millis();
//    }
    
    //handle millis() overflow
    if (millis() < lastPollMillis)
        lastPollMillis = 0;
    //handle overflow for clock flip too
//    if (millis() < lastDisplayMillis)
//        lastDisplayMillis = 0;
}

void displayString(String s)
{
    myDisplay.off();
    myDisplay.clear();
    myDisplay.scroll(stringToChars(s), 0);
    myDisplay.on();
}

void scrollString(String s)
{
    myDisplay.off();
    myDisplay.clear();
    myDisplay.on();
    myDisplay.scroll(stringToChars(s), SCROLL_INTERVAL);
}

void displayFlash()
{
    for (int i = 0; i < FLASH_COUNT; i++)
    {
        myDisplay.off();
        delay(FLASH_INTERVAL);
        myDisplay.on();
        delay(FLASH_INTERVAL);
    }
}

void playTone(int *notes, int n)
{
    for (int i = 0; i < n; i++)
    {
        tone(PIN_BUZZ, notes[i], noteLen);
        int breath = noteLen * 1.30;
        delay(breath);
        noTone(PIN_BUZZ);
    }
}

String getClock()
{
    time_t t = now() + GMT_OFFSET;
    String c = String();
    int tmp = hourFormat12(t);
//    if (tmp < 10)
//        c += "0";
    c += String(tmp) + ".";
    tmp = minute(t);
    if (tmp < 10)
        c += "0";
    c += String(tmp) + " ";
    if (isAM(t))
        c += "A";
    else
        c += "P";
    return c;
}

time_t stringToTime(String s)
{
    sLog(s);
    char arr[12];
        time_t t = 0;
    s.toCharArray(arr, sizeof(arr));
    for (int i = 0; i < 10; i++)
    {
        if (arr[i] >= '0' && arr[i] <= '9')
            t = (10 * t) + (arr[i] - '0'); // convert digits to a number    
    }
//    sLog(t);
    return t;
}

long stringToLong(String s)
{
    char arr[12];
    s.toCharArray(arr, sizeof(arr));
    return atol(arr);
}

char * stringToChars(String s)
{
    char arr[256];
    s.toCharArray(arr, sizeof(arr));
    return arr;
}

int gammaCorrect(int channel)
{
    return channel == 0 ? 0 : pow(255, ((channel + 1) / 4) / 64.0) + 0.5;
}

void allOff()
{
    analogWrite(PIN_R, 0);
    analogWrite(PIN_G, 0);
    analogWrite(PIN_B, 0);
}

void vibeColor(long vibeCatId)
{
    int r, g, b;
    switch (vibeCatId)
    {
        case 1:
            //purple
//          sLog("purple");
            r = colorPurple[0];
            g = colorPurple[1];
            b = colorPurple[2];
            break;
        case 2:
            //blue
//          sLog("blue");
            r = colorBlue[0];
            g = colorBlue[1];
            b = colorBlue[2];
            break;
        case 3:
            //green
//          sLog("green");
            r = colorGreen[0];
            g = colorGreen[1];
            b = colorGreen[2];
            break;
        case 4:
            //yellow
//          sLog("yellow");
            r = colorYellow[0];
            g = colorYellow[1];
            b = colorYellow[2];
            break;
        case 5:
            //orange
//          sLog("orange");
            r = colorOrange[0];
            g = colorOrange[1];
            b = colorOrange[2];
            break;
        case 6:
            //red
//          sLog("red");
            r = colorRed[0];
            g = colorRed[1];
            b = colorRed[2];
            break;
        case 7:
            //brown
//          sLog("brown");
            r = colorBrown[0];
            g = colorBrown[1];
            b = colorBrown[2];
            break;
        case 8:
            //gray
//          sLog("gray0");
            r = colorGray0[0];
            g = colorGray0[1];
            b = colorGray0[2];
            break;
        case 9:
            //gray1
//          sLog("gray1");
            r = colorGray1[0];
            g = colorGray1[1];
            b = colorGray1[2];
            break;
        case 10:
            //gray2
//          sLog("gray2");
            r = colorGray2[0];
            g = colorGray2[1];
            b = colorGray2[2];
            break;
        case 11:
            //pink
//          sLog("pink");
            r = colorPink[0];
            g = colorPink[1];
            b = colorPink[2];
            break;
        case 12:
            //light blue
//          sLog("light blue");
            r = colorLightBlue[0];
            g = colorLightBlue[1];
            b = colorLightBlue[2];
            break;
    }
    analogWrite(PIN_R, gammaCorrect(r));
    analogWrite(PIN_G, gammaCorrect(g));
    analogWrite(PIN_B, gammaCorrect(b));
}

void sLog(String s)
{
    if (DEBUG)
        Serial.println(s);
}
