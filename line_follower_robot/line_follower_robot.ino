#include <DualTB9051FTGMotorShield.h>


const int PIN_SENSOR_LEFT = 13;
const int PIN_SENSOR_RIGHT = 11;
const int PIN_SENSOR_FRONT = 5;
const int PIN_MODE = 3;
const int PIN_POT_SENSITIVITY = A2;

DualTB9051FTGMotorShield md;
int mode = 0;
const int maxTolerance=10;
int whiteCount = 0;

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
  Serial.print("M1=");
  Serial.print(m1);
  Serial.print(", M2=");
  Serial.print(m2);
}

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

void loop() {
  //  delay(10);
  stopIfFault();

  //  int pot_sensitivity = analogRead(PIN_POT_SENSITIVITY);
  //  int speed = map(pot_sensitivity, 0, 1023, 0, 400); // driver goes up to -400/400
  //  md.setM1Speed(speed);
  //  md.setM2Speed(speed);
  //  return;

  mode = digitalRead(PIN_MODE);
  int sens_left = digitalRead(PIN_SENSOR_LEFT);
  int sens_right = digitalRead(PIN_SENSOR_RIGHT);
  int sens_front = digitalRead(PIN_SENSOR_FRONT);
  int pot_sensitivity = analogRead(PIN_POT_SENSITIVITY);
  int speed = -map(pot_sensitivity, 0, 1023, 0, 400); // driver goes up to -400/400

  Serial.println("");
  if ( sens_left == 1 && sens_right == 1 ) {
    // go straight and see where it leads
    setSpeed(speed, speed);
  } else if ( sens_left == 0 && sens_right == 0 ) {
    // go straight
    if (sens_front == 0 && whiteCount > maxTolerance) {
      // stop, we've lost the line
      speed = 0;
    }
    setSpeed(speed, speed);
  } else if ( sens_left == 1 && sens_right == 0 ) {
    // turn left
    setSpeed(speed, speed / 8);
  } else if ( sens_left == 0 && sens_right == 1 ) {
    // turn right
    setSpeed(speed / 8, speed);
  }
  if (sens_front == 0 && sens_left == 0 && sens_right==0) {
    whiteCount++;    
  } else {
    whiteCount=0;
  }
  

  Serial.print(", left:");
  Serial.print(sens_left);
  Serial.print(", right:");
  Serial.print(sens_right);
  Serial.print(", sensitivity:");
  Serial.print(pot_sensitivity);
}
