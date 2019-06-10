#include <DualTB9051FTGMotorShield.h>


const int PIN_SENSOR_LEFT = 13;
const int PIN_SENSOR_RIGHT = 11;
const int PIN_SENSOR_FRONT = 5;
const int PIN_MODE = 3;
const int PIN_POT_SENSITIVITY = A2;

const int maxTolerance = 20;
const int maxSpeed = 400;

DualTB9051FTGMotorShield md;
int mode = 0;
int whiteCount = 0;

void setup() {
  Serial.begin(19200);
  // put your setup code here, to run once:
  pinMode(PIN_SENSOR_LEFT, INPUT_PULLUP);
  pinMode(PIN_SENSOR_RIGHT, INPUT_PULLUP);
  pinMode(PIN_SENSOR_FRONT, INPUT_PULLUP);
  pinMode(PIN_MODE, INPUT_PULLUP);
  pinMode(PIN_POT_SENSITIVITY, INPUT);

  md.init();
  // Uncomment to flip a motor's direction:
  //md.flipM1(true);
  //md.flipM2(true);
  md.enableDrivers();
  delay(1); // wait for drivers to be enabled so fault pins are no longer low
}

void stopIfFault()
{
  // in debug mode
  if (mode == 1) return;

  if (md.getM1Fault())
  {
    Serial.println("M1 fault");
    md.disableDrivers();
    //    while(1);
    delay(5000);
    md.enableDrivers();
    return;
  }
  if (md.getM2Fault())
  {
    Serial.println("M2 fault");
    md.disableDrivers();
    //    while(1);
    \
    delay(5000);
    md.enableDrivers();
  }
}

void setSpeed(int m1, int m2) {
  if (mode == 1) {
    Serial.print("DBG:");
    // debug
    md.setM1Speed(0);
    md.setM2Speed(0);
  } else {
    md.setM1Speed(m1);
    md.setM2Speed(m2);
  }
  Serial.print(", M1=");
  Serial.print(m1);
  Serial.print(", M2=");
  Serial.print(m2);
}

void simple_algo(int sens_left, int sens_right, int sens_front, int v) {
  if ( sens_left == 1 && sens_right == 1 ) {
    // go straight and see where it leads
    setSpeed(v, v);
  } else if ( sens_left == 0 && sens_right == 0 ) {
    // go straight
    if (sens_front == 0 && whiteCount > maxTolerance) {
      // stop, we've lost the line
      v = 0;
    }
    setSpeed(v, v);
  } else if ( sens_left == 1 && sens_right == 0 ) {
    // turn left
    setSpeed(v, v / 8);
  } else if ( sens_left == 0 && sens_right == 1 ) {
    // turn right
    setSpeed(v / 8, v);
  }
  if (sens_front == 0 && sens_left == 0 && sens_right == 0) {
    whiteCount++;
  } else {
    whiteCount = 0;
  }
}

int lastError = 0;
const int proportional_constant = 4;
const int derivative_constant = 6;
// stolen from https://github.com/pololu/zumo-32u4-arduino-library/blob/master/examples/LineFollower/LineFollower.ino
void proportional_algo(int sens_left, int sens_right, int sens_front, int v) {
  // TODO: calc position from sensors as interval (0, 4000)
  int pos;
  if (sens_left == 0 && sens_front == 0 && sens_right == 0) {
    // no black line in sight - stop
    whiteCount++;
    if (whiteCount > maxTolerance) {
      whiteCount = maxTolerance;
      setSpeed(0, 0);
      return;
    }
  } else {
    whiteCount = 0;    
  }
  if (sens_left == 0 && sens_right == 0) {
    // we're in the middle
    pos = 2000;
  } else if (sens_left == 1 && sens_right == 0) {
    // we're drifting towards right
    pos = 3000;
    if (sens_front == 0) { // looks like our nose sticks out of a curve
      pos = 4000;
    }
  } else if (sens_left == 0 && sens_right == 1) {
    // we're drifting towards left
    pos = 1000;
    if (sens_front == 0) { // looks like our nose sticks out of a curve
      pos = 0;
    }
  } else if (sens_left == 1 && sens_right == 1) {
    // likely a junction, just keep straight to find the exit
    pos = 2000;
    if (sens_front == 0) { // unless our nose doesn't see anything, we should turn until we find something
        setSpeed(50, -50);
    }
  }

  int error = pos - 2000;
  int speedDifference = error / proportional_constant + derivative_constant * (error - lastError);

  lastError = error;
  int leftSpeed = v + speedDifference;
  int rightSpeed = v - speedDifference;

  // Constrain our motor speeds to be between 0 and maxSpeed.
  // One motor will always be turning at maxSpeed, and the other
  // will be at maxSpeed-|speedDifference| if that is positive,
  // else it will be stationary.  For some applications, you
  // might want to allow the motor speed to go negative so that
  // it can spin in reverse.
  leftSpeed = constrain(leftSpeed, 0, v);
  rightSpeed = constrain(rightSpeed, 0, v);

  setSpeed(-leftSpeed, -rightSpeed);

  Serial.print("pos:");
  Serial.print(pos);
  Serial.print("error:");
  Serial.print(error);
  Serial.print("speedDifference:");
  Serial.print(speedDifference);
  Serial.print("leftSpeed:");
  Serial.print(leftSpeed);
  Serial.print("rightSpeed:");
  Serial.print(rightSpeed);
}

void loop() {
  //  delay(10);
  stopIfFault();

  mode = digitalRead(PIN_MODE);
  int sens_left = digitalRead(PIN_SENSOR_LEFT);
  int sens_right = digitalRead(PIN_SENSOR_RIGHT);
  int sens_front = digitalRead(PIN_SENSOR_FRONT);
  int pot_sensitivity = analogRead(PIN_POT_SENSITIVITY);
  int v = map(pot_sensitivity, 0, 1023, 0, maxSpeed); // driver goes up to -400/400

  Serial.println("");
  if (mode == 1) {
    Serial.print("DBG: ");
  }
  Serial.print("front:");
  Serial.print(sens_front);
  Serial.print("left:");
  Serial.print(sens_left);
  Serial.print(", right:");
  Serial.print(sens_right);
  Serial.print(", v:");
  Serial.print(v);

  // TODO: calc position
  //simple_algo(sens_left, sens_right, sens_front, v);
  proportional_algo(sens_left, sens_right, sens_front, v);
}
