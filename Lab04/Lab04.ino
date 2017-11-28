#include <Wire.h>

// MESSAGES
#define RED 0
#define GREEN 1
#define OFF 2
#define PING 3
#define ACK 4
#define STATUS 5

// SEMAPHORES
#define YELLOW_TRANSITION_TIME 500
#define YELLOW_BLINKING_PERIOD 1000
#define WATCHDOG_TIMER 300

// CONTROLLER
#define WIRE_MODE false
#define MAX_ENTRY 4
#define PING_PERIOD 100
#define ERRORS_PERIOD 200


// SEMAPHORE A
const int A_redLedPin = 11;
const int A_yellowLedPin = 10;
const int A_greenLedPin = 9;
const int A_redStatusPin = A1;
const int A_pedestriansLedPin = 8;
const int A_pedestriansButtonPin = 7;

boolean A_redLight = false;
boolean A_yellowLight = false;
boolean A_greenLight = false;
boolean A_pedestriansLight = false;

boolean A_pedestriansButtonPressed = false;

int A_status = 0; //0 - OFF | 1 - OPEN | 2 - CLOSE
boolean A_error = false;

long A_lastTime = 0;
long A_watchdogTimer = 0;
boolean A_watchdog = true;
// -------------------------------

// SEMAPHORE B
const int B_redLedPin = 6;
const int B_yellowLedPin = 5;
const int B_greenLedPin = 4;
const int B_redStatusPin = A2;

boolean B_redLight = false;
boolean B_yellowLight = false;
boolean B_greenLight = false;

int B_status = 0; //0 - OFF | 1 - OPEN | 2 - CLOSE
boolean B_error = false;

long B_lastTime = 0;
long B_watchdogTimer = 0;
boolean B_watchdog = true;
// -------------------------------

// ENTRY CONTROLLER
const int entryLSPin = 2;
const int entryHSPin = 3;

int entryNumber;
bool isController = false;

uint8_t messageReceive[4] = {0, 0, 0, 0};
uint8_t messageSent[5] = {0, 0, 0, 0, 0};
int mSLength = 0;
// -------------------------------

// CONTROLLER
const int potentiometerPin = A0;
const int controllerButtonPin = 13;
const int controllerLedPin = 12;

int entryOpen = 1;

bool controllerOn = false;
bool sentOff = true;
bool reduceTime = false;

uint8_t messageController[5] = {0, 0, 0, 0, 0}; 

int persistentErrors = 0;
bool entryErrors[5] = {false, false, false, false, false};

long controller_period = 2000;
long controller_lastTimePeriod = 0;
long controller_lastTimePing = 0;
long controller_lastTimeBlink = 0;
long controller_lastTimeErrors = 0;
// -------------------------------

void setup() {
  Serial.begin(9600);
  
  entryNumber = getEntryNumber();

  Serial.println("ENTRY:");
  Serial.println(entryNumber);
  Serial.println("------");
  
  if(entryNumber == 1) {
    isController = true;
  }
  
  pinMode(A_redLedPin, OUTPUT);
  pinMode(A_yellowLedPin, OUTPUT);
  pinMode(A_greenLedPin, OUTPUT);
  pinMode(A_redStatusPin, INPUT);
  
  pinMode(A_pedestriansLedPin, OUTPUT);
  pinMode(A_pedestriansButtonPin, INPUT);
  
  pinMode(B_redLedPin, OUTPUT);
  pinMode(B_yellowLedPin, OUTPUT);
  pinMode(B_greenLedPin, OUTPUT);
  pinMode(B_redStatusPin, INPUT);

  pinMode(controllerButtonPin, INPUT);
  pinMode(controllerLedPin, OUTPUT);

  pinMode(entryLSPin, INPUT);
  pinMode(entryHSPin, INPUT);
  
  Wire.begin(entryNumber);
  Wire.onReceive(receiveEvent);
  Wire.onRequest(requestEvent);
}

void loop() {
  if(isController) {
    controller();
  }

  semaphoreA();

  semaphoreB();
}

//--------------------------------------| SEMAPHORE A |--------------------------------------------------------

