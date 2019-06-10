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

uint16_t maxVal = 0, minVal = 1024;

void setup()
{
  // configure the sensors
  qtr.setTypeAnalog();
  qtr.setSensorPins((const uint8_t[]) {
    A0, A1
  }, SensorCount);
  //  qtr.setEmitterPin(2);

  Serial.begin(9600);
  delay(10); // let everything settle

  // calibrate
  Serial.println("");
  Serial.println("Start Calibration");
  // TODO: make robot "wiggle"
  delay(2000);
  for (int i = 0; i < 250; i++) {
    Serial.println("");
    Serial.print("Measuring");
    // read raw sensor values
    qtr.read(sensorValues);
    if (sensorValues[0] > maxVal) maxVal = sensorValues[0];
    if (sensorValues[0] < minVal) minVal = sensorValues[0];
    Serial.println("");
    Serial.print("[ max=");
    Serial.print(maxVal);
    Serial.print(",\tmin=");
    Serial.print(minVal);
    Serial.print(" ]");
    printSensorValues(sensorValues);

    delay(25);
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

void loop()
{
  Serial.println();

  // read raw sensor values
  qtr.read(sensorValues);

  printSensorValues(sensorValues);
  delay(100);
}
