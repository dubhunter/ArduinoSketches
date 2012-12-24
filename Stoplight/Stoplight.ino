 
 int redPin = 6;
 int ylwPin = 5;
 int grnPin = 3;

void setup() {
  pinMode(redPin, OUTPUT);
  pinMode(ylwPin, OUTPUT);
  pinMode(grnPin, OUTPUT);
}

void loop() {
  digitalWrite(redPin, HIGH);
  delay(2000);
  digitalWrite(grnPin, HIGH);
  delay(1000);
  digitalWrite(redPin, LOW);
//  digitalWrite(grnPin, HIGH);
  delay(2000);
  digitalWrite(ylwPin, HIGH);
  delay(1000);
  digitalWrite(grnPin, LOW);
//  digitalWrite(ylwPin, HIGH);
  delay(2000);
  digitalWrite(ylwPin, LOW);
}
