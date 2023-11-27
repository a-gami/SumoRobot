#include <Servo.h> //servo library 
#include <Wire.h> //i2c wire library


//reflectance sensor pins
const int rFR = 2;            //front right side sensor
const int rFL = 4;            //front left side sensor

int rFLState = 0;          // Initialize sensor variables
int rFRState = 0;

// Speed control pins
const int ENA = 5;            //left side
const int ENB = 6;            //right side

// Direction control pins
const int IN1 = 7;            //back right motor (not used)
const int IN2 = 8;            //front right motor
const int IN3 = 9;            //front left motor
const int IN4 = 11;           //back left motor

// Ultrasonic Sensor Pins
const int Echo = 12;
const int Trig = 13;

// Servo pin
const int servoPin = 3;

char direction = 'f';
char act;

Servo myservo;


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

  pinMode(rFR, INPUT);   // Declare assigned sensor pins as input
  pinMode(rFL, INPUT);

  Wire.begin(); //join i2c bus

  Serial.begin(9600);       // Start serial monitor
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
 * @brief Get the distance (cm) by the ultrasonic sensor
 * 
 * @return int The distance sensed by the ultrasonic sensor
 */
int getDistance() {
  // trigger an ultrasonic wave for 20 microseconds
  digitalWrite(Trig, LOW);
  delayMicroseconds(20);
  digitalWrite(Trig, HIGH);
  delayMicroseconds(20);
  digitalWrite(Trig, LOW);
  // calculation converting time until signal returned to distance
  float Fdistance = pulseIn(Echo, HIGH);
  Fdistance = Fdistance / 58;
  // returns an integer as the distance (in cm) sensed
  return (int)Fdistance;
}

void loop() {
  int distance = getDistance(); // Get the distance from the ultrasonic sensor
  //Serial.println(distance);
  rFRState = digitalRead(rFR);  //read right sensor
  rFLState = digitalRead(rFL);   //read left sensor 

  Wire.beginTransmission(8); // transmit to device #8
  Wire.write(distance);          
  Wire.endTransmission();    // stop transmitting


  /*
  Serial.print(rFRState);
  Serial.print("  ");
  Serial.println(rFLState);
  */

  /*
  if(rFRState == HIGH){          //If right sensor is over reflective material
    //Serial.println("Right Sensor");
    
    Wire.beginTransmission(8); // transmit to device #8
    Wire.write("l");          //marker to determine what is being sent (right sensor high)
    Wire.endTransmission();    // stop transmitting

    swingTurnBackLeft(100);
    delay(100);
  }   
  else if(rFLState == HIGH){     //If Left sensor above reflective material 
    //Serial.println("Left Sensor") ;
    Wire.beginTransmission(8); // transmit to device #8
    Wire.write("l");          //marker to determine what is being sent (left sensor high)
    Wire.endTransmission();    // stop transmitting

    swingTurnBackRight(100);
    delay(100);
  }
  else{
    
      forward(100);
    
  }*/
  
  myservo.write(65);

  delay(1000);
}