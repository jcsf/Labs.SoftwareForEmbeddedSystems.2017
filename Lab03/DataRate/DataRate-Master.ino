#include <Wire.h>

void setup() {
  // Start Wire Connection
  Wire.begin();
}

void loop() {
  Wire.beginTransmission(8);
  Wire.write("A");
  Wire.endTransmission();
}
