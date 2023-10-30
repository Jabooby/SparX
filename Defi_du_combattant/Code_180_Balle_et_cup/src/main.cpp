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
float followV2();

SharpIR irDroite(IR_PIN[IR_DROITE], MODEL_IR);
SharpIR irGauche(IR_PIN[IR_GAUCHE], MODEL_IR);
uint16_t sensorValues[SENSOR_COUNT];
QTRSensors sensor; //classe QTR
QTRReadMode mode = QTRReadMode::On;
Adafruit_TCS34725 tcs = Adafruit_TCS34725(TCS34725_INTEGRATIONTIME_24MS, TCS34725_GAIN_4X);

void setup() {
   BoardInit();

  //start de timer
  sparx.startTimer = millis();
  sparx.timerRunning = true;
  sparx.moteurs.vitesse_voulue = 0.3;
  SERVO_Enable(SERVO_1);
  SERVO_SetAngle(SERVO_1, 60); //60 == 90 degré
  sparx.orientation = 90.0;
  sensor.setTypeRC();
  sensor.setSensorPins((const uint8_t[]){38, 39, 40, 41, 42, 43, 44, 45}, SENSOR_COUNT);
  sensor.setEmitterPin(37);
}

void loop() {
  if (sparx.timerRunning && ((millis() - sparx.startTimer) > TIMER_TIME))
  {
    sparx.startTimer += TIMER_TIME; //toujours la premiere chose dans le IF
    deplacer(sparx.orientation); //toujours la dernière chose à faire dans le IF
  }
}

/*
 * @brief déplace selon l'angle voulue. 0 est à droite, 180 à gauche, 90 tout droit
 * @param angle : <90 droite (horaire), >90 gauche (antihoraire)
 * @return float: retourne le radius
 */
float deplacer(float angle)
{
  float radius = 0;
  float radians = DEG_TO_RAD *(angle+45);
  acceleration();
  //voir excel pour les formules
  float v_Droite = -(sparx.moteurs.vitesse_moment*cos(radians))/COS_45_RAD;
  float v_Gauche = (sparx.moteurs.vitesse_moment*sin(radians))/COS_45_RAD;
  if(((v_Droite >= v_Gauche - 0.005) && (v_Droite <= v_Gauche +0.005)))
  {
    PID(v_Gauche, v_Droite);
  }
  else
  {
    sparx.moteurs.vitesse_moteur_droite = v_Droite;
    sparx.moteurs.vitesse_moteur_gauche = v_Gauche;
  }
  //Serial.println(sparx.moteurs.vitesse_moteur_gauche);
  MOTOR_SetSpeed(RIGHT,sparx.moteurs.vitesse_moteur_droite);
  MOTOR_SetSpeed(LEFT, sparx.moteurs.vitesse_moteur_gauche);
  //calcul radius
  radius = (distance_entre_roues_m/2)*((sparx.moteurs.vitesse_moteur_droite + sparx.moteurs.vitesse_moteur_gauche)/(sparx.moteurs.vitesse_moteur_droite - sparx.moteurs.vitesse_moteur_gauche));
  ENCODER_Reset(0);
  ENCODER_Reset(1);
  return(radius);
}
/*
 * @brief fait une accélération de la vitesse et déceleration dépendamment de valeur voulue
 * @param rien
 * @return 
 */
void acceleration()
{
  //augmente la vitesse
  if(sparx.moteurs.vitesse_moment < sparx.moteurs.vitesse_voulue)
    sparx.moteurs.vitesse_moment += 0.02;
  //diminue la vitesse
  else if (sparx.moteurs.vitesse_moment > sparx.moteurs.vitesse_voulue)
    sparx.moteurs.vitesse_moment -= 0.02;
  // reste pareille
  else
    sparx.moteurs.vitesse_moment = sparx.moteurs.vitesse_voulue;
}

float followV2()
{
  int position = 0;
  int error = 0;
  sensor.read(sensorValues, mode);
  for (uint8_t i = 0; i < SENSOR_COUNT; i++)
  {
    if(sensorValues[i] > 2000)
    {
      position = 1000 * i;
      break;
    }
    else
    {
      position = 8000;
    }
  }
  error = position - 3500; //si capteur 3 ou 4 on va tout droit
  if((error < 1000) && (error > -1000))
  {
    sparx.orientation = 90.0;
    return 0.0;
  }
  /*
  else if (position == 8000 && CouleurDebut == 'J') //8000 == dans le blanc, aucune ligne
  {
    sparx.orientation = 75.0;
    return 0.0;
  }
  else if (position == 8000 && CouleurDebut == 'V') //8000 == dans le blanc, aucune ligne
  {
    sparx.orientation = 115.0;
    return 0.0;
  }*/
  else if (position == 8000) //8000 == dans le blanc, aucune ligne
  {
    sparx.orientation = 90.0;
    return 0.0;
  }
  else
  {
    if ((sparx.orientation < 130) && (sparx.orientation > 50)) //limite les angles
      return (error / 250);
    else
      return 0.0;
  }
}

// put function definitions here:
int myFunction(int x, int y) {
  return x + y;
}