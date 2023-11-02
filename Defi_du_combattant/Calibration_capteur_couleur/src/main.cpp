//capteur de son A14 = intensite = orange A15 = ambient = brun
#include <Arduino.h>
#include <LibRobus.h>
#include <robot_sparX.h>
#include <SharpIR.h>
#include <Wire.h>
#include <Adafruit_TCS34725.h>
#include <QTRSensors.h>

#define MODEL_IR 1080
#define SENSOR_COUNT 8
// put function declarations here:
int myFunction(int, int);
float deplacer(float angle);
void acceleration();
float detectionMur(int distanceMur);
void couleurStart();
void detectionVerre();
float followV2();
char retour_couleur();
bool start();
void getRawData_noDelay(uint16_t* r, uint16_t* g, uint16_t* b, uint16_t* c);

SharpIR irDroite(IR_PIN[IR_DROITE], MODEL_IR);
SharpIR irGauche(IR_PIN[IR_GAUCHE], MODEL_IR);
uint16_t sensorValues[SENSOR_COUNT];
QTRSensors sensor; //classe QTR
QTRReadMode mode = QTRReadMode::On;
Adafruit_TCS34725 tcs = Adafruit_TCS34725(TCS34725_INTEGRATIONTIME_24MS, TCS34725_GAIN_4X);
int nbTour = 1;
char CouleurDebut; //Red, Green, Blue, Clear
bool go = false;
int distanceRobotMur = 0;
int temp;
void setup() {
  // put your setup code here, to run once:
  BoardInit();
  if (tcs.begin()) 
  {
    //Serial.println("Found sensor");
    tcs.clearInterrupt();
   
 
}
sensor.setTypeRC();
  sensor.setSensorPins((const uint8_t[]){38, 39, 40, 41, 42, 43, 44, 45}, SENSOR_COUNT);
  sensor.setEmitterPin(37);
  distanceRobotMur = irDroite.distance();
}
void loop() {
  uint16_t r, g , b, c;
   char couleur;
   getRawData_noDelay(&r, &g, &b, &c);
  temp = tcs.calculateColorTemperature(r,g,b);
  Serial.println(temp);
 
}
void getRawData_noDelay(uint16_t* r, uint16_t* g, uint16_t* b, uint16_t* c) {
   // *c = tcs.read16(TCS34725_CDATAL);
   // *r = tcs.read16(TCS34725_RDATAL);
   // *g = tcs.read16(TCS34725_GDATAL);
   // *b = tcs.read16(TCS34725_BDATAH);
    tcs.getRawData(r,g,b,c);
}

