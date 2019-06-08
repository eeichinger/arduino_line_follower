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
    //    while(1);
    delay(5000);
    md.enableDrivers();
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

int blackcount = 0;

void loop() {
//  delay(5);
  stopIfFault();

  //  int pot_sensitivity = analogRead(PIN_POT_SENSITIVITY);
  //  int speed = map(pot_sensitivity, 0, 1023, 0, 400); // driver goes up to -400/400
  //  md.setM1Speed(speed);
  //  md.setM2Speed(speed);
  //  return;

  int sens_left = digitalRead(PIN_SENSOR_LEFT);
  int sens_right = digitalRead(PIN_SENSOR_RIGHT);
  int pot_sensitivity = analogRead(PIN_POT_SENSITIVITY);
  int speed = map(pot_sensitivity, 0, 1023, 0, 400); // driver goes up to -400/400
  Serial.println("");
  if ( sens_left == 1 && sens_right == 1 ) {
    // stop
    blackcount++;
    if (blackcount > 4) {
      blackcount = 4;
      md.setM1Speed(0);
      md.setM2Speed(0);
      Serial.print("M1=000, M2=000");
    } else {
      md.setM1Speed(speed);
      md.setM2Speed(speed);
      Serial.print("M1=");
      Serial.print(speed);
      Serial.print(", M2=");
      Serial.print(speed);
    }
  } else if ( sens_left == 0 && sens_right == 0 ) {
    blackcount = 0;
    // go straight
    md.setM1Speed(speed);
    md.setM2Speed(speed);
    Serial.print("M1=");
    Serial.print(speed);
    Serial.print(", M2=");
    Serial.print(speed);
  } else if ( sens_left == 1 && sens_right == 0 ) {
    blackcount = 0;
    // turn left
    md.setM1Speed(speed*1.25);
    md.setM2Speed(speed/6);
    Serial.print("M1=");
    Serial.print(speed);
    Serial.print(", M2=");
    Serial.print(0);
  } else if ( sens_left == 0 && sens_right == 1 ) {
    blackcount = 0;
    // turn right
    md.setM1Speed(speed/6);
    md.setM2Speed(speed*1.25);
    Serial.print("M1=");
    Serial.print(0);
    Serial.print(", M2=");
    Serial.print(speed);
  }

  Serial.print(", blackcount:");
  Serial.print(blackcount);
  Serial.print(", left:");
  Serial.print(sens_left);
  Serial.print(", right:");
  Serial.print(sens_right);
  Serial.print(", sensitivity:");
  Serial.print(pot_sensitivity);
}