void semaphoreA() {
  long currentTime = millis();

  if(A_status == 0 || A_error) {
    if(currentTime - A_lastTime >= YELLOW_BLINKING_PERIOD) {
      A_yellowLight = !A_yellowLight;
      A_lastTime = currentTime;
    }
    A_redLight = false;
    A_greenLight = false;
    A_pedestriansLight = false;
  } else { 
    if(currentTime - A_lastTime < YELLOW_TRANSITION_TIME) {
      A_redLight = false;
      A_yellowLight = true;
      A_greenLight = false; 
      A_pedestriansLight = false;
    } else {
      if(A_status == 1) {
        A_redLight = false;
        A_yellowLight = false;
        A_greenLight = true;
        A_pedestriansLight = false;
        if(digitalRead(A_pedestriansButtonPin) == HIGH) {
          A_pedestriansButtonPressed = true;
        }
      } else if (A_status == 2) {
        A_redLight = true;
        A_yellowLight = false;
        A_greenLight = false;
        A_pedestriansLight = true;
      }
    }
  }

  if(A_redLight) {
    digitalWrite(A_redLedPin, HIGH);
  } else {
    digitalWrite(A_redLedPin, LOW);
  }
  if(A_yellowLight) {
    digitalWrite(A_yellowLedPin, HIGH);
  } else {
    digitalWrite(A_yellowLedPin, LOW);
  }
  if(A_greenLight) {
    digitalWrite(A_greenLedPin, HIGH);
  } else {
    digitalWrite(A_greenLedPin, LOW);
  }
  if(A_pedestriansLight) {
    digitalWrite(A_pedestriansLedPin, HIGH);
  } else {
    digitalWrite(A_pedestriansLedPin, LOW);
  }

  if(A_status == 2 && (A_redLight || A_error)) {
      digitalWrite(A_redLedPin, HIGH);
      if(analogRead(A_redStatusPin) > 450) {
        A_error = false;
      } else {
        digitalWrite(A_redLedPin, LOW);
        A_error = true;
      }
  }

  if(A_status > 0 && currentTime >= A_watchdogTimer + WATCHDOG_TIMER) {
    if(!A_watchdog) {
      Serial.println("IN WATCHDOG A");
      setSemaphoreAOff();
    } else {
      A_watchdog = false;
    }
    A_watchdogTimer = currentTime;
  }
}

void setSemaphoreAOff() {
  if(A_status != 0) {
    A_status = 0;
    A_lastTime = millis();
    A_yellowLight = true;
    A_error = false;
    A_watchdogTimer = millis();
    A_watchdog = true;
  }
}

void setSemaphoreAOpen() {
  if(A_status != 1) {
    A_status = 1;
    A_lastTime = millis();
  }
}

void setSemaphoreAClose() {
  if(A_status != 2) {
    A_status = 2;
    A_lastTime = millis();
  }
}

boolean isSemaphoreAError() {
  return A_error;
}

//--------------------------------------| SEMAPHORE B |--------------------------------------------------------

void semaphoreB() {
 long currentTime = millis();
  if(B_status == 0 || B_error) {
    if(currentTime - B_lastTime >= YELLOW_BLINKING_PERIOD) {
      B_yellowLight = !B_yellowLight;
      B_lastTime = currentTime;
    }
    B_redLight = false;
    B_greenLight = false;
  } else {
    if(currentTime - B_lastTime < YELLOW_TRANSITION_TIME) {
      B_redLight = false;
      B_yellowLight = true;
      B_greenLight = false;
    } else {
      if(B_status == 1) {
        B_redLight = false;
        B_yellowLight = false;
        B_greenLight = true;
      } else if(B_status == 2) {
        B_redLight = true;
        B_yellowLight = false;
        B_greenLight = false;
      }
    }
  }

  if(B_redLight) {
    digitalWrite(B_redLedPin, HIGH);
  } else {
    digitalWrite(B_redLedPin, LOW);
  }
  if(B_yellowLight) {
    digitalWrite(B_yellowLedPin, HIGH);
  } else {
    digitalWrite(B_yellowLedPin, LOW);
  }
  if(B_greenLight) {
    digitalWrite(B_greenLedPin, HIGH);
  } else {
    digitalWrite(B_greenLedPin, LOW);
  }

  if(B_status == 2 && (B_redLight || B_error)) {
      digitalWrite(B_redLedPin, HIGH);
      if(analogRead(B_redStatusPin) > 450) {
        B_error = false;
      } else {
        digitalWrite(B_redLedPin, LOW);
        B_error = true;
      }
  }

  if(B_status > 0 && currentTime >= B_watchdogTimer + WATCHDOG_TIMER) {
    if(!B_watchdog) {
      Serial.println("IN WATCHDOG B");
      setSemaphoreBOff();
    } else {
      B_watchdog = false;
    }
    B_watchdogTimer = currentTime;
  }
}

