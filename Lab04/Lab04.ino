#include <Wire.h>

// MESSAGES
#define RED 0
#define GREEN 1
#define OFF 2
#define PING 3
#define ACK 4
#define STATUS 5

// SEMAPHORE A
const int A_redLedPin = 11;
const int A_yellowLedPin = 12;
const int A_greenLedPin = 13;
const int A_redStatusPin = A1;
const int A_pedestriansLedPin = 9;
const int A_pedestriansButtonPin = 8;

boolean A_redLight = false;
boolean A_yellowLight = false;
boolean A_greenLight = false;
boolean A_pedestriansLight = false;

boolean A_pedestriansButtonPressed = false;

int A_status = 0; //0 - OFF | 1 - OPEN | 2 - CLOSE
const long A_blinkingPeriod = 1000;
const long A_transitionTime = 500;
boolean A_error = false;

long A_lastTime = 0;
// -------------------------------

// SEMAPHORE B
const int B_redLedPin = 5;
const int B_yellowLedPin = 6;
const int B_greenLedPin = 7;
const int B_redStatusPin = A2;

boolean B_redLight = false;
boolean B_yellowLight = false;
boolean B_greenLight = false;

int B_status = 0; //0 - OFF | 1 - OPEN | 2 - CLOSE
const long B_blinkingPeriod = 1000;
const long B_transitionTime = 500;
boolean B_error = false;

long B_lastTime = 0;
// -------------------------------

// CONTROLLER
const int potentiometerPin = A0;
const int controllerButtonPin = 2;
const int controllerLedPin = 3;

int maxEntry = 4;
char messageController[6] = "00000"; 

bool controllerOn = false;
bool sentOff = true;
bool reduceTime = false;

int persistentErrors = 0;
bool entryErrors[4] = {false, false, false, false};

bool aloneMode = true;

long controller_lastTimePeriod = 0;
long controller_lastTimePing = 0;
long controller_lastTimeBlink = 0;
long controller_period = 2000;
long controller_periodPing = 1000;

// -------------------------------

const int entryLSPin = 4;
const int entryHSPin = 10;

int entryNumber;
int entryOpen = 1;
bool isController = false;

char messageSent[6] = "00000";
char messageReceive[5] = "0000"; 

void setup() {
  Serial.begin(9600);
  
  entryNumber = getEntryNumber();
  
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
  
  /*Wire.begin(entryNumber);
  Wire.onReceive(receiveEvent);
  Wire.onRequest(requestEvent);*/
}

void loop() {
  if(isController || aloneMode) {
    controller();
  }

  semaphoreA();

  semaphoreB();
}

//--------------------------------------| SEMAPHORE A |--------------------------------------------------------

void semaphoreA() {
  long currentTime = millis();
  if(A_status == 0 || A_error) {
    if(currentTime - A_lastTime >= A_blinkingPeriod) {
      A_yellowLight = !A_yellowLight;
      A_lastTime = currentTime;
    }
    A_redLight = false;
    A_greenLight = false;
    A_pedestriansLight = false;
  } else {
    if(currentTime - A_lastTime < A_transitionTime) {
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
          delay(500);
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
        A_error = true;
      }
  }
}

void setSemaphoreAOff() {
  if(A_status != 0) {
    A_status = 0;
    A_lastTime = millis();
    A_yellowLight = true;
    A_error = false;
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
    if(currentTime - B_lastTime >= B_blinkingPeriod) {
      B_yellowLight = !B_yellowLight;
      B_lastTime = currentTime;
    }
    B_redLight = false;
    B_greenLight = false;
  } else {
    if(currentTime - B_lastTime < B_transitionTime) {
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
        B_error = true;
      }
  }
}

