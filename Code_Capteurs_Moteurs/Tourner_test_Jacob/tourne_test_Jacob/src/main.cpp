/**
 * @file   main.cpp
 * @author P-29 SparX
 * @date  début 7 octobre 2023 - fin ???
 * @brief    
 * 
 */


/************************* INCLUDES *************************/
#include <Arduino.h>
#include <LibRobus.h>
/************************* CONSTANTES *************************/
#define diametre  7.62 //les roues ont 2 pouces de diametre
#define radius    diametre / 2
#define dist_entre_roue 17
#define cir_entre_roue dist_entre_roue*PI
/************************* DECLARATION DES ÉTATS MACHINE *************************/
enum direction {
  NORD          = 0, //pointe vers la fin (90degrée)
  EST           = 1, //pointe vers la droite (0degrée ou 360degrée)
  OUEST         = 2, //pointe vers la gauche (180 degrée)
  SUD           = 3, //pointe vers le début (270 degrée)
} direction; //la direction dépend de la direction du robot selon le point de départ

enum deplacement {
  ARRET         = 0, 
  AVANCE        = 1,
  TOURNE_DROITE = 2,
  TOURNE_GAUCHE = 3,
  RECULE        = 4,
} deplacement; //dicte le déplacement que le robot doit faire

/************************* DÉCLARATION DE STRUCTURE *************************/
struct PID {
  //constantes PID
  float kp = 0.00061;
  float ki = 0.000001;
  float kd = 0.000016;
  //variables de calcul PID
  float errsum  = 0.0;
  float prevErr = 0.0;
  float errdiff = 0;
};
struct robot {
  struct PID pid;
  char orientation = NORD;
  char mouvement = ARRET;
  float vitesse_moteur_gauche = 0.2;
  float vitesse_moteur_droite = 0.2;
  float vitesse = 0.0;
  bool detection = false;
  int position[2] = {0,1}; //position X et Y
  bool retour = false;
  bool depart = false;
  int vertpin = 53;
  int rougepin = 49;
  int analog1 = A0;
  int analog2 = A1;
};
/************************* VARIABLES GLOBALES *************************/
struct robot sparx; //création de la valeur global SparX. SparX est le robot et nous pouvons accéder
// les différentes fonctions du robot. Pour accéder le moteur gauche juste faire "Sparx.vitesse_moteur_gauche"
//variables timer
int counter = 0;
int timer = 0;
int reset = 0;
int interval = 50; //50 mS
bool rightangle = false;

/************************* DECLARATIONS DE FONCTIONS *************************/
void PID();
int getDistance();
void tournerDroite(int angle);
void actionDroit();
void arret();
void tournerGauche(int angle);
void actionGauche();

void setup() {
  // put your setup code here, to run once:
  BoardInit();
  sparx.pid.errsum = 0.0;
  pinMode(LED_BUILTIN,OUTPUT);//détecteur de proximité
  pinMode(sparx.vertpin, INPUT);
  pinMode(sparx.rougepin, INPUT);
  pinMode(sparx.analog1, INPUT_PULLUP);
  pinMode(sparx.analog2, INPUT_PULLUP);
  Serial.println("Hello World");
  delay(100);
}

void loop() {
  tournerDroite(90);
  tournerGauche(90);
  
  while(1)
  {

  }
  
}

/************************* DECLARATIONS DE FONCTIONS *************************/
void actionDroit(){
  MOTOR_SetSpeed(RIGHT,-sparx.vitesse_moteur_droite);
  MOTOR_SetSpeed(LEFT, sparx.vitesse_moteur_gauche);
};
void actionGauche(){
  MOTOR_SetSpeed(RIGHT,sparx.vitesse_moteur_droite);
  MOTOR_SetSpeed(LEFT, -sparx.vitesse_moteur_gauche);
};
void arret(){
  MOTOR_SetSpeed(RIGHT, 0);
  MOTOR_SetSpeed(LEFT, 0);
};
void PID(){
    //lire les valeurs des encodeurs
    float left = ENCODER_Read(0);
    float right = ENCODER_Read(1);
    //calcul d'erreur
    float error = abs(right) - abs(left);
    //calcul propotionelle
    float proportionalValue = abs(error * sparx.pid.kp);
    //calcul intégrale
    sparx.pid.errsum += error;
    float integralValue = abs(sparx.pid.ki * sparx.pid.errsum);
    //calcul dérivé
    float errdiff = error-sparx.pid.prevErr;
    sparx.pid.prevErr = error;
    float derivativeValue = abs(sparx.pid.kd*errdiff);
    //si la moteur gauche est plus lente
    if (error > 0){
      sparx.vitesse_moteur_gauche = sparx.vitesse + proportionalValue + integralValue + derivativeValue;

    } 
    //si la moteur gauche est plus vite
    else if (error < 0) {
      sparx.vitesse_moteur_gauche = sparx.vitesse - (proportionalValue + integralValue + derivativeValue);
      
    }
    else sparx.vitesse = sparx.vitesse;
}

//0 == distance des 2 moteurs 1 == distance moteurs gauche 2 == distance droite
float getDistance(char options){
  float tab_options[3] = {0.0, 0.0, 0.0};

  float gauche = ENCODER_Read(0);
  float droite = ENCODER_Read(1);

  float circumference = diametre * PI;

  float distanceGauche = (gauche/3200) * circumference;
  tab_options[1] = distanceGauche;

  float distanceDroite = (droite/3200) * circumference;
  tab_options[2] = distanceDroite;

  float distance = (distanceGauche+distanceDroite)/2;
  tab_options[0] = distance;

  return tab_options[options];
}

void tournerDroite(int angle)
{
  bool angleAtteint = false;
  float coefficient = 360.0 / angle;
  //Serial.println(coefficient);
  float distanceVoulue = cir_entre_roue / coefficient;
  //Serial.println(distanceVoulue);
  //Serial.println("calcul angle");
  PID();
  //Serial.println("PID");
  while(!angleAtteint)
  {
    
    if(getDistance(1) >= distanceVoulue)
    {
      //Serial.println(getDistance(1));
      angleAtteint = true;
    }
    else
    {
      actionDroit();
      //Serial.println(getDistance(1));
    }
  }
  //Serial.println("STOP");
  arret();
  ENCODER_ReadReset(0);
  ENCODER_ReadReset(1);
}

void tournerGauche(int angle)
{
  bool angleAtteint = false;
  float coefficient = 360.0 / angle;
  //Serial.println(coefficient);
  float distanceVoulue = cir_entre_roue / coefficient;
  //Serial.println(distanceVoulue);
  //Serial.println("calcul angle");
  PID();
  //Serial.println("PID");
  while(!angleAtteint)
  {
    
    if(getDistance(2) >= distanceVoulue)
    {
      //Serial.println(getDistance(1));
      angleAtteint = true;
    }
    else
    {
      actionGauche();
      //Serial.println(getDistance(1));
    }
  }
  //Serial.println("STOP");
  arret();
  ENCODER_ReadReset(0);
  ENCODER_ReadReset(1);
}
