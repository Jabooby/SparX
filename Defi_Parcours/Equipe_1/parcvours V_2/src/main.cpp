/**
 * @file   main.cpp
 * @author P-29 SparX
 * @date  début 28 septembre 2023 - fin ???
 * @brief    
 * 
 */


/************************* INCLUDES *************************/
#include <Arduino.h>
#include <LibRobus.h>
/************************* CONSTANTES *************************/
#define wow       "wow" //ceci est un exemple
#define pi        3.14159
#define diametre  7.62 //les roues ont 2 pouces de diametre
#define circonference_mm  76 * PI
#define radius    diametre / 2
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

enum ligne {
  VIDE          = 0,
  L_NORD        = 1, //ligne au Nord du centre
  L_EST         = 2, //ligne à l'Est du centre
  L_OUEST       = 3, //ligne à l'Ouest du centre
  L_SUD         = 4, //ligne au Sud du centre
  L_NORD_EST    = 5, //ligne Nord et Est
  L_NORD_OUEST  = 6, //ligne Nord et Ouest
  L_NORD_SUD    = 7, //ligne Nord et Sud
  L_EST_OUEST   = 8, //ligne Est et Ouest
  L_EST_SUD     = 9, //ligne Est et Sud
  L_OUEST_SUD   = 10, //ligne Ouest et Sud
  L_NORD_EST_OUEST=11, //ligne Nord, Est et Ouest
  L_NORD_EST_SUD=12, //ligne Nord, Est et Sud
  L_NORD_OUEST_SUD=13,//ligne Nord, Ouest et Sud
  L_EST_OUEST_SUD=14, //ligne Est, Ouest et Sud
  L_NORD_EST_OUEST_SUD=15,//ligne Nord, Est, Ouest et Sud
} ligne; //dicte l'emplacement des lignes par rapport au centre d'une case

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
struct moteurs {
  float vitesse_moteur_gauche = 0.3;
  float vitesse_moteur_droite = 0.3;
  float vitesse = 0.3;
  int encodeurGauche = 0;
  int encodeurDroite = 0;
};
struct robot {
  struct PID pid;
  struct moteurs moteurs;
  char orientation = NORD;
  char mouvement = ARRET;
  bool detection = false;
  int position[2] = {0,1}; //position Y et X
  bool retour = false;
  bool depart = false;
  int vertpin = 48;
  int rougepin = 49;
};
/************************* VARIABLES GLOBALES *************************/
struct robot sparx; //création de la valeur global SparX. SparX est le robot et nous pouvons accéder
// les différentes fonctions du robot. Pour accéder le moteur gauche juste faire "Sparx.vitesse_moteur_gauche"
//variables timer
int counter = 0;
int timer = 0;
int reset = 0;
int interval = 50; //50 mS
int analog1 = A0;
int analog2 = A1;
bool rightangle = false;

 int matrice_parcour[11][3]{
    {9,4,10},//1
    {2,0,3},//2
    {2,0,3},//3
    {2,0,3},//4
    {2,0,3},//5
    {2,0,3},//6
    {2,0,3},//7
    {2,0,3},//8
    {2,0,3},//9
    {11,11,11}, //10
    {15,15,15} //11
  };
/************************* DECLARATIONS DE FONCTIONS *************************/
int myFunction(int, int); //ceci est un exemple
void arret();
//void avance();
void recule();
void tourneDroit();
void actionDroit();
void tourneGauche();
void PID();
bool capteur_infrarouge();
bool start();
void getangle(float angle);
void verification_obstacle();
int getDistance();
void travel(float distance);
void tourneGauche();
bool verification_matrice(int y, int x, int direction);
  

void setup() {
  // put your setup code here, to run once:
  BoardInit();
  sparx.pid.errsum = 0.0;
  pinMode(LED_BUILTIN,OUTPUT);//détecteur de proximité
  pinMode(sparx.vertpin, INPUT);
  pinMode(sparx.rougepin, INPUT);
  pinMode(analog1, INPUT_PULLUP);
  pinMode(analog2, INPUT_PULLUP);
  delay(100);
}

void loop() {
  //mettre code sifflet
  //verif +bouge
  //tourneGauche();
  
  //travel(500);
  while(sparx.position[0] < 9)
  {
    verification_obstacle();
  }
  Serial.println("Position fin de code: ");
  Serial.println(sparx.position[0]);
  while(1);
  //Serial.println(capteur_infrarouge());
}

/*
 * @brief Fonction exemple qui fait une addition et retourne le résultat
 * @param x: première valeur que tu veux additionner;
 * @param y: deuxième valeur que tu veux additionner;
 * @return Retourne la somme des deux valeurs
 */
int myFunction(int x, int y) {
  return x + y;
}

void arret(){
  MOTOR_SetSpeed(RIGHT, 0);
  MOTOR_SetSpeed(LEFT, 0);
  ENCODER_Reset(0);
  ENCODER_Reset(1);
  /*timer = millis();
  while((timer+100) < millis());*/
};

