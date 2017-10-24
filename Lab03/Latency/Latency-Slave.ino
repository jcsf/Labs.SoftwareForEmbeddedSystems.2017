#include <Wire.h>

const int sendPin = 8;

void setup() {
  pinMode(sendPin, OUTPUT);
  Wire.begin(8);
  Wire.onReceive(receiveEvent);
}

void loop() {}

void receiveEvent() {
  Wire.read();
  digitalWrite(sendPin, HIGH);
}