void setSemaphoreBOff() {
  if(B_status != 0) {
    B_status = 0;
    B_lastTime = millis();
    B_yellowLight = true;
    B_error = false;
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

void receiveMessage(char* message) {
  char testIntegrity = 0;

  for(int i = 0; i < 3; i++) {
    testIntegrity = testIntegrity + message[i];
  }
    
  if(testIntegrity == message[3]) {
    if(message[1] - '0' == RED) {
      setSemaphoreAClose();
      setSemaphoreBOpen();
      requestEvent(generateMessage(entryNumber, ACK, 0));
    } else if (message[1] - '0' == GREEN) {
      setSemaphoreBClose();
      setSemaphoreAOpen();
      requestEvent(generateMessage(entryNumber, ACK, 0));
    } else if (message[1] - '0' == OFF) {
      setSemaphoreAOff();
      setSemaphoreBOff();
      requestEvent(generateMessage(entryNumber, ACK, 0));
    } else if (message[1] - '0' == PING) {
      requestEvent(generateStatusMessage(entryNumber, STATUS, 0, isSemaphoreAError(), isSemaphoreBError(), A_pedestriansButtonPressed));
      A_pedestriansButtonPressed = false;
    }
  }  
}

char* generateMessage(int sender, int messageType, int destination) {
  char message[5] = "0000";
  sprintf(message, "%d%d%d", sender, messageType, destination);
  
  char integrity = message[0] + message[1] + message[2];
  
  sprintf(messageSent, "%d%d%d%c", sender, messageType, destination, integrity);
  
  return messageSent;
}

char* generateStatusMessage(int sender, char messageType, int destination, boolean errorSemaphoreA, boolean errorSemaphoreB, boolean buttonPressed) {
  char message[6] = "00000";
  char statusMessage = 0x00;
  
  if(errorSemaphoreA) {
    statusMessage = statusMessage | 0xE0;
  }

  if(errorSemaphoreB) {
    statusMessage = statusMessage | 0x1C;
  }

  if(buttonPressed) {
    statusMessage = statusMessage | 0x02;
  }

  sprintf(message, "%d%d%d%c", sender, messageType, destination, statusMessage);

  char integrity = message[0] + message[1] + message[2] + message[3];

  sprintf(messageSent, "%d%d%d%c%c", sender, messageType, destination, statusMessage, integrity);
  
  return messageSent;
}

void requestEvent(char *message){
  if(entryNumber == 1) {
    if(message[1] - '0' == ACK) {
      receiveMessageController(message, 4);
    } else {
      receiveMessageController(message, 5);
    }
  } else {
    Wire.write(message);
  }
}

void receiveEvent() {
  int i = 0;
  while (1 < Wire.available()) {
    messageReceive[i] = Wire.read();
    i++;
  }
  receiveMessage(messageReceive);
}

//--------------------------------------| CONTROLLER |--------------------------------------------------------
void controller() { 
  if(digitalRead(controllerButtonPin) == HIGH) {
    controllerOn = !controllerOn;
    sentOff = false;
    entryOpen = 1;
    delay(700);
  }

  if (persistentErrors > 2) {
    controllerOn = false;
    sentOff = false;
    persistentErrors = 0;
  }
  
  if(controllerOn) { 
    int angleRead = analogRead(potentiometerPin);
    controller_period = map(angleRead, 0, 1023, 2000, 15000);

    long currentTime = millis();

    // --------------| PERIOD |--------------
    if(currentTime >= controller_lastTimePeriod + controller_period) {
      //See if Errors Exists
      for(int i = 0; i < maxEntry; i++) {
        if(entryErrors[i]) {
          persistentErrors++;
          break;
        }
        if(i == maxEntry - 1) {
          persistentErrors = 0;
        }
      }

      //See if Button Has Been Pressed
      if(reduceTime) {
        long remainingTime = (controller_lastTimePeriod + controller_period) - currentTime;
        controller_lastTimePeriod -= remainingTime / 2;
        reduceTime = false;
      }

      //Close Entrys
      for(int i = 0; i < maxEntry; i++) {
        if(i != entryOpen - 1) {
          sendMessage(generateMessageController(0, RED, i+1));
        }
      }
    
      //Open Entry
      sendMessage(generateMessageController(0, GREEN, entryOpen));
    
      if(entryOpen == 4) {
        entryOpen = 1;
      } else {
        entryOpen++;
      }

      controller_lastTimePeriod = millis();
    }

    // --------------| PINGS |--------------
    if(currentTime >= controller_lastTimePing + controller_periodPing) {
      
      
      for(int i = 0; i < maxEntry; i++) {
          sendMessage(generateMessageController(0, PING, i+1));
      }

      controller_lastTimePing = millis();
    }
    
  } else {
    digitalWrite(controllerLedPin, LOW);
    // --------------| OFF |--------------
    if(!sentOff) {
      for(int i = 0; i < maxEntry; i++) {
        sendMessage(generateMessageController(0, OFF, i+1));
      }
      sentOff = true;
    }
  }
}

char* generateMessageController(int sender, int messageType, int destination) {
  char message[5] = "0000";
  sprintf(message, "%d%d%d", sender, messageType, destination);
  
  char integrity = message[0] + message[1] + message[2];
  
  sprintf(messageController, "%d%d%d%c", sender, messageType, destination, integrity);
  
  return messageController;
}

void sendMessage(char* message) {
  blinkLedCommunication();
  if(message[2] == '1') {
    receiveMessage(message);
  } else {
    //sendWireMessage(message);
  }
  blinkLedCommunication();
}

void sendWireMessage(char* message){
  int entry = message[2] -'0';
  int mLength = 0;
  Wire.beginTransmission(entry);
  Wire.write(message);
  Wire.endTransmission();
  if(message[1] - '0' == PING) {
    Wire.requestFrom(entry, 6);
    mLength = 5;
  } else {
    Wire.requestFrom(entry, 5);
    mLength = 4;
  }
  int i = 0;
  while (Wire.available()) {
    messageController[i] = Wire.read();
    i++;
  }
  
  receiveMessageController(messageController, mLength);
}

void receiveMessageController(char* message, int mLength) {
  char testIntegrity = 0;

  blinkLedCommunication();
  
  for(int i = 0; i < mLength-1; i++) {
    testIntegrity = testIntegrity + message[i];
  }
  
  if(testIntegrity == message[mLength-1]) {
    if(message[1] - '0' == ACK) {
      return;
    } else if (message[1] - '0' == STATUS) {
      char statusM = message[3];
      char statusLeds = statusM & 0xFC;
      char statusButton = statusM & 0x02;
      int entry = message[0] - '0';
      
      if(statusLeds != 0) {
        entryErrors[entry] = true;
      } else {
        entryErrors[entry] = false;
      }

      if(statusButton == 0x02) {
        reduceTime = true;
      }
    }
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

