#include <Wire.h>

int count = 0;

void setup() {
  Wire.begin(8);
  Wire.onReceive(receiveEvent);
  Serial.begin(9600);
}

void loop() {
  long startTime = millis();
  count = 0;
  while(millis() < startTime + 5000);
  count = count / 5;
  Serial.println(count);
}

void receiveEvent() {
  Wire.read();
  count++;
}
