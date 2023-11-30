#include <Wire.h>                                     

const int fTrigPin = 13 ;                                     // Redboard pin for trigger on the distance sensor       
const int fEchoPin = 12 ;                                     // Redboard pin for echo on the distance sensor

const int fRightSensor = 2 ;                                 // Redboard pin for right side reflectance sensor
const int fLeftSensor = 4 ;                                 // Same (but different pin) for left side

int fRSensorState = 0 ;                                      // Initialize right side sensor variable, 0 meaning false/high
int fLSensorState = 0 ;                                      // Same for left side

int spinDirection = 0 ;                                     // Initialize spin direction, 0 meaning spin counterclockwise. Switched to either 0 or 1 based on ring contact
float fDistance = 0 ;                                        // Initialize front sensor distance to 0 inches
int fDistanceState = 0 ;

char command;

int sensorData[4] = {0, 0, 0, '\0'}; // Array to hold sensor states and distance

// Speed control pins
const int ENA = 6;            //right side
const int ENB = 5;            //left side

// Direction control pins (LOCATIONS UNCERTAIN)
const int IN2 = 8;           //front right motor
const int IN3 = 9;            //front left motor

void setup()
{
    Serial.begin(9600) ;                                    // Set up serial connection with attached computer (pre-competition)

    pinMode(fTrigPin, OUTPUT);
    pinMode(fEchoPin, INPUT);

    pinMode(fLeftSensor, INPUT) ;                            // Declare assigned left sensor pin as input
    pinMode(fRightSensor, INPUT) ;                           // Declare assigned right sensor pin as input

    pinMode(IN2, OUTPUT);
    pinMode(IN3, OUTPUT);
    pinMode(ENA, OUTPUT);
    pinMode(ENB, OUTPUT);

    Wire.begin(8);
    Wire.onRequest(requestEvent); // register event
    Wire.onReceive(receiveEvent); // register event
}

// function that executes whenever data is requested by master
void requestEvent() {
  Wire.write((char*)sensorData, sizeof(sensorData)); // respond with message of 1 byte
}

void receiveEvent(int bytes) 
{
  while (Wire.available()) { // loop through all
    char command = Wire.read();       // receive byte as a character
    }
}
  //x = Wire.read();    // receive byte as an integer

float getDistance()                                            // Function for measuring and calibrating frontal distance (inches)
{
    float echoTime ;                                            // Store time it takes for a pulse to bounce off an object and return
    float fDistance ;                                    // Store measured distance calculated from the echo time

    digitalWrite(fTrigPin, HIGH) ;                               // Begin pulse from trigger pin
    delayMicroseconds(10) ;                                     // For 10us
    digitalWrite(fTrigPin, LOW) ;                                // End pulse from trigger pin

    echoTime = pulseIn(fEchoPin, HIGH) ;                         // Store time it takes for echo pin to receive pulse return

    fDistance = echoTime / 148.0 ;                       // Calculate distance to the reflective object (half return time multiplied by speed of sound)
  
    return fDistance ; 
}

void forward(int carSpeed)  // Forward, cap at 255
{
  // Setting Direction and Power Pins
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  // Write speed to 'A' and 'B' MotorGroups
  analogWrite(ENA, carSpeed);
  analogWrite(ENB, carSpeed);
}

void backward(int carSpeed) {
  digitalWrite(IN2, HIGH);
  digitalWrite(IN3, HIGH);
  analogWrite(ENA, carSpeed);
  analogWrite(ENB, carSpeed);
}

void turnRight(int carSpeed) {
  digitalWrite(IN2, HIGH);
  digitalWrite(IN3, LOW);
  analogWrite(ENA, carSpeed);
  analogWrite(ENB, carSpeed);
}

void turnLeft(int carSpeed) {
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, HIGH);
  analogWrite(ENA, carSpeed);
  analogWrite(ENB, carSpeed);
}

void swingTurnLeft(int carSpeed) {
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  analogWrite(ENA, 0.25 * carSpeed);
  analogWrite(ENB, carSpeed);
}

void swingTurnRight(int carSpeed) {
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  analogWrite(ENA, carSpeed);
  analogWrite(ENB, 0.25 * carSpeed);
}

void swingTurnBackLeft(int carSpeed) {
  digitalWrite(IN2, HIGH);
  digitalWrite(IN3, HIGH);
  analogWrite(ENA, 0.25 * carSpeed);
  analogWrite(ENB, carSpeed);
}

void swingTurnBackRight(int carSpeed) {
  digitalWrite(IN2, HIGH);
  digitalWrite(IN3, HIGH);
  analogWrite(ENA, carSpeed);
  analogWrite(ENB, 0.25 * carSpeed);
}

void loop()
{
  fDistance = getDistance();

  if(fDistance > 30 || fDistance == 0)
  {
    int fDistanceState = 0;
  }
  else 
  {
    fDistanceState = 1;
  }

  Serial.print(fDistance);
  Serial.println(" inches");

  fLSensorState = digitalRead(fLeftSensor);
  fRSensorState = digitalRead(fRightSensor);

  sensorData[0] = fLSensorState;
  sensorData[1] = fRSensorState;
  sensorData[2] = fDistanceState;

  Serial.println(command);

  switch(command)
  {
    case 'f':
      forward(255);
      break;
    case 'b':
      backward(255);
      break;
    case 'r':
      turnRight(255);
      break;
    case 'l':
      turnLeft(255);
      break;
    case 'o':
      swingTurnLeft(255);
      break;
    case 't':
      swingTurnRight(255);
      break;
    case 'k':
      swingTurnBackRight(255);
      break;
    case 'w':
      swingTurnBackLeft(255);
      break;
  }

  delay(25);
}