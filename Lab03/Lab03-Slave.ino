#include <Wire.h>

const int greenLedPin = 2;
const int redLedPin = 3;
const int yellowLedPin = 4;

int temperature = 0;
int light = 0;
int angle = 0;
boolean isAngleOn = false;
int intervalTime = 0;
unsigned long previousTime = 0, currentTime = 0;

void setup() {
  pinMode(greenLedPin, OUTPUT);
  pinMode(redLedPin, OUTPUT);
  pinMode(yellowLedPin, OUTPUT);
  
  Wire.begin(8);
  Wire.onReceive(receiveEvent);

  while(millis() < 3000);
}

void loop() {  
  
  if(temperature > 30) {
    digitalWrite(greenLedPin, HIGH);
  } else {
    digitalWrite(greenLedPin, LOW);
  }
  
  // Noise Reduction
  if(light < 20) {
    light = 0;
  } else if (light < 40) {
    light = 30;
  } else if (light < 60) {
    light = 50;
  } else if (light < 80) {
    light = 70;
  } else if (light < 100) {
    light = 90;
  } else if (light < 120) {
    light = 110;
  } else if (light < 140) {
    light = 130;
  } else if (light < 160) {
    light = 150;
  } else if (light < 180) {
    light = 170;
  } else if (light < 200) {
    light = 190;
  } else if (light < 220) {
    light = 210;
  } else if (light < 240) {
    light = 230;
  } else {
    light = 255;
  }
  analogWrite(redLedPin, light);

  // Potentiometer
  currentTime = millis();
  intervalTime = map(angle, 0, 180, 200, 2000); // Map Angle to Miliseconds
  
  if (currentTime - previousTime >= intervalTime) {
    isAngleOn = !isAngleOn;
    if(isAngleOn) {
      digitalWrite(yellowLedPin, HIGH);
    } else {
      digitalWrite(yellowLedPin, LOW);
    }
    previousTime = currentTime;
  }
}

void receiveEvent() {
  char type;
  while (1 < Wire.available()) {
    type = Wire.read(); // Read Type
  }
  if(type == 'T') {
    temperature = Wire.read();
  } else if(type == 'L') {
    light = Wire.read();
  } else if(type == 'A') {
    angle = Wire.read();
  }
}
