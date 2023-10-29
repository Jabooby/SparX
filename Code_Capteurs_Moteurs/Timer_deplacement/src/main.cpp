#include <Arduino.h>
#include <LibRobus.h>
#include <robot_sparX.h>
#include <QTRSensors.h>


QTRSensors sensor; //classe QTR

const uint8_t SensorCount = 8;
uint16_t sensorValues[SensorCount];
QTRReadMode mode = QTRReadMode::On;
const float kP = 15;
const float threshold = 4.5;
float degre = 90.0;

// put function declarations here:
int myFunction(int, int);
float deplacer(float angle);
void acceleration();
float Follow();
float followV2();

void setup() {
  // put your setup code here, to run once:
  BoardInit();

  //start de timer
  sparx.startTimer = millis();
  sparx.timerRunning = true;
  sparx.moteurs.vitesse_voulue = 0.3;
  delay(100);
  //setup du suiveur de lignes
  float difference = 0;
  float adjust = 0;
  Serial.begin(9600);
  sensor.setTypeRC();
  sensor.setSensorPins((const uint8_t[]){38, 39, 40, 41, 42, 43, 44, 45}, SensorCount);
  sensor.setEmitterPin(37);
  

}

void loop() {
  // timer de 50ms, le code dans le if est executé au 50 ms 
  // changer TIMER_TIMER dans robot_sparX.h pour changer le temps
  if (sparx.timerRunning && ((millis() - sparx.startTimer) > TIMER_TIME))
  {
    sparx.startTimer += TIMER_TIME;
    degre += followV2();
    deplacer(degre);
    //deplacer(90.0);
  }
}
/*
 * @brief déplace selon l'angle voulue. 0 est à droite, 180 à gauche, 90 tout droit
 * @param angle : <90 droite (horaire), >90 gauche (antihoraire)
 * @return float: retourne le radius
 */
float Follow()
{
  // read raw sensor values
  float index = 0;
  float somme_capteurs = 0;
  float moyenne_capteurs = 0; //moyenne des numéros de capteurs
  float conversion = 0;
  float difference = 0;
  sensor.read(sensorValues);
 
  // print the sensor values as numbers from 0 to 2500, where 0 means maximum
  // reflectance and 2500 means minimum reflectance
  for (uint8_t i = 0; i < SensorCount; i++)
  {
    if( sensorValues[i] > 1250)
    { 
      index++;  
      somme_capteurs += i + 1; 
    }
  }
  moyenne_capteurs = (somme_capteurs)/(index);
  
  difference = moyenne_capteurs - threshold;
  conversion = (difference*kP) + 90;
  return conversion;
}

float followV2()
{
  int position = 0;
  int error = 0;
  sensor.read(sensorValues, mode);
  for (uint8_t i = 0; i < SensorCount; i++)
  {
    if(sensorValues[i] > 2000)
    {
      position = 1000 * i;
      break;
    }
    else
    {
      position = 7000;
    }
  }
  error = position - 3500; //si capteur 3 ou 4 on fait rien
  if((error < 1000) && (error > -1000))
  {
    degre = 90.0;
    return 0.0;
  }
  else
  {
    if ((degre < 125) && (degre > 55))
      return (error / 500);
    else
      return 0.0;
  }
}


float deplacer(float angle)
{
  float radius = 0;
  float radians = DEG_TO_RAD *(angle+45);
  acceleration();
  //voir excel pour les formules
  float v_Droite = -(sparx.moteurs.vitesse_moment*cos(radians))/COS_45_RAD;

  float v_Gauche = (sparx.moteurs.vitesse_moment*sin(radians))/COS_45_RAD;
  if(((v_Droite >=-0.001) && (v_Droite <=0.001)) || ((v_Gauche >=-0.001) && (v_Gauche <=0.001)))
  {
    sparx.moteurs.vitesse_moteur_droite = v_Droite;
    sparx.moteurs.vitesse_moteur_gauche = v_Gauche;
  }
  else
    PID(v_Gauche, v_Droite); 
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
    sparx.moteurs.vitesse_moment += 0.025;
  //diminue la vitesse
  else if (sparx.moteurs.vitesse_moment > sparx.moteurs.vitesse_voulue)
    sparx.moteurs.vitesse_moment -= 0.025;
  // reste pareille
  else
    sparx.moteurs.vitesse_moment = sparx.moteurs.vitesse_voulue;
}

// put function definitions here:
int myFunction(int x, int y) {
  return x + y;
}