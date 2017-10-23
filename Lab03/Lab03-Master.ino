#include <Wire.h>

const int tempSensor = A0;
const int lightSensor = A1;
const int angleSensor = A3;

int tempRead = 0;
int lightRead = 0;
int angleRead = 0;
int lastTempRead = -1;
int lastLightRead = -1;
int lastAngleRead = -1;

int lightReadMinValue = 1023; //Start on max
int lightReadMaxValue = 0; //Start on min

float temperature = 0;
int light = 0;
int angle = 0;

void setup() {

  // Start Wire Connection
  Wire.begin();
  
  // Calibrate Light Sensor
  
  // Calibration During the First Three Seconds
  while (millis() < 3000) {
    lightRead = analogRead(lightSensor);
    
    // Record the Maximum Value Read
    if (lightRead > lightReadMaxValue) {
      lightReadMaxValue = lightRead;
    }

    // Record the Minimum Value Read
    if (lightRead < lightReadMinValue) {
      lightReadMinValue = lightRead;
    }
  }
}

void loop() {  
  
  // Temperature
  tempRead = analogRead(tempSensor); // Read Temperature
  if (lastTempRead != tempRead) {
    temperature =  (((tempRead / 1024.0) * 5.0 ) - 0.5) * 100;
    Wire.beginTransmission(8);
    Wire.write("T");
    Wire.write((int)temperature);
    Wire.endTransmission();
    lastTempRead = tempRead;
  }
  
  // Light
  lightRead = analogRead(lightSensor); // Read Light 
  if(lastLightRead != lightRead) {
    light = map(lightRead, lightReadMinValue, lightReadMaxValue, 255, 0); // Convert Analog Value
    light = constrain(light, 0, 255);
    Wire.beginTransmission(8);
    Wire.write("L");
    Wire.write(light);
    Wire.endTransmission();
    lastLightRead = lightRead;
  }
 
  // Potentiometer
  angleRead = analogRead(angleSensor); // Read Angle
  if (lastAngleRead != angleRead) {
    angle = map(angleRead, 0, 1023, 0, 180); // Map Read Value to Angle
    Wire.beginTransmission(8);
    Wire.write("A");
    Wire.write(angle);
    Wire.endTransmission();
    lastAngleRead = angleRead;
  }
}
