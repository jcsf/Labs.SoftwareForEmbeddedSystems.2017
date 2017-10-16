const int greenLedPin = 2;
const int redLedPin = 3;
const int yellowLedPin = 4;

const int tempSensor = A0;
const int lightSensor = A1;
const int angleSensor = A3;

int tempRead = 0;
int lightRead = 0;
int angleRead = 0;

int lightReadMinValue = 1023; //Start on max
int lightReadMaxValue = 0; //Start on min

float temperature = 0;
int light = 0;
int angle = 0;
int dTime = 0;
boolean isAngleOn = false;

void setup() {
  pinMode(greenLedPin, OUTPUT);
  pinMode(redLedPin, OUTPUT);
  pinMode(yellowLedPin, OUTPUT);
  
  // Calibrate Light Sensor
  
  // Start Calibration
  digitalWrite(greenLedPin, HIGH);
  digitalWrite(redLedPin, HIGH);
  digitalWrite(yellowLedPin, HIGH);
  
  // Calibration During the First Five Seconds
  while (millis() < 5000) {
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
  
  // End Calibration
  digitalWrite(greenLedPin, LOW);
  digitalWrite(redLedPin, LOW);
  digitalWrite(yellowLedPin, LOW);
}

void loop() {
  // Analog Read's
  tempRead = analogRead(tempSensor); // Read Temperature
  lightRead = analogRead(lightSensor); // Read Light
  angleRead = analogRead(angleSensor); // Read Angle

  // Convert Analog Values
  temperature =  (((tempRead / 1024.0) * 5.0 ) - 0.5) * 100;
  light = map(lightRead, lightReadMinValue, lightReadMaxValue, 255, 0);
  angle = map(angleRead, 0, 1023, 0, 180); 
  
  // Reaction Temperature
  if(temperature > 26) {
    digitalWrite(greenLedPin, HIGH);
  } else {
    digitalWrite(greenLedPin, LOW);
  }

  // Reaction Light
  light = constrain(light, 0, 255);
  digitalWrite(redLedPin, light);

  // Reaction Angle (Potentiometer)
  dTime = map(angle, 0, 180, 200, 2000);
  isAngleOn = !isAngleOn;
  
  if(isAngleOn) {
    digitalWrite(yellowLedPin, HIGH);
  } else {
    digitalWrite(yellowLedPin, LOW);
  }

  delay(dTime); // Wait 0.2 - 2 Seconds
}
