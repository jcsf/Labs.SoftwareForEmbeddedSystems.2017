const int redLedPin = 4; 
const int greenLedPin = 5;
const int blueLedPin = 3;
const int yellowLedPin = 2;
const int buttonPin = 13;

int lightLed = 0;
int buttonRead = 0;

void setup() {
  pinMode(redLedPin, OUTPUT);
  pinMode(greenLedPin, OUTPUT);
  pinMode(blueLedPin, OUTPUT);
  pinMode(yellowLedPin, OUTPUT);

  pinMode(buttonPin, INPUT);
}

void loop() {
  // Read Button State
  buttonRead = digitalRead(buttonPin);

  if(buttonRead != HIGH) {
    // Increment Led On
    if(lightLed == 4) {
      lightLed = 0;
    } else {
      lightLed += 1;
    }
  }

   // Set all leds to off
   digitalWrite(redLedPin, LOW);
   digitalWrite(greenLedPin, LOW);
   digitalWrite(blueLedPin, LOW);
   digitalWrite(yellowLedPin, LOW);
  
  switch (lightLed) {
    case 0: // ALL LEDS OFF
      break;
    case 1: // RED LED ON
      digitalWrite(redLedPin, HIGH);
      break;
    case 2: // GREEN LED ON
      digitalWrite(greenLedPin, HIGH);
      break;
    case 3: // BLUE LED ON
      digitalWrite(blueLedPin, HIGH);
      break;
    case 4: // YELLOW LED ON
      digitalWrite(yellowLedPin, HIGH);
      break;
    default: // ALL LEDS ON - ERROR NOT EXPECTED BEHAVIOUR
      digitalWrite(redLedPin, HIGH);
      digitalWrite(greenLedPin, HIGH);
      digitalWrite(blueLedPin, HIGH);
      digitalWrite(yellowLedPin, HIGH);
      break;
    }
    
    delay(1000); // Wait 1 Second
}
