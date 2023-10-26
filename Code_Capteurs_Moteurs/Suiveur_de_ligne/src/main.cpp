#include <Arduino.h>
#include <LibRobus.h>
#include <QTRSensors.h>

QTRSensors sensor; //classe QTR

const uint8_t SensorCount = 8;
uint16_t sensorValues[SensorCount];


void setup()
{
  Serial.begin(9600);
  sensor.setTypeRC();
  sensor.setSensorPins((const uint8_t[]){A4, A5, A6, A7, A8, A9, A10, A11}, SensorCount);
  sensor.setEmitterPin(22);
}

void loop()
{
  // read raw sensor values
  sensor.read(sensorValues);
 
  // print the sensor values as numbers from 0 to 2500, where 0 means maximum
  // reflectance and 2500 means minimum reflectance
  for (uint8_t i = 0; i < SensorCount; i++)
  {
    Serial.print(sensorValues[i]);
    Serial.print('\t');
  }
  Serial.println();

  delay(250);
}