void setSemaphoreBOff() {
  if(B_status != 0) {
    B_status = 0;
    B_lastTime = millis();
    B_yellowLight = true;
    B_error = false;
    B_watchdog = true;
    B_watchdogTimer = millis();
  }
}

void setSemaphoreBOpen() {
  if(B_status != 1) {
    B_status = 1;
    B_lastTime = millis();
  }
}

void setSemaphoreBClose() {
  if(B_status != 2) {
    B_status = 2;
    B_lastTime = millis();
  }
}

boolean isSemaphoreBError() {
  return B_error;
}

//--------------------------------------| ENTRY CONTROLLER |--------------------------------------------------------

int getEntryNumber() {
  int entry = 0;
  if(digitalRead(entryHSPin) == HIGH) {
    entry = 1;
  }
  entry = entry << 1;
  if(digitalRead(entryLSPin) == HIGH) {
    entry = entry + 1;
  }

  return entry + 1;
}

void receiveMessage(uint8_t* message) {
  uint8_t testIntegrity = 0;
  
  for(int i = 0; i < 3; i++) {
    testIntegrity = testIntegrity + message[i];
  }

  A_watchdog = true;
  B_watchdog = true;
  
  if(testIntegrity == message[3]) {
    if(message[1] == RED) {
      Serial.println("RED MESSAGE RECEIVED");
      setSemaphoreAClose();
      setSemaphoreBOpen();
      generateMessage(entryNumber, ACK, 0);
    } else if (message[1] == GREEN) {
      Serial.println("GREEN MESSAGE RECEIVED");
      setSemaphoreBClose();
      setSemaphoreAOpen();
      generateMessage(entryNumber, ACK, 0);
    } else if (message[1] == OFF) {
      Serial.println("OFF MESSAGE RECEIVED");
      setSemaphoreAOff();
      setSemaphoreBOff();
      generateMessage(entryNumber, ACK, 0);
    } else if (message[1] == PING) {
      Serial.println("PING MESSAGE RECEIVED");
      generateStatusMessage(entryNumber, STATUS, 0, isSemaphoreAError(), isSemaphoreBError(), A_pedestriansButtonPressed);
      A_pedestriansButtonPressed = false;
    }
  }

  if(entryNumber == 1) {
    receiveMessageController(messageSent, mSLength);
  }

  A_watchdog = true;
  B_watchdog = true;
}

void generateMessage(uint8_t sender, uint8_t messageType, uint8_t destination) {
  messageSent[0] = sender;
  messageSent[1] = messageType;
  messageSent[2] = destination;
  messageSent[3] = sender + messageType + destination;

  mSLength = 4;
}

void generateStatusMessage(int sender, char messageType, int destination, boolean errorSemaphoreA, boolean errorSemaphoreB, boolean buttonPressed) {
  uint8_t statusMessage = 0x00;
  
  if(errorSemaphoreA) {
    statusMessage = statusMessage | 0xE0;
  }

  if(errorSemaphoreB) {
    statusMessage = statusMessage | 0x1C;
  }

  if(buttonPressed) {
    statusMessage = statusMessage | 0x02;
  }

  messageSent[0] = sender;
  messageSent[1] = messageType;
  messageSent[2] = destination;
  messageSent[3] = statusMessage;
  messageSent[4] = sender + messageType + destination + statusMessage;

  mSLength = 5;
}

void requestEvent(){
  Wire.write(messageSent, mSLength);
}

void receiveEvent() {
  int i = 0;
  while (0 < Wire.available()) {
    messageReceive[i] = Wire.read();
    i++;
  }
  
  receiveMessage(messageReceive);
}

