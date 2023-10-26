#include <Arduino.h>
#include <LibRobus.h>
#include <QTRSensors.h>

QTRSensors sensor; //classe QTR

const uint8_t SensorCount = 8;
uint16_t sensorValues[SensorCount];

void FollowLine()
{
  bool line = true;
  float eclairage = 0;
  float difference = 0;
  int threshold = 50;
  float adjust = 0;
  const float kP = 0.005;
  float vitesse = 0.5;

  while(line)
  {
    difference = (eclairage - threshold) ;
    adjust = (difference*kP); 

    MOTOR_SetSpeed(RIGHT, vitesse + adjust); //En supposant que le capteur est à gauche 
    MOTOR_SetSpeed(LEFT, vitesse - adjust);
  }
}


void setup()
{
  Serial.begin(9600);
  sensor.setTypeRC();
  sensor.setSensorPins((const uint8_t[]){3, 4, 5, 6, 7, 8, 9, 10}, SensorCount);
  sensor.setEmitterPin(2);
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