#include <DualTB9051FTGMotorShield.h>


const int PIN_SENSOR_LEFT = 13;
const int PIN_SENSOR_RIGHT = 11;
const int PIN_POT_SENSITIVITY = A2;

DualTB9051FTGMotorShield md;

void stopIfFault()
{
  if (md.getM1Fault())
  {
    Serial.println("M1 fault");
    md.disableDrivers();
    while (1);
  }
  if (md.getM2Fault())
  {
    Serial.println("M2 fault");
    md.disableDrivers();
    while (1);
  }
}

void setup() {
  Serial.begin(19200);
  // put your setup code here, to run once:
  pinMode(PIN_SENSOR_LEFT, INPUT_PULLUP);
  pinMode(PIN_SENSOR_RIGHT, INPUT_PULLUP);
  pinMode(PIN_POT_SENSITIVITY, INPUT);

  md.init();
  // Uncomment to flip a motor's direction:
  //md.flipM1(true);
  //md.flipM2(true);
  md.enableDrivers();
  delay(1); // wait for drivers to be enabled so fault pins are no longer low
}

void loop() {
  delay(50);
  stopIfFault();

  int sens_left = digitalRead(PIN_SENSOR_LEFT);
  int sens_right = digitalRead(PIN_SENSOR_RIGHT);
  int pot_sensitivity = analogRead(PIN_POT_SENSITIVITY);
  int speed = map(pot_sensitivity, 0, 1023, -100, 100); // driver goes up to -400/400
  if ( sens_left == 1 && sens_right == 1 ) {
    // stop
    md.setM1Speed(0);
    md.setM2Speed(0);
  } else if ( sens_left == 0 && sens_right == 0 ) {
    // go straight
    md.setM1Speed(speed);
    md.setM2Speed(speed);
  } else if ( sens_left == 1 && sens_right == 0 ) {
    // turn left
    md.setM1Speed(0);
    md.setM2Speed(speed);
  } else if ( sens_left == 0 && sens_right == 1 ) {
    // turn right
    md.setM1Speed(speed);
    md.setM2Speed(0);
  }

  Serial.println("");
  Serial.print("speed:");
  Serial.print(speed);
  Serial.print("left:");
  Serial.print(sens_left);
  Serial.print(", right:");
  Serial.print(sens_right);
  Serial.print(", sensitivity:");
  Serial.print(pot_sensitivity);
}
