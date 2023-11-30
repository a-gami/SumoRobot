#include <Wire.h> 

char direction;

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

    pinMode(IN1, OUTPUT);
    pinMode(IN2, OUTPUT);
    pinMode(IN3, OUTPUT);
    pinMode(IN4, OUTPUT);
    pinMode(ENA, OUTPUT);
    pinMode(ENB, OUTPUT);

    Wire.begin(8);
    Wire.onReceive(receiveEvent);
}

void receiveEvent() {

  while (Wire.available()) { // loop through all
    direction = Wire.read();       // receive byte as a character
  }
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

void stop() {
  analogWrite(ENA, 0);
  analogWrite(ENB, 0);
}

void loop()
{
  switch(direction)
  {
    case 'f':
      forward(255);
      Serial.println("wahoo!");
      break;
    case 'r':
      turnRight(255);
      break;
    case 'l':
      turnLeft(255);
      break;
    default:
      stop();
      break;
  }

  Serial.println(direction);
}