//not used
/*void avance(){
  PID();
  int distance_init = getDistance();
  while(getDistance()- distance_init < 50)
  {
    PID();
    MOTOR_SetSpeed(RIGHT,sparx.moteurs.vitesse_moteur_droite);
    MOTOR_SetSpeed(LEFT, sparx.moteurs.vitesse_moteur_gauche);
  }
  arret();
};*/

/*void recule(){
  PID();
  int distance_init = getDistance();
  while(getDistance()- distance_init >= -50)
  {
    PID();
    MOTOR_SetSpeed(RIGHT,-sparx.vitesse_moteur_droite);
    MOTOR_SetSpeed(LEFT, -sparx.vitesse_moteur_gauche);
  }
  arret();
};*/

void actionDroit(){
  MOTOR_SetSpeed(RIGHT, -0.5*sparx.moteurs.vitesse_moteur_droite);
  MOTOR_SetSpeed(LEFT, 0.5*sparx.moteurs.vitesse_moteur_gauche);
};

void actionGauche(){
  MOTOR_SetSpeed(RIGHT, 0.5*sparx.moteurs.vitesse_moteur_droite);
  MOTOR_SetSpeed(LEFT, -0.5*sparx.moteurs.vitesse_moteur_gauche);
};

void PID(){
    //lire les valeurs des encodeurs
    sparx.moteurs.encodeurGauche = ENCODER_Read(0);
    sparx.moteurs.encodeurDroite = ENCODER_Read(1);
    //calcul d'erreur
    float error = abs(sparx.moteurs.encodeurDroite) - abs(sparx.moteurs.encodeurGauche);
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
      sparx.moteurs.vitesse_moteur_gauche = sparx.moteurs.vitesse + proportionalValue + integralValue + derivativeValue;

    } 
    //si la moteur gauche est plus vite
    else if (error < 0) {
      sparx.moteurs.vitesse_moteur_gauche = sparx.moteurs.vitesse - (proportionalValue + integralValue + derivativeValue);
      
    }
    else
    {
      //sparx.moteurs.vitesse = sparx.moteurs.vitesse;
      sparx.moteurs.vitesse_moteur_gauche = sparx.moteurs.vitesse;
      sparx.moteurs.vitesse_moteur_droite = sparx.moteurs.vitesse;
    }
    
}

void travel(float distance){
  float moyenne = (abs(sparx.moteurs.encodeurGauche) + abs(sparx.moteurs.encodeurDroite))/2;
  
  float circonference = 2*PI*38.1;
  float goalVal = (3200.0f*distance)/circonference;
  
  PID();
  while(moyenne < goalVal)
  {
    PID();
    /*sparx.moteurs.encodeurGauche = ENCODER_Read(0);
    sparx.moteurs.encodeurDroite = ENCODER_Read(1);*/
    moyenne = (abs(sparx.moteurs.encodeurGauche) + abs(sparx.moteurs.encodeurDroite))/2;    
    MOTOR_SetSpeed(RIGHT, sparx.moteurs.vitesse_moteur_droite);
    MOTOR_SetSpeed(LEFT, sparx.moteurs.vitesse_moteur_gauche);
  }
  arret();
}

bool capteur_infrarouge() {
  bool etat;
  bool vert = digitalRead(sparx.vertpin);
  bool rouge = digitalRead(sparx.rougepin);

  if (vert && rouge){ // aucun obstacle => avance
      etat = 0;
    }
  else {// obstacle==> avance pas
      etat = 1;
    }
  return etat;
}

int getDistance(){
  sparx.moteurs.encodeurGauche = ENCODER_Read(0);
  sparx.moteurs.encodeurDroite = ENCODER_Read(1);
  float circumference = diametre * PI;
  float distancel = (sparx.moteurs.encodeurGauche/3200) * circumference;
  float distancer = (sparx.moteurs.encodeurDroite/3200) * circumference;
  float distance = (distancel+distancer)/2;
  
  return distance;

}
bool verification_matrice(int y, int x, int direction)
{
  int etatCase = matrice_parcour[y][x];
  switch(direction)
  {
    case NORD:
      switch(etatCase)
      {
        case L_NORD:
          return true;
          break;
        case L_NORD_EST:
          return true;
          break;
        case L_NORD_EST_OUEST:
          return true;
          break;
        case L_NORD_EST_SUD:
          return true;
          break;
        case L_NORD_OUEST:
            return true;
          break;
        case L_NORD_OUEST_SUD:
            return true;
          break;
        case L_NORD_SUD:
            return true;
          break;
        case L_NORD_EST_OUEST_SUD:
            return true;
          break;
        default:
          return false;
          break;
      }
      break;
    case OUEST:
    switch(etatCase)
      {
        case L_EST:
          return true;
          break;
        case L_NORD_EST:
          return true;
          break;
        case L_NORD_EST_OUEST:
          return true;
          break;
        case L_NORD_EST_SUD:
          return true;
          break;
        case L_EST_OUEST:
          return true;
          break;
        case L_EST_OUEST_SUD:
          return true;
          break;
        case L_EST_SUD:
          return true;
          break;
        case L_NORD_EST_OUEST_SUD:
          return true;
          break;
        default:
          return false;
          break;
      }
      break;
    case EST:
    switch(etatCase)
      {
        case L_OUEST:
          return true;
          break;
        case L_OUEST_SUD:
          return true;
          break;
        case L_NORD_EST_OUEST:
          return true;
          break;
        case L_EST_OUEST:
          return true;
          break;
        case L_NORD_OUEST:
          return true;
          break;
        case L_NORD_OUEST_SUD:
          return true;
          break;
        case L_EST_OUEST_SUD:
          return true;
          break;
        case L_NORD_EST_OUEST_SUD:
          return true;
          break;
        default:
          return false;
          break;
      }
      break;
    case SUD:
    switch(etatCase)
      {
        case L_SUD:
          return true;
          break;
        case L_EST_SUD:
          return true;
          break;
        case L_NORD_SUD:
          return true;
          break;
        case L_NORD_EST_SUD:
          return true;
          break;
        case L_OUEST_SUD:
          return true;
          break;
        case L_NORD_OUEST_SUD:
          return true;
          break;
        case L_EST_OUEST_SUD:
          return true;
          break;
        case L_NORD_EST_OUEST_SUD:
          return true;
          break;
        default:
          return false;
          break;
      }
      break;
    default:
      break;
  }

}

void verification_obstacle()
{
  //sparx.orientation
  //sparx.position
  
  //Serial.println("PID");
  //NORD
  if(sparx.orientation == NORD)
  {
    
    
    if((verification_matrice(sparx.position[0], sparx.position[1], sparx.orientation) || (capteur_infrarouge()) /*(matrice_parcour[(sparx.position[0])+1][sparx.position[1]] == 15)*/)/*(1||5||6||7||11||12||13||15)*/)
    {
      //Serial.println("droite");
      tourneDroit();
      sparx.orientation = EST;
    }
    else
    {
      matrice_parcour[sparx.position[0]][sparx.position[1]] = 15;
      sparx.position[0]+= 1;
      travel(500);
    }
    
  }
  //EST
  else if(sparx.orientation == EST)
  {
    if((verification_matrice(sparx.position[0], sparx.position[1], sparx.orientation)|| (capteur_infrarouge()) || (matrice_parcour[sparx.position[0]][sparx.position[1]+1] == 15)))
    {
      tourneGauche();
      tourneGauche();
      sparx.orientation = OUEST;
    }
    else
    {
      travel(500);
      matrice_parcour[sparx.position[0]][sparx.position[1]] = 15;
      tourneGauche();
      sparx.orientation = NORD;
      sparx.position[1]++;
    }
  }
  //OUEST
  else if(sparx.orientation == OUEST)
  {
    if((verification_matrice(sparx.position[0], sparx.position[1], sparx.orientation) || (capteur_infrarouge()) || (matrice_parcour[sparx.position[0]][sparx.position[1]-1] == 15)))
    {
      tourneGauche();
      sparx.orientation = SUD;
    }
    else
    {
      travel(500);
      matrice_parcour[sparx.position[0]][sparx.position[1]] = 15;
      tourneDroit();
      sparx.orientation = NORD;
      sparx.position[1]--;
    }
  }
  //SUD
  else if(sparx.orientation == SUD)
  {
    if(verification_matrice(sparx.position[0], sparx.position[1], sparx.orientation) || (capteur_infrarouge()) || (matrice_parcour[sparx.position[0]-1][sparx.position[1]] == 15))
    {
      tourneDroit();
      sparx.orientation = EST;
    }
    else
    {
      travel(500);
      matrice_parcour[sparx.position[0]][sparx.position[1]] = 15;
      sparx.position[0]--;
    }
  }

}
bool start() {
  bool go = false;
  int frequency = analogRead(analog1);
  int ambiant = analogRead(analog2);
  if (frequency > ambiant) go=true;
  return go;
}

void getangle(float angle){
  float coefficient = 360.0 /angle ;
  float circonference = (22.0*PI);
  float distance = (circonference/coefficient);
  float nbtours = (distance/23.93);
  float nbpulses = (nbtours*3200);
  sparx.moteurs.encodeurDroite = ENCODER_Read(1);
  if (abs(sparx.moteurs.encodeurDroite) > (nbpulses-10))
  {
    //Serial.println("turn");
    rightangle = true;
  }
};

void tourneDroit()
{
  
  while(!rightangle)
  {
    PID();
    actionDroit();
    getangle(79.3); //29-A = 79.3
  }
  ENCODER_ReadReset(0);
  ENCODER_ReadReset(1);
  rightangle = false;
  arret();
}

void tourneGauche()
{
  
  while(!rightangle)
  {
    PID();
    actionGauche();
    getangle(77.2); //29-A = 77.2
  }
  ENCODER_ReadReset(0);
  ENCODER_ReadReset(1);
  rightangle = false;
  arret();
}