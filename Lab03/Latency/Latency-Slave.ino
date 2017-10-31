#include <Wire.h>

const int sendPin = 8;

void setup() {
  pinMode(sendPin, OUTPUT);
  Wire.begin(8);
  Wire.onReceive(receiveEvent);

  digitalWrite(sendPin, LOW);
}

void loop() {
  digitalWrite(sendPin, LOW);
  delay(2000);
}

void receiveEvent() {
  Wire.read();
  digitalWrite(sendPin, HIGH);
}
