#include <Servo.h> //servo library 
#include <Wire.h>  //i2c wire library


// Reflectance sensor pins
const int rBR = 2;            //back right reflectance sensor
const int rBL = 4;            //back left reflectance sensor

int rBLState = 0;          // Initialize sensor variables
int rBRState = 0;

// Speed control pins
const int ENA = 5;            //left side
const int ENB = 6;            //right side

// Direction control pins
const int IN1 = 7;            //back right motor 
const int IN2 = 8;            //front right motor
const int IN3 = 9;            //front left motor
const int IN4 = 11;           //back left motor (not used)

// Ultrasonic Sensor Pins
const int Echo = 12;
const int Trig = 13;

// Servo pin
const int servoPin = 3;

Servo myservo;

int x = 0;
char value;
char direction = 'p';
char action;
int distance;
int perpendicular = 70;   //ballpark, figure out later

void setup() {
  // Configure all of these pins as outputs
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);
  pinMode(ENA, OUTPUT);
  pinMode(ENB, OUTPUT);

  // Set the ultrasonic sensor pins
  pinMode(Echo, INPUT);
  pinMode(Trig, OUTPUT);

  // Set the servo pin
  myservo.attach(servoPin);

  pinMode(rBR, INPUT);   // Declare assigned sensor pins as input
  pinMode(rBL, INPUT);

  Wire.begin(8); //join i2c bus, set address to 8
  Wire.onRequest(requestEvent); // register event
  Wire.onReceive(receiveEvent); // register event

  Serial.begin(9600); // Start serial monitor
}

// function that executes whenever data is received from bottom board
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

// function that executes whenever data is requested by master
void requestEvent() {
  Wire.write(value); // respond with message of 1 byte
}

//     Left motor truth table
// Here are some handy tables to show the various modes of operation.
//  ENA         IN1             IN2         Description
//  LOW   Not Applicable   Not Applicable   Motor is off
//  HIGH        LOW             LOW         Motor is stopped (brakes)
//  HIGH        LOW             HIGH        Motor is on and turning forwards
//  HIGH        HIGH            LOW         Motor is on and turning backwards
//  HIGH        HIGH            HIGH        Motor is stopped (brakes)

//     Right motor truth table
// Here are some handy tables to show the various modes of operation.
//  ENB         IN3             IN4         Description
//  LOW   Not Applicable   Not Applicable   Motor is off
//  HIGH        LOW             LOW         Motor is stopped (brakes)
//  HIGH        LOW             HIGH        Motor is on and turning forwards
//  HIGH        HIGH            LOW         Motor is on and turning backwards
//  HIGH        HIGH            HIGH        Motor is stopped (brakes)


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

void back(int carSpeed) {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
  analogWrite(ENA, carSpeed);
  analogWrite(ENB, carSpeed);
}

void turnLeft(int carSpeed) {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
  analogWrite(ENA, carSpeed);
  analogWrite(ENB, carSpeed);
}

void turnRight(int carSpeed) {
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
  analogWrite(ENA, carSpeed);
  analogWrite(ENB, carSpeed);
}

//A swing turn is when only one side of the robot's drivebase is powered.
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

void stop() {
  digitalWrite(ENA, 0);
  digitalWrite(ENB, 0);
}

/**
 * @brief Get the distance (in) by the ultrasonic sensor
 * 
 * @return int The distance sensed by the ultrasonic sensor
 */
int getDistance() {
  // trigger an ultrasonic wave for 20 microseconds
  digitalWrite(Trig, LOW);
  delayMicroseconds(10);
  digitalWrite(Trig, HIGH);
  delayMicroseconds(10);
  digitalWrite(Trig, LOW);
  // calculation converting time until signal returned to distance
  float Fdistance = pulseIn(Echo, HIGH);
  Fdistance = Fdistance / 148.0;
  // returns an integer as the distance (in cm) sensed
  return Fdistance;
}

int panFind(){    //look to left, then right with ultrasonic sensor and return whether it needs to turn right (2) or left (1)
  myservo.write(perpendicular-10);
  delay(50);
  int distLeft = getDistance();
  myservo.write(perpendicular+10);
  delay(50);
  int distRight = getDistance();

  myservo.write(perpendicular);     //return to normal

  if (distLeft>distRight){
    return 1;
  }
  else{
    return 2;
  }
}

void loop() {
  
  rBRState = digitalRead(rBR);  //read right sensor
  rBLState = digitalRead(rBL);   //read left sensor 

  Serial.println(x);

  
  if(action == 'r'){
    swingTurnBackLeft(200);
    delay(50);    //give time to clear the line
    action = 'x';   //clear action
  }
  else if(action == 'l'){
    swingTurnBackRight(200);
    delay(50);
    action = 'x';
  }
  else if(rBRState == HIGH){          //If right sensor is over reflective material
    // swing turn forward left, tell front board to follow along with "i"
    value = 'i';
    swingTurnLeft(200);
    delay(50);
  }   
  else if(rBLState == HIGH){     //If Left sensor above reflective material 
    // swing turn forward right, tell front board to follow along with "e"
    value = 'e';
    swingTurnRight(200);
    delay(50);
  }

  else if(action == 's'){
    stop();
  }

  else{
    distance = getDistance(); // Get the distance from the ultrasonic sensor

    if ((distance < 30) && (direction != 'f')){     //seeing something and going in this board's direction or panning
      value = 'b';
      direction = 'b';

      back(100);
    }
    else if ((distance >= 30) && (direction == 'b')){     //detected in front but no longer see it
      value = 's';
      stop();                   //stop front

      if (panFind() == 1){
        turnLeft(200);
        delay(20);        //may have to decrease time or speed of turning, i just want about a 10 degree turn 
      }
      else{
        turnRight(200);
        delay(20);
      }
    }
    else if (direction == 'f'){
      //should be controlled by front board, this just follows along
      forward(100);
    }
    else{                           //default to panning mode
      value = 'p';
      turnRight(150);
    }
  }

  myservo.write(45);

  delay(100);
}