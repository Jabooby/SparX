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
  int position[2] = {0,0}; //position X et Y
  bool retour = false;
  bool depart = false;
  int vertpin = 53;
  int rougepin = 49;
  int position[2] = {0,1};
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

 int matrice_parcour[10][3]{
    {0,L_NORD,0},//1
    {0,0,0},//2
    {0,0,0},//3
    {0,0,0},//4
    {0,0,0},//5
    {0,0,0},//6
    {0,0,0},//7
    {0,0,0},//8
    {0,0,0},//9
    {0,0,0} //10
  };
/************************* DECLARATIONS DE FONCTIONS *************************/
int myFunction(int, int); //ceci est un exemple
void arret();
void avance();
void recule();
void tourneDroit();
void actionDroit();
void tourneGauche();
void PID();
bool capteur_infrarouge();
bool start();
void getangle();


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
  while(sparx.position[1] < 10)
  {
    verification_obstacle();
  }
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
};

void avance(){
  PID();
  int distance_init = getDistance();
  while(getDistance()- distance_init < 50)
  {
    PID();
    MOTOR_SetSpeed(RIGHT,sparx.moteurs.vitesse_moteur_droite);
    MOTOR_SetSpeed(LEFT, sparx.moteurs.vitesse_moteur_gauche);
  }
  arret();
};

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
  MOTOR_SetSpeed(RIGHT,-0.5*sparx.moteurs.vitesse_moteur_droite);
  MOTOR_SetSpeed(LEFT, 0.5*sparx.moteurs.vitesse_moteur_gauche);
};

/*void tourneGauche(){
  MOTOR_SetSpeed(RIGHT, -0.5*sparx.vitesse_moteur_droite);
  MOTOR_SetSpeed(LEFT, 0.5*sparx.vitesse_moteur_gauche);
};*/

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
      sparx.moteurs.vitesse = sparx.moteurs.vitesse;
      sparx.moteurs.vitesse_moteur_gauche = sparx.moteurs.vitesse;
      sparx.moteurs.vitesse_moteur_droite = sparx.moteurs.vitesse;
    }
    
}

void travel(int distance){
  PID();
  float moyenne = (abs(sparx.moteurs.encodeurGauche) + abs(sparx.moteurs.encodeurDroite))/2;
  float circonference = 2*PI*38.1;
  float goalVal = (3200.0f*500.0f)/circonference;
  if((moyenne == goalVal) or ((moyenne >= goalVal-100) and (moyenne <= goalVal+100))) sparx.mouvement = ARRET;
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
  float left = ENCODER_Read(0);
  float right = ENCODER_Read(1);
  float circumference = diametre * PI;
  float distancel = (left/3200) * circumference;
  float distancer = (right/3200) * circumference;
  float distance = (distancel+distancer)/2;
  //Serial.print("distance: ");
  //Serial.println(distance);
  return distance;

}

void verification_obstacle()
{
  //sparx.orientation
  //sparx.position
  PID();
  //NORD
  if(sparx.orientation == NORD)
  {
    if((matrice_parcour[sparx.position[0]][sparx.position[1]] == L_NORD) || (!capteur_infrarouge())/*(1||5||6||7||11||12||13||15)*/)
    {
      tourneDroit();
      sparx.orientation = EST;
    }
    else
    {
      avance();
      matrice_parcour[sparx.position[0]][sparx.position[1]] = 15;
      sparx.position[0]++;
    }
  }
  //EST
  else if(sparx.orientation == EST)
  {
    if((matrice_parcour[sparx.position[0]][sparx.position[1]] == L_EST) || (!capteur_infrarouge()))
    {
      tourneDroit();
      tourneDroit();
      sparx.orientation = OUEST;
    }
    else
    {
      avance();
      matrice_parcour[sparx.position[0]][sparx.position[1]] = 15;
      tourneGauche();
      sparx.orientation = NORD;
      sparx.position[1]++;
    }
  }
  //OUEST
  else if(sparx.orientation == OUEST)
  {
    if((matrice_parcour[sparx.position[0]][sparx.position[1]] == L_OUEST) || (!capteur_infrarouge()))
    {
      tourneGauche();
      sparx.orientation = SUD;
    }
    else
    {
      avance();
      matrice_parcour[sparx.position[0]][sparx.position[1]] = 15;
      tourneDroit();
      sparx.orientation = NORD;
      sparx.position[1]--;
    }
  }
  //SUD
  else if(sparx.orientation == SUD)
  {
    if(matrice_parcour[sparx.position[0]][sparx.position[1]] == L_SUD)
    {
      tourneDroit();
      sparx.orientation = EST;
    }
    else
    {
      avance();
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

void getangle(){

float left1 = ENCODER_Read(0);
float right2 = ENCODER_Read(1);

/*Serial.print("left: ");
    Serial.println(left1);
    Serial.print("right: ");
    Serial.println(right2);
*/
float circonference = (18.5*PI);

float distance = (circonference/4.0);
float nbtours = (distance/23.93);
float nbpulses = (nbtours*3200);
/*Serial.print("nbpulses: ");
Serial.println(nbpulses);
*/
if ((right2 <= (nbpulses+50)) and (right2 >= (nbpulses-50)))
  {
   //Serial.println("turn");
   rightangle = true;
   
  }

};

void tourneDroit()
{
  PID();
  getangle();
  while(!rightangle)
  {
    actionDroit();
    getangle();
  }
  ENCODER_ReadReset(0);
  ENCODER_ReadReset(1);
  rightangle = false;
  arret();
}