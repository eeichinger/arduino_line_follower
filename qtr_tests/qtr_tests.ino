#include <QTRSensors.h>

// This example is designed for use with six analog QTR sensors. These
// reflectance sensors should be connected to analog pins A0 to A5. The
// sensors' emitter control pin (CTRL or LEDON) can optionally be connected to
// digital pin 2, or you can leave it disconnected and remove the call to
// setEmitterPin().
//
// The main loop of the example reads the raw sensor values (uncalibrated). You
// can test this by taping a piece of 3/4" black electrical tape to a piece of
// white paper and sliding the sensor across it. It prints the sensor values to
// the serial monitor as numbers from 0 (maximum reflectance) to 1023 (minimum
// reflectance).

QTRSensors qtr;

const uint8_t SensorCount = 2;
uint16_t sensorValues[SensorCount];

const QTRReadMode readMode = QTRReadMode::On;

void setup()
{
  // configure the sensors
  qtr.setTypeAnalog();
  qtr.setSensorPins((const uint8_t[]) {
    A2, A3
  }, SensorCount);
  //  qtr.setEmitterPin(2);

  Serial.begin(9600);
  delay(10); // let everything settle

  // calibrate
  Serial.println("");
  Serial.println("Start Calibration");
  Serial.println("get ready to wiggle robot");
  // TODO: make robot "wiggle"
  delay(2000);
  Serial.println("wiggle robot now");
  for (int i = 0; i < 250; i++) {
    qtr.calibrate(readMode);
    delay(20);
  }
  Serial.println("Finished Calibration");
  delay(2000);
}

void printSensorValues(uint16_t sensorValues[]) {
  //  Serial.println();
  Serial.print(", sensors[ ");
  for (uint8_t i = 0; i < SensorCount; i++)
  {
    Serial.print(sensorValues[i]);
    Serial.print('\t');
  }
  Serial.print(" ]");
}

uint16_t calcPos() {
  // read raw sensor values
  uint16_t pos = qtr.readLineBlack(sensorValues, readMode);
  Serial.print("pos1: ");
  Serial.print(pos);
  if (pos == 0) {
    pos = sensorValues[0];
  } else if (pos == 1000) {    
    pos = 3000 - sensorValues[1];
  } else {
    pos = pos + 1000;
  }  
  return pos*3.33;
}

void loop()
{
  Serial.println();

  // read raw sensor values
  uint16_t pos = calcPos();
  Serial.print(", pos2: ");
  Serial.print(pos);
  printSensorValues(sensorValues);
  delay(100);
}
