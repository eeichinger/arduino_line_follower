#include <QTRSensors.h>

#include <DualTB9051FTGMotorShield.h>


const int PIN_MODE = 3;
const int PIN_POT_SENSITIVITY = A2;

const int maxTolerance = 20;
const int maxSpeed = 400;

DualTB9051FTGMotorShield md;
QTRSensors qtr;

const uint8_t SensorCount = 2;
uint16_t sensorValues[SensorCount];

const QTRReadMode readMode = QTRReadMode::On;




int mode = 0;
int whiteCount = 0;
const int wiggleWidth = 30;

void setup() {
  Serial.begin(9600);
  // put your setup code here, to run once:
  // configure the sensors
  qtr.setTypeAnalog();
  qtr.setSensorPins((const uint8_t[]) {
    A3, A4
  }, SensorCount);

  pinMode(PIN_MODE, INPUT_PULLUP);
  pinMode(PIN_POT_SENSITIVITY, INPUT);

  md.init();
  // Uncomment to flip a motor's direction:
  //md.flipM1(true);
  //md.flipM2(true);
  md.enableDrivers();
  delay(10); // wait for drivers to be enabled so fault pins are no longer low

  mode = digitalRead(PIN_MODE);

  // calibrate
  Serial.println("");
  Serial.println("Start Calibration");
  Serial.println("get ready to wiggle robot");
  // TODO: make robot "wiggle"
  delay(2000);
  Serial.println("wiggle robot now");
  for (int i = 0; i < wiggleWidth / 2; i++) {
    qtr.calibrate(readMode);
    setSpeed(-200, 200);
    delay(20);
  }
  for (int j = 0; j < 4; j++) {
    for (int i = 0; i < wiggleWidth; i++) {
      qtr.calibrate(readMode);
      if (j % 2 == 0) {
        setSpeed(200, -200);
      } else {
        setSpeed(-200, 200);
      }
      delay(20);
    }
  }
  Serial.println("");
  Serial.println("searching line");
  int pos = 0;
  while (pos < 500) {
    Serial.print("cali-pos=");
    //    qtr.calibrate(readMode);
    pos = calcPos1000(); //qtr.readLineBlack(sensorValues, readMode);
    Serial.print("cali-pos=");
    Serial.println(pos);
    setSpeed(100, -100);
    delay(20);
  }
  Serial.println("\nFinished Calibration");
  setSpeed(0, 0);
  pos = qtr.readLineBlack(sensorValues, readMode);
  Serial.print("cali-pos=");
  Serial.println(pos);
//  delay(1000);
//  pos = qtr.readLineBlack(sensorValues, readMode);
//  Serial.print("cali-pos=");
//  Serial.println(pos);
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

//void simple_algo(int sens_left, int sens_right, int sens_front, int v) {
//  int sens_left = digitalRead(PIN_SENSOR_LEFT);
//  int sens_right = digitalRead(PIN_SENSOR_RIGHT);
//  int sens_front = digitalRead(PIN_SENSOR_FRONT);
//  Serial.print("front:");
//  Serial.print(sens_front);
//  Serial.print("left:");
//  Serial.print(sens_left);
//  Serial.print(", right:");
//  Serial.print(sens_right);
//
//  if ( sens_left == 1 && sens_right == 1 ) {
//    // go straight and see where it leads
//    setSpeed(v, v);
//  } else if ( sens_left == 0 && sens_right == 0 ) {
//    // go straight
//    if (sens_front == 0 && whiteCount > maxTolerance) {
//      // stop, we've lost the line
//      v = 0;
//    }
//    setSpeed(v, v);
//  } else if ( sens_left == 1 && sens_right == 0 ) {
//    // turn left
//    setSpeed(v, v / 8);
//  } else if ( sens_left == 0 && sens_right == 1 ) {
//    // turn right
//    setSpeed(v / 8, v);
//  }
//  if (sens_front == 0 && sens_left == 0 && sens_right == 0) {
//    whiteCount++;
//  } else {
//    whiteCount = 0;
//  }
//}

uint16_t calcPos1000() {
  // read raw sensor values
  uint16_t pos = qtr.readLineBlack(sensorValues, readMode);
  Serial.print("pos1: ");
  Serial.print(pos);
  return pos;
//  if (pos == 0) {
//    pos = sensorValues[0];
//  } else if (pos == 1000) {
//    pos = 3000 - sensorValues[1];
//  } else {
//    pos = pos + 1000;
//  }
//  return pos * 3.333;
}

int lastError = 0;
const float kp = 1;
const float kd = 0.3;

//int whiteCount = 0;
// stolen from https://github.com/pololu/zumo-32u4-arduino-library/blob/master/examples/LineFollower/LineFollower.ino
void proportional_algo(int v) {
  // read raw sensor values
  int pos = calcPos1000(); //qtr.readLineBlack(sensorValues, readMode);
  if (sensorValues[0] < 50 && sensorValues[1] < 50) {
    if (whiteCount > 5) {
      //     lost line, stop
      setSpeed(0, 0);
//      while (1);
      return;
    }
    whiteCount++;
  } else {
    whiteCount = 0;
  }
  
  int error = pos - 500;
//  int err_sgn = error/abs(error);
//  error = error*error*err_sgn;
  int speedDifference = error * kp + (error - lastError) * kd;

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

  Serial.print(", pos:");
  Serial.print(pos);
  Serial.print(", error:");
  Serial.print(error);
  Serial.print(", lastError:");
  Serial.print(lastError);
  Serial.print(", speedDifference:");
  Serial.print(speedDifference);
  Serial.print(", leftSpeed:");
  Serial.print(leftSpeed);
  Serial.print(", rightSpeed:");
  Serial.print(rightSpeed);

  lastError = error;
}

void loop() {
  delay(10);

  stopIfFault();

  mode = digitalRead(PIN_MODE);
  int pot_sensitivity = analogRead(PIN_POT_SENSITIVITY);
  int v = 400; //map(pot_sensitivity, 0, 1023, 0, maxSpeed); // driver goes up to -400/400

  Serial.println("");
  if (mode == 1) {
    Serial.print("DBG: ");
  }
  Serial.print(", v:");
  Serial.print(v);

  //simple_algo(v);
  proportional_algo(v);
}
