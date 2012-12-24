/*
  Melody
 
 Plays a melody
 
 circuit:
 * 8-ohm speaker on digital pin 8
 
 created 21 Jan 2010
 modified 14 Oct 2010
 by Tom Igoe

This example code is in the public domain.
 
 http://arduino.cc/en/Tutorial/Tone
 
 */
 #include "pitches.h"
const int PIN_BUZZ = 9;

//tones
int notesNew[] = { NOTE_G6, NOTE_G7, NOTE_D8 };
int notesSvn[] = { NOTE_A6, NOTE_A7, NOTE_A6, NOTE_A7 };
int noteLen = 1000 / 16;

void setup() {
    Serial.begin(9600);
    pinMode(PIN_BUZZ, OUTPUT);
}

void loop() {
    Serial.println("New User:");
    playTone(notesNew, 3);
    Serial.println("New Code:");
    playTone(notesSvn, 4);
}

void playTone(int *notes, int n)
{
    Serial.println(n);
    for (int i = 0; i < n; i++)
    {
        Serial.println(notes[i]);
        tone(PIN_BUZZ, notes[i], noteLen);
        int breath = noteLen * 1.30;
        delay(breath);
        noTone(PIN_BUZZ);
    }
    delay(1000);
}
