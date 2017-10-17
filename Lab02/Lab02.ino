const int greenLedPin = 2;
const int redLedPin = 3;
const int yellowLedPin = 4;

const int tempSensor = A0;
const int lightSensor = A1;
const int angleSensor = A3;

int tempRead = 0;
int lightRead = 0;
int angleRead = 0;
int lastAngleRead = 0;

int lightReadMinValue = 1023; //Start on max
int lightReadMaxValue = 0; //Start on min

float temperature = 0;
int light = 0;

int angle = 0;
boolean isAngleOn = false;
int intervalTime = 0;
unsigned long previousTime = 0, currentTime = 0;

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
  
  // End Calibration
  digitalWrite(greenLedPin, LOW);
  digitalWrite(redLedPin, LOW);
  digitalWrite(yellowLedPin, LOW);

  Serial.begin(9600);
}

void loop() {  
  // Reaction Temperature

  tempRead = analogRead(tempSensor); // Read Temperature
  temperature =  (((tempRead / 1024.0) * 5.0 ) - 0.5) * 100;
  
  if(temperature > 30) {
    digitalWrite(greenLedPin, HIGH);
  } else {
    digitalWrite(greenLedPin, LOW);
  }

  // Reaction Light
  lightRead = analogRead(lightSensor); // Read Light 
  light = map(lightRead, lightReadMinValue, lightReadMaxValue, 255, 0); // Convert Analog Value
  light = constrain(light, 0, 255);
  
  Serial.println(light);
  
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
  
  if (currentTime - previousTime >= intervalTime) {
    angleRead = analogRead(angleSensor); // Read Angle
    if (lastAngleRead != angleRead) {
      angle = map(angleRead, 0, 1023, 0, 180); 
      intervalTime = map(angle, 0, 180, 200, 2000);
      lastAngleRead = angleRead;
    }
    isAngleOn = !isAngleOn;
    if(isAngleOn) {
      digitalWrite(yellowLedPin, HIGH);
    } else {
      digitalWrite(yellowLedPin, LOW);
    }
    previousTime = currentTime;
  }
}