//--------------------------------------| CONTROLLER |--------------------------------------------------------
void controller() { 
  int changeController = false;
  while(digitalRead(controllerButtonPin) == HIGH) {
    if(!changeController) {
      controllerOn = !controllerOn;
    }
    changeController = true;
    sentOff = false;
    entryOpen = 1;
  }

  // Go Off if Errors Persist
  if (persistentErrors > 2) {
    controllerOn = false;
    sentOff = false;
    persistentErrors = 0;
  }
  
  if(controllerOn) { 
    // -----------| READ PERIOD |------------
    int angleRead = analogRead(potentiometerPin);
    controller_period = map(angleRead, 0, 1023, 2000, 15000);

    long currentTime = millis();

    // --------------| PERIOD |--------------
    if(currentTime >= controller_lastTimePeriod + controller_period || changeController) {
      //See if Button Has Been Pressed
      if(reduceTime) {
        long remainingTime = (controller_lastTimePeriod + controller_period) - currentTime;
        controller_lastTimePeriod -= remainingTime / 2;
        reduceTime = false;
      }

      //Close Entrys
      for(int i = 0; i < MAX_ENTRY; i++) {
        if(i != entryOpen - 1) {
          sendMessage(generateMessageController(0, RED, i+1));
        }
      }
    
      //Open Entry
      sendMessage(generateMessageController(0, GREEN, entryOpen));
    
      if(entryOpen == MAX_ENTRY) {
        entryOpen = 1;
      } else {
        entryOpen++;
      }

      controller_lastTimePeriod = millis();
    }

    // --------------| PINGS |--------------
    if(currentTime >= controller_lastTimePing + PING_PERIOD || changeController) {
     
      for(int i = 0; i < MAX_ENTRY; i++) {
          sendMessage(generateMessageController(0, PING, i+1));
      }

      controller_lastTimePing = millis();
    }

    // --------------| ERRORS |--------------
    if(currentTime >= controller_lastTimeErrors + ERRORS_PERIOD || changeController) {
     
      //See if Errors Exists
      for(int i = 1; i <= MAX_ENTRY; i++) {
        if(entryErrors[i]) {
          persistentErrors++;
          break;
        }
        if(i == MAX_ENTRY) {
          persistentErrors = 0;
        }
      }

      controller_lastTimeErrors = millis();
    }
        
  } else { // --------------| OFF |--------------
    digitalWrite(controllerLedPin, LOW);
    if(!sentOff) {
      for(int i = 0; i < MAX_ENTRY; i++) {
        sendMessage(generateMessageController(0, OFF, i+1));
      }
      sentOff = true;
    }
  }
}

uint8_t* generateMessageController(uint8_t sender, uint8_t messageType, uint8_t destination) {
  messageController[0] = sender;
  messageController[1] = messageType;
  messageController[2] = destination;
  messageController[3] = sender + messageType + destination;

  return  messageController;
}

void sendMessage(uint8_t* message) {
  blinkLedCommunication();
  if(message[2] == 1) {
    receiveMessage(message);
  } else {
    if(WIRE_MODE) {
      sendWireMessage(message);
    }
  }
  blinkLedCommunication();
}

void sendWireMessage(uint8_t* message){
  int entry = message[2];
  int mLength = 0;
  if(message[1] == PING) {
    mLength = 5;
  } else {
    mLength = 4; 
  }
  Wire.beginTransmission(entry);
  Wire.write(message, 4);
  Wire.endTransmission();
  
  Wire.requestFrom(entry, mLength);
  int i = 0;
  
  long startTime = millis();
  long timeout = startTime + 200;
  
  while (0 < Wire.available() && millis() < timeout) {
    messageController[i] = Wire.read();
    i++;
  }
  
  if(i < mLength) {
    entryErrors[entry] = true;
  }
  
  receiveMessageController(messageController, mLength);
}

void receiveMessageController(uint8_t* message, int mLength) {
  uint8_t testIntegrity = 0;

  blinkLedCommunication();
  
  for(int i = 0; i < mLength-1; i++) {
    testIntegrity = testIntegrity + message[i];
  }

  int entry = message[0];
     
  if(testIntegrity == message[mLength-1]) {
    if(message[1] == ACK) {
      entryErrors[entry] = false;
      return;
    } else if (message[1] == STATUS) {
      uint8_t statusM = message[3];
      uint8_t statusLeds = statusM & 0xFC;
      uint8_t statusButton = statusM & 0x02;
      
      if(statusLeds != 0) {
        entryErrors[entry] = true;
      } else {
        entryErrors[entry] = false;
      }

      if(statusButton == 0x02) {
        reduceTime = true;
      }
    }
  } else {
     entryErrors[entry] = true;
  }

  blinkLedCommunication();
}

void blinkLedCommunication() {
  long currentTime = millis();
  
  // --------------| BLINKING |-----------
  if(currentTime >= controller_lastTimeBlink + 100) {
    digitalWrite(controllerLedPin, !digitalRead(controllerLedPin));
    controller_lastTimeBlink = currentTime;
  }
}

