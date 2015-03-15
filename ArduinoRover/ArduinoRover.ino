#include <AFMotor.h>
#include <Servo.h>
#include <NewPing.h>

#define TRIGGER_PIN  17                          // Arduino A3 pin tied to trigger pin on the ultrasonic sensor.
#define ECHO_PIN     16                          // Arduino A2 pin tied to echo pin on the ultrasonic sensor.
#define MAX_DISTANCE 200                         // Maximum distance we want to ping for (in centimeters). Maximum sensor distance is rated at 400-500cm.
NewPing frontSonar(TRIGGER_PIN, ECHO_PIN, MAX_DISTANCE);       // NewPing setup of pins and maximum distance.
float centerDistance = 0;
float rightDistance = 0;
float leftDistance = 0;
long randomNumber = 0;
boolean isLastTurnRight = true;

#define RIGHT_TURN_ANGLE 27
#define LEFT_TURN_ANGLE 27
AF_DCMotor motorR(2, MOTOR12_64KHZ);             // Right motor connected to M2, 64KHz PWM
AF_DCMotor motorL(1, MOTOR12_64KHZ);             // Left motor connected to M1, 64KHz PWM
const int TURN_TIME = 500;

#define SERVO_LEFT_POSITION 48
#define SERVO_CENTER_POSITION 75
#define SERVO_RIGHT_POSITION 102
Servo frontServo;

void setup() {
  Serial.begin(115200);                         // set up Serial library at 115200 bps   
  frontServo.attach(10);
  frontServo.write(SERVO_CENTER_POSITION); 
}

void loop() {    
  motionPattern_1();
}

void motionPattern_1() {  
  centerDistance = getDistance(frontSonar);
  Serial.println(centerDistance);
  if (centerDistance <= 20 && centerDistance > 0) {
    stopMovement(1000);
    frontServo.write(SERVO_RIGHT_POSITION);    
    rightDistance = getDistance(frontSonar);
    delay(1000);
    frontServo.write(SERVO_LEFT_POSITION);
    leftDistance = getDistance(frontSonar);
    delay(1000);
    Serial.print(leftDistance);
    Serial.print("  :  ");
    Serial.println(rightDistance);
    if (rightDistance > leftDistance) {
      turnRight(); Serial.println( "turned right.");
      isLastTurnRight = true;
    }
    else if (rightDistance < leftDistance) {
      turnLeft(); Serial.println( "turned left."); 
      isLastTurnRight = false;
    }
    else {
//      randomSeed(analogRead(5));
//      randomNumber = random(2);
      if (isLastTurnRight) {turnRight(); Serial.println( "turned random right."); }
      else {turnLeft(); Serial.println( "turned random left."); }
      delay(50); 
    }
    frontServo.write(SERVO_CENTER_POSITION);
    delay(1000);
  } else {
    moveForward(220);
  }
}

float getDistance(NewPing &sonar) {
  delay(50);                      // Wait 50ms between pings (about 20 pings/sec). 29ms should be the shortest delay between pings.
  return sonar.ping_cm();
//  unsigned int uS = sonar.ping();
//  return (uS / US_ROUNDTRIP_CM);  
}

void moveForward(int speedCode) {
  moveForward(speedCode, 0);
}

void moveForward(int speedCode, int timeDelay) {
  motorR.setSpeed(speedCode);
  motorL.setSpeed(speedCode);
  motorR.run(FORWARD);
  motorL.run(FORWARD);  
  if (timeDelay > 0) delay(timeDelay);
}

void moveBackward(int speedCode) {
  moveBackward(speedCode, 0);
}

void moveBackward(int speedCode, int timeDelay) {
  motorR.setSpeed(speedCode);
  motorL.setSpeed(speedCode);
  motorR.run(BACKWARD);
  motorL.run(BACKWARD);
  if (timeDelay > 0) delay(timeDelay);
}

void stopMovement() {
  stopMovement(0);
}

void stopMovement(int timeDelay) {
  motorR.setSpeed(0);
  motorL.setSpeed(0);
  motorR.run(BRAKE);
  motorL.run(BRAKE);
  if (timeDelay > 0) delay(timeDelay);
}

void turnRight() {
  turn(motorL, motorR);
}

void turnLeft() {
  turn(motorR, motorL);
}

void turn(AF_DCMotor &turnToMotor, AF_DCMotor &turnAwayMotor) {
  turnToMotor.setSpeed(200);
  turnAwayMotor.setSpeed(200);
  turnToMotor.run(FORWARD);
  turnAwayMotor.run(BACKWARD);
  delay(TURN_TIME); 
  stopMovement(); 
}
