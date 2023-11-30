#include <Wire.h>                                     

const int fTrigPin = 13 ;                                     // Redboard pin for trigger on the distance sensor       
const int fEchoPin = 12 ;                                     // Redboard pin for echo on the distance sensor

const int fRightSensor = 2 ;                                 // Redboard pin for right side reflectance sensor
const int fLeftSensor = 4 ;                                 // Same (but different pin) for left side

int fRSensorState = 0 ;                                      // Initialize right side sensor variable, 0 meaning false/high
int fLSensorState = 0 ;                                      // Same for left side

int spinDirection = 0 ;                                     // Initialize spin direction, 0 meaning spin counterclockwise. Switched to either 0 or 1 based on ring contact
float fDistance = 0 ;                                        // Initialize front sensor distance to 0 inches

// Speed control pins
const int ENA = 6;            //right side
const int ENB = 5;            //left side

// Direction control pins (LOCATIONS UNCERTAIN)
const int IN1 = 11;           //back right motor (close to board)
const int IN2 = 9;            //front right motor
const int IN3 = 8;            //front left motor
const int IN4 = 7;            //back left motor (close to board)

void setup()
{
    Serial.begin(9600) ;                                    // Set up serial connection with attached computer (pre-competition)

    pinMode(fTrigPin, OUTPUT);
    pinMode(fEchoPin, INPUT);

    pinMode(fLeftSensor, INPUT) ;                            // Declare assigned left sensor pin as input
    pinMode(fRightSensor, INPUT) ;                           // Declare assigned right sensor pin as input

    pinMode(IN1, OUTPUT);
    pinMode(IN2, OUTPUT);
    pinMode(IN3, OUTPUT);
    pinMode(IN4, OUTPUT);
    pinMode(ENA, OUTPUT);
    pinMode(ENB, OUTPUT);

    Wire.begin(8);
    Wire.onRequest(requestEvent); // register event
    Wire.onReceive(receiveEvent); // register event
}

// function that executes whenever data is requested by master
void requestEvent() {
  Wire.write(value); // respond with message of 1 byte
}

void receiveEvent(int bytes) {

  while (Wire.available()) { // loop through all
    char received = Wire.read();       // receive byte as a character
    if (((received == 'f')||(received == 'p')) || (received == 'b')){
      direction = received;
    }
    else{
      action = received;
    }
  }
  //x = Wire.read();    // receive byte as an integer
}


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
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
  // Write speed to 'A' and 'B' MotorGroups
  analogWrite(ENA, carSpeed);
  analogWrite(ENB, carSpeed);
}

void backward(int carSpeed) {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
  analogWrite(ENA, carSpeed);
  analogWrite(ENB, carSpeed);
}

void turnRight(int carSpeed) {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
  analogWrite(ENA, carSpeed);
  analogWrite(ENB, carSpeed);
}

void turnLeft(int carSpeed) {
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
  analogWrite(ENA, carSpeed);
  analogWrite(ENB, carSpeed);
}

void loop()
{
  fDistance = getDistance();

  Serial.print(fDistance);
  Serial.println(" inches");

  fLSensorState = digitalRead(fLeftSensor);
  fRSensorState = digitalRead(fRightSensor);

  if((fLSensorState == LOW) || (fRSensorState == LOW))
  {
    if(fLSensorState == LOW)
    {
      Wire.beginTransmission(8); // transmit to device #8
      Wire.write('r');          //marker to determine right sensor high
      Wire.endTransmission();
      Serial.print("r");
      spinDirection = 1;
      Serial.print(spinDirection);
      turnRight(255);
      delay(500);
    }
    if(fRSensorState == LOW)
    {
      Wire.beginTransmission(8); // transmit to device #8
      Wire.write('l');          //marker to determine right sensor high
      Wire.endTransmission();
      Serial.print("l");
      spinDirection = 0;
      Serial.print(spinDirection);
      turnLeft(255);
      delay(500);
    }
  }

  else if((fDistance > 30) || (fDistance == 0))
  {
    if(spinDirection == 0)
    {
      Wire.beginTransmission(8); // transmit to device #8
      Wire.write('l');          //marker to determine right sensor high
      Wire.endTransmission();
      Serial.print("l");
      turnLeft(255);
    }
    else if(spinDirection == 1)
    {
      Wire.beginTransmission(8); // transmit to device #8
      Wire.write('r');          //marker to determine right sensor high
      Wire.endTransmission();
      Serial.print("r");
      turnRight(255);
    }
  }

  else
  {
    Wire.beginTransmission(8) ; // transmit to device #8
    Wire.write('f') ;          //marker to determine right sensor high
    Wire.endTransmission() ;
    Serial.print("f") ;
    forward(255) ;
  }

  delay(25);
}