#include <Wire.h>                                    

const int rTrigPin = 13 ;                                     // Redboard pin for trigger on the distance sensor       
const int rEchoPin = 12 ;                                     // Redboard pin for echo on the distance sensor

const int rRightSensor = 2 ;                                 // Redboard pin for right side reflectance sensor
const int rLeftSensor = 4 ;                                 // Same (but different pin) for left side

int rRSensorState = 0 ;                                      // Initialize right side sensor variable, 0 meaning false/high
int rLSensorState = 0 ;                                      // Same for left side

int fRSensorState = 0 ; 
int fLSensorState = 0 ;

int spinDirection = 0 ;                                     // Initialize spin direction, 0 meaning spin counterclockwise. Switched to either 0 or 1 based on ring contact
float rDistance = 0 ;                                        // Initialize rear sensor distance to 0 inches

int fDistanceState = 0 ; 

int received[4] = {0, 0, 0, '\0'};

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

    pinMode(rTrigPin, OUTPUT);
    pinMode(rEchoPin, INPUT);

    pinMode(rLeftSensor, INPUT) ;                            // Declare assigned left sensor pin as input
    pinMode(rRightSensor, INPUT) ;                           // Declare assigned right sensor pin as input

    pinMode(IN1, OUTPUT);
    pinMode(IN2, OUTPUT);
    pinMode(IN3, OUTPUT);
    pinMode(IN4, OUTPUT);
    pinMode(ENA, OUTPUT);
    pinMode(ENB, OUTPUT);

    Wire.begin();
}

float getDistance() {                                           // Function for measuring and calibrating frontal distance (inches)
    float echoTime ;                                            // Store time it takes for a pulse to bounce off an object and return
    float rDistance ;                                    // Store measured distance calculated from the echo time

    digitalWrite(rTrigPin, HIGH) ;                               // Begin pulse from trigger pin
    delayMicroseconds(10) ;                                     // For 10us
    digitalWrite(rTrigPin, LOW) ;                                // End pulse from trigger pin

    echoTime = pulseIn(rEchoPin, HIGH) ;                         // Store time it takes for echo pin to receive pulse return

    rDistance = echoTime / 148.0 ;                       // Calculate distance to the reflective object (half return time multiplied by speed of sound)
  
    return rDistance ; 
}

void forward(int carSpeed){  // Forward, cap at 255
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

void swingTurnLeft(int carSpeed) {
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
  analogWrite(ENA, 0.25 * carSpeed);
  analogWrite(ENB, carSpeed);
}

void swingTurnRight(int carSpeed) {
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
  analogWrite(ENA, carSpeed);
  analogWrite(ENB, 0.25 * carSpeed);
}

void swingTurnBackLeft(int carSpeed) {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
  analogWrite(ENA, 0.25 * carSpeed);
  analogWrite(ENB, carSpeed);
}

void swingTurnBackRight(int carSpeed) {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
  analogWrite(ENA, carSpeed);
  analogWrite(ENB, 0.25 * carSpeed);
}

void loop(){
  Wire.requestFrom(8, 4);    // request 4 bytes from device 8 (last should be null)

  while (Wire.available()){ 
    received[0] = int(Wire.read());
    received[1] = int(Wire.read());   
    received[2] = int(Wire.read());  // 1 or 0
    received[3] = int(Wire.read());  //null

    fRSensorState = received[0] ;
    fLSensorState = received[1] ;
    fDistanceState = received[2] ;
  }

  rDistance = getDistance();

  //Serial.print(rDistance);
  //Serial.println(" inches");

  Serial.println(received[1]);

  rLSensorState = digitalRead(rLeftSensor);
  rRSensorState = digitalRead(rRightSensor);

  if(((rLSensorState == LOW) || (rRSensorState == LOW)) || ((fRSensorState == 0) || (fLSensorState == 0))) {
    if(rLSensorState == LOW)
    {
      Wire.beginTransmission(8); // transmit to device #8
      Wire.write('t');          //marker to determine right sensor high
      Wire.endTransmission();
      Serial.println("t");
      spinDirection = 0;
      //Serial.print(spinDirection);
      swingTurnRight(255);
      delay(100);
    }
    if(rRSensorState == LOW)
    {
      Wire.beginTransmission(8); // transmit to device #8
      Wire.write('o');          //marker to determine right sensor high
      Wire.endTransmission();
      Serial.println("o");
      spinDirection = 1;
      //Serial.print(spinDirection);
      swingTurnLeft(255);
      delay(100);
    }
    if(fRSensorState == 0)
    {
      Wire.beginTransmission(8);
      Wire.write('w');
      Wire.endTransmission();
      Serial.println("w");
      spinDirection = 0;
      //Serial.print(spinDirection);
      swingTurnBackLeft(255);
      delay(100);
    }
    if(fLSensorState == 0)
    {
      Wire.beginTransmission(8);
      Wire.write('k');
      Wire.endTransmission();
      Serial.println("k");
      spinDirection = 1;
      //Serial.print(spinDirection);
      swingTurnBackRight(255);
      delay(100);
    }
  }

  else if((rDistance > 25 || rDistance == 0) && (fDistanceState == 0))
  {
    if(spinDirection == 0)
    {
      Wire.beginTransmission(8); // transmit to device #8
      Wire.write('l');          //marker to determine right sensor high
      Wire.endTransmission();
      Serial.println("l");
      turnLeft(255);
    }
    //else if(spinDirection == 1){
    else{
      Wire.beginTransmission(8); // transmit to device #8
      Wire.write('r');          //marker to determine right sensor high
      Wire.endTransmission();
      Serial.println("r");
      turnRight(255);
    }
  }

  else if(fDistanceState == 1){
    Wire.beginTransmission(8); // transmit to device #8
    Wire.write('f');          //marker to determine right sensor high
    Wire.endTransmission();
    Serial.println("f");
    forward(255);
  }

  else if(rDistance < 25){
    Wire.beginTransmission(8);
    Wire.write('b');
    Wire.endTransmission();
    Serial.println("b");
    backward(255);
  }

  delay(25);
}