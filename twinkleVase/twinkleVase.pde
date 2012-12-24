
const int PIN_SPEED = A0;
const int PIN_SOUND = A1;
const int PIN_LED1 = 3;
const int PIN_LED2 = 5;
const int PIN_LED3 = 6;
const int PIN_LED4 = 9;

int fadeMin = 127;
//int fadeMin = 191;
int fadeMax = 255;
int fadeStep = 8;
//int fadeDelay = 4;
int delayFactor = 16;

void setup()
{
    Serial.begin(9600);
    pinMode(PIN_SPEED, INPUT);
    pinMode(PIN_LED1, OUTPUT);
    pinMode(PIN_LED2, OUTPUT);
    pinMode(PIN_LED3, OUTPUT);
    pinMode(PIN_LED4, OUTPUT);
}

void loop()
{
    int spd = analogRead(PIN_SPEED);
    Serial.println(spd, DEC);
//    spd = 1;
    
//    fadeUp(PIN_LED1, PIN_LED3, spd);
//    fadeDn(PIN_LED1, PIN_LED3, spd);
//    
//    fadeUp(PIN_LED2, PIN_LED4, spd);
//    fadeDn(PIN_LED2, PIN_LED4, spd);
    
    fadeUp(PIN_LED1, spd);
    fadeDn(PIN_LED1, spd);
    
    fadeUp(PIN_LED3, spd);
    fadeDn(PIN_LED3, spd);
    
    fadeUp(PIN_LED2, spd);
    fadeDn(PIN_LED2, spd);
    
    fadeUp(PIN_LED4, spd);
    fadeDn(PIN_LED4, spd);
}

void fadeUp(int pin1, int del)
{
    for (int i = fadeMin; i <= fadeMax; i += fadeStep) {
        analogWrite(pin1, i);
//        analogWrite(pin2, i);
        delay(del / delayFactor);
    }
}

void fadeDn(int pin1, int del)
{
    for (int i = fadeMax; i >= fadeMin; i -= fadeStep) {
        analogWrite(pin1, i);
//        analogWrite(pin2, i);
        delay(del / delayFactor);
    }
}
