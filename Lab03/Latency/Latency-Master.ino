#include <Wire.h>

const int receivePin = 8;

void setup() {
  pinMode(receivePin, INPUT);
  Serial.begin(9600);

  Wire.begin();
}

void loop() {
  Wire.beginTransmission(8);
  Wire.write("A");
  Wire.endTransmission();
  
  long sendTime = millis();
  
  while(digitalRead(receivePin) != HIGH);

  long latency = millis() - sendTime;

  Serial.println(latency);

  while(true);
}
