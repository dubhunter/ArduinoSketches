#include <Ethernet.h>
#include <ArdOSC.h>
//#define FLIPDISPLAY
#include <AlphaNumeric_Driver.h>


//local network settings
byte mac[] = { 0x90, 0xA2, 0xDB, 0x00, 0x00, 0x01 };
byte ip[] = { 192,168,0,27 };
int serverPort  = 8000;
int controllerPort  = 8009;
int color[] = { 0, 0, 0 };

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
const int FLASH_COUNT = 5;

OSCServer server;

OSCClient client;

OSCMessage *rcvMsg;

alphaNumeric myDisplay(PIN_SDI, PIN_CLK, PIN_LE, PIN_OE, NUM_DISPLAYS);

void setup()
{
  //set outputs and turn off
  pinMode(PIN_R, OUTPUT);
  pinMode(PIN_G, OUTPUT);
  pinMode(PIN_B, OUTPUT);
  
  //start the serial library
  Serial.begin(57600);
    
  //start the Ethernet connection
  Ethernet.begin(mac, ip);
  //give the Ethernet shield a second to initialize
  delay(1000);
  
  server.sockOpen(serverPort);
  myDisplay.clear();
  
  allOff();
}

void loop()
{
  if (server.available())
  {
    rcvMsg = server.getMessage();
    
    //logging code
//    byte *ip = rcvMsg->getIpAddress();
//    long int intValue;
//    float floatValue;
//    char *stringValue;
//    
//    Serial.print(ip[0],DEC);
//    Serial.print(".");
//    Serial.print(ip[1],DEC);
//    Serial.print(".");
//    Serial.print(ip[2],DEC);
//    Serial.print(".");
//    Serial.print(ip[3],DEC);
//    Serial.print(":");
//    
//    Serial.print(rcvMsg->getPortNumber());
//    Serial.print(" ");
//    Serial.print(rcvMsg->getOSCAddress());
//    Serial.print(" ");
//    Serial.print(rcvMsg->getTypeTags());
//    Serial.print("--");
//    
//    for(int i = 0; i < rcvMsg->getArgsNum(); i++)
//    {
//    
//      switch(rcvMsg->getTypeTag(i))
//      {
//      
//        case 'i':       
//          intValue = rcvMsg->getInteger32(i);
//          
//          Serial.print(intValue);
//          Serial.print(" ");
//          break; 
//         
//        case 'f':        
//          floatValue = rcvMsg->getFloat(i);
//          
//          Serial.print(floatValue);
//          Serial.print(" ");
//          break; 
//        
//        case 's':         
//          stringValue = rcvMsg->getString(i);
//           
//          Serial.print(stringValue);
//          Serial.print(" ");
//          break;
//      }
//    }
//    Serial.println("");
    //end log
    
    //create response for labels
//    OSCMessage response;
    
//    response.setAddress(ip, controllerPort);
    
    //get value
    int value = rcvMsg->getFloat(0);
    char buf[32];
    sprintf(buf, "%f", value);
     
    Serial.print(rcvMsg->getOSCAddress());
    Serial.print(": ");
    Serial.print(value);
    Serial.println("");
    
    if (String(rcvMsg->getOSCAddress()) == "/1/faderR" || String(rcvMsg->getOSCAddress()) == "/2/toggleR")
    {
//      if (String(rcvMsg->getOSCAddress()) == "/1/faderR")
//        response.setOSCMessage("/1/labelR" "f", buf);
//      analogWrite(PIN_R, value);
      color[0] = value;
      update();
    }
    
    if (String(rcvMsg->getOSCAddress()) == "/1/faderG" || String(rcvMsg->getOSCAddress()) == "/2/toggleG")
    {
//      if (String(rcvMsg->getOSCAddress()) == "/1/faderG")
//        response.setOSCMessage("/1/labelG" "f", buf);
//      analogWrite(PIN_G, int(value));
      color[1] = value;
      update();
    }
    
    if (String(rcvMsg->getOSCAddress()) == "/1/faderB" || String(rcvMsg->getOSCAddress()) == "/2/toggleB")
    {
//      if (String(rcvMsg->getOSCAddress()) == "/1/faderB")
//        response.setOSCMessage("/1/labelB" "f", buf);
//      analogWrite(PIN_B, int(value));
      color[2] = value;
      update();
    }
  
//    client.send(&response);
//    response.flush();
  }
}

void allOff()
{
  color[0] = 0;
  color[1] = 0;
  color[2] = 0;
  update();
}

void update()
{
  analogWrite(PIN_R, gammaCorrect(color[0]));
  analogWrite(PIN_G, gammaCorrect(color[1]));
  analogWrite(PIN_B, gammaCorrect(color[2]));
  updateDisplay();
}

void updateDisplay()
{
    myDisplay.off();
    myDisplay.scroll(stringToChars("#" + stringToHex(color[0]) + stringToHex(color[1]) + stringToHex(color[2])), 0);
    myDisplay.on();
}

int gammaCorrect(int channel)
{
  return channel == 0 ? 0 : pow(255, ((channel + 1) / 4) / 64.0) + 0.5; 
}

String stringToHex(int i)
{
    String s = String(i, HEX);
    return s.length() > 1 ? s : "0" + s;
}

char * stringToChars(String s)
{
	char arr[256];
	s.toCharArray(arr, sizeof(arr));
	return arr;
}
