/*
  Analog input, analog output, serial output
 
 Reads an analog input pin, maps the result to a range from 0 to 255
 and uses the result to set the pulsewidth modulation (PWM) of an output pin.
 Also prints the results to the serial monitor.
 
 The circuit:
 * potentiometer connected to analog pin 0.
   Center pin of the potentiometer goes to the analog pin.
   side pins of the potentiometer go to +5V and ground
 * LED connected from digital pin 9 to ground
 
 created 29 Dec. 2008
 Modified 4 Sep 2010
 by Tom Igoe
 
 This example code is in the public domain.
 
 */

// These constants won't change.  They're used to give names
// to the pins used:
const int analogInPin = A1;  // Analog input pin that the potentiometer is attached to
const int analogOutPin = 9; // Analog output pin that the LED is attached to

int val1 = 0;        // value read from the pot
int val2 = 0;        // value read from the pot
int tmp = 0;        // value output to the PWM (analog out)
int tmp2 = 0;        // value output to the PWM (analog out)
int converge = 10;
float peak = 1.1;
int avg = 0;
int diff = 0;
int vMin = 1023;
int vMax = 0;

void setup() {
  // initialize serial communications at 9600 bps:
  Serial.begin(9600);
  pinMode(A0, INPUT);
  pinMode(A1, INPUT);
  pinMode(3, OUTPUT);
  pinMode(5, OUTPUT);
  pinMode(6, OUTPUT);
  pinMode(9, OUTPUT);
}

void loop() {
  // read the analog in value:
  val1 = analogRead(A0);
  tmp = analogRead(A1);
//  if (val < vMin) vMin = val;
//  if (val > vMax) vMax = val;
//  tmp = map(val, vMin, vMax, 0, 255);
  
//  diff = val2 - avg;
////  avg += diff / map(val1, 0, 1023, 1, 10);
//  avg += diff / converge;
//  if (val2 > avg * peak)
//      tmp = (100 * val2) / avg;
//  else
//      tmp = 0;
  if (tmp < vMin) vMin = tmp;
  if (tmp > vMax) vMax = tmp;
  tmp = map(tmp, vMin, vMax, 0, 1023);
  Serial.println(tmp);
  if (tmp < 256){
      tmp2 = tmp;
      tmp = 0;
  }else{
      tmp2 = 255;
      tmp -= tmp2;
  }
  analogWrite(3, tmp2);
  if (tmp < 256){
      tmp2 = tmp;
      tmp = 0;
  }else{
      tmp2 = 255;
      tmp -= tmp2;
  }
  analogWrite(5, tmp2);
  if (tmp < 256){
      tmp2 = tmp;
      tmp = 0;
  }else{
      tmp2 = 255;
      tmp -= tmp2;
  }
  analogWrite(6, tmp2);
  if (tmp < 256){
      tmp2 = tmp;
      tmp = 0;
  }else{
      tmp2 = 255;
      tmp -= tmp2;
  }
  analogWrite(9, tmp2);

  // wait 10 milliseconds before the next loop
  // for the analog-to-digital converter to settle
  // after the last reading:
//  delay(10);                     
}
