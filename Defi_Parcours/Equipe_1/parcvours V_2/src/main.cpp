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
#define circonference_cm diametre * PI
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
  float ki = 0.0000001;
  float kd = 0.000032;
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
bool go = false;
int reset = 0;
int interval = 50; //50 mS
int ambiantpin = A2;
int intensitypin = A3;
bool rightangle = false;

 int matrice_parcour[11][3]{
    {9,7,10},//1
    {8,15,8},//2
    {2,7,3},//3
    {8,15,8},//4
    {2,4,3},//5
    {8,8,8},//6
    {2,1,3},//7
    {8,15,8},//8
    {2,7,3},//9
    {11,15,11}, //10
    {15,15,15} //11
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
bool getangle(float angle);
void verification_obstacle();
float getDistance();
void travel(float distance);
void tourneGauche();
bool verification_matrice(int y, int x, int direction);
void tourne180();
void avanceFin();
  
bool start() {
  //bool go = false;
  int frequency = analogRead(intensitypin);
  int ambiant = analogRead(ambiantpin);
  

  if (frequency > 800){
    go=true;
  } 
  else go = false;
  return go;
}
void setup() {
  // put your setup code here, to run once:
  BoardInit();
  sparx.pid.errsum = 0.0;
  pinMode(LED_BUILTIN,OUTPUT);//détecteur de proximité
  pinMode(sparx.vertpin, INPUT);
  pinMode(sparx.rougepin, INPUT);
  pinMode(intensitypin, INPUT_PULLUP);
  pinMode(ambiantpin, INPUT_PULLUP);
  delay(100);
}

void loop() {
  //mettre code sifflet
  //verif +bouge
  //tourneGauche();
  
  //travel(500);
  //avance();
  start();
  if (start()){

    while(sparx.position[0] < 9)
    {
      verification_obstacle();
    }
    avance();
    if(sparx.position[1] == 2)
    {
      tourneGauche();
      avance();
      tourneGauche();
      avance();
    }
    else
    {
      tourneDroit();
      avance();
      tourneDroit();
      avance();
    }
    for(int i = 0; i < 11; i++)
    {
      avanceFin();
    }
    while(1);
  }
  //Serial.println("Position fin de code: ");
  //Serial.println(sparx.position[0]);
  else;
  
 
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

void avanceFin(){
  sparx.moteurs.vitesse_moteur_gauche = 0.45;
  sparx.moteurs.vitesse_moteur_droite = 0.45;
  sparx.moteurs.vitesse = 0.6;
  PID();
  //float distance_init = getDistance();
  while(getDistance() < 50.5f)
  {
    //Serial.println(getDistance());
    PID();
    MOTOR_SetSpeed(RIGHT,sparx.moteurs.vitesse_moteur_droite);
    MOTOR_SetSpeed(LEFT, sparx.moteurs.vitesse_moteur_gauche);
    
  }
  arret();
};
void avance(){
  sparx.moteurs.vitesse_moteur_gauche = 0.3;
  sparx.moteurs.vitesse_moteur_droite = 0.3;
  sparx.moteurs.vitesse = 0.3;
  PID();
  //float distance_init = getDistance();
  while(getDistance() < 50.5f)
  {
    if(getDistance() > 10.5f)
    {
      sparx.moteurs.vitesse_moteur_gauche = 0.4;
      sparx.moteurs.vitesse_moteur_droite = 0.4;
      sparx.moteurs.vitesse = 0.4;
    }
    if(getDistance() > 37.5f)
    {
      sparx.moteurs.vitesse_moteur_gauche = 0.3;
      sparx.moteurs.vitesse_moteur_droite = 0.3;
      sparx.moteurs.vitesse = 0.3;
    }
    if(getDistance() > 42.5f)
    {
      sparx.moteurs.vitesse_moteur_gauche = 0.15;
      sparx.moteurs.vitesse_moteur_droite = 0.15;
      sparx.moteurs.vitesse = 0.15;
    }
    //Serial.println(getDistance());
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

void actionDroite(){
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
    //PID();
    sparx.moteurs.encodeurGauche = ENCODER_Read(0);
    sparx.moteurs.encodeurDroite = ENCODER_Read(1);
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

float getDistance(){
  sparx.moteurs.encodeurGauche = ENCODER_Read(LEFT);
  //Serial.println(sparx.moteurs.encodeurGauche);
  float distancel = (sparx.moteurs.encodeurGauche/3200.0f) * circonference_cm;
  return distancel;

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
    
    
    if((verification_matrice(sparx.position[0], sparx.position[1], sparx.orientation) || (capteur_infrarouge()) || (matrice_parcour[sparx.position[0]+1][sparx.position[1]] == 15))/*(1||5||6||7||11||12||13||15)*/)
    {
      //Serial.println("droite");
      tourneDroit();
      sparx.orientation = EST;
    }
    else
    {
      avance();
      //travel(500);
      matrice_parcour[sparx.position[0]][sparx.position[1]] = 15;
      sparx.position[0]++;
      
    }
    
  }
  //EST
  else if(sparx.orientation == EST)
  {
    if((verification_matrice(sparx.position[0], sparx.position[1], sparx.orientation)|| (capteur_infrarouge()) || (matrice_parcour[sparx.position[0]][sparx.position[1]+1] == 15)))
    {
      tourne180();
      sparx.orientation = OUEST;
    }
    else
    {
      //travel(500);
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
    if((verification_matrice(sparx.position[0], sparx.position[1], sparx.orientation) || (capteur_infrarouge()) || (matrice_parcour[sparx.position[0]][sparx.position[1]-1] == 15)))
    {
      tourneGauche();
      sparx.orientation = SUD;
    }
    else
    {
      //travel(500);
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
    if(verification_matrice(sparx.position[0], sparx.position[1], sparx.orientation) || (capteur_infrarouge()) || (matrice_parcour[sparx.position[0]-1][sparx.position[1]] == 15))
    {
      tourneGauche();
      sparx.orientation = EST;
    }
    else
    {
      //travel(500);
      avance();
      matrice_parcour[sparx.position[0]][sparx.position[1]] = 15;
      sparx.position[0]--;
    }
  }

}
/*
bool start() {
  bool go = false;
  int frequency = analogRead(analog1);
  int ambiant = analogRead(analog2);
  if (frequency > ambiant) go=true;
  return go;
}
*/
void tourneDroit()
{
  sparx.moteurs.vitesse_moteur_gauche = 0.3;
  sparx.moteurs.vitesse_moteur_droite = 0.3;
  sparx.moteurs.vitesse = 0.3;
  while(!rightangle)
  {
    PID();
    actionDroit();
    getangle(75.5); //29-A = 76.0 29-B =75.5
  }
  ENCODER_ReadReset(0);
  ENCODER_ReadReset(1);
  rightangle = false;
  arret();
}

void tourneGauche()
{
  sparx.moteurs.vitesse_moteur_gauche = 0.3;
  sparx.moteurs.vitesse_moteur_droite = 0.3;
  sparx.moteurs.vitesse = 0.3;
  
  while(!rightangle)
  {
    PID();
    actionGauche();
    getangle(74.65); //29-A = 75.2 29-B =74.4
  }
  ENCODER_ReadReset(0);
  ENCODER_ReadReset(1);
  rightangle = false;
  arret();
}

void tourne180_gauche()
{
  sparx.moteurs.vitesse_moteur_gauche = 0.3;
  sparx.moteurs.vitesse_moteur_droite = 0.3;
  sparx.moteurs.vitesse = 0.3;
  
  while(!rightangle)
  {
    PID();
    actionGauche();
    getangle(149.4); 
  }
  ENCODER_ReadReset(0);
  ENCODER_ReadReset(1);
  rightangle = false;
  arret();
}

void tourne180_droite()
{
  sparx.moteurs.vitesse_moteur_gauche = 0.3;
  sparx.moteurs.vitesse_moteur_droite = 0.3;
  sparx.moteurs.vitesse = 0.3;
  
  while(!rightangle)
  {
    PID();
    actionGauche();
    getangle(149.4); 
  }
  ENCODER_ReadReset(0);
  ENCODER_ReadReset(1);
  rightangle = false;
  arret();
}


/*
 * @brief Fonction qui permet de tourner le robot en mode pivot. (voir: https://docs.idew.org/code-robotics/references/robot-behaviors/turning)
 * @param angle: si l'angle est positif on tourne à droite , sinon à gauche. Angle définie l'angle que le robot doit tourner;
 * @return void
 */
void tourner(float angle)
{
  //si angle plus grand que 0
  if (angle > 0.0)
  {
    //tand que on est pas arrivé à l'angle voulue
    while(!(getangle(angle)))
    {
      PID(); //PID pour savoir si les 2 moteurs ont un vitesse angulaire égal
      actionDroite(); //tourne à droite
    }
    ENCODER_ReadReset(LEFT); //reset encodeur du moteur gauch
    ENCODER_ReadReset(RIGHT); //reset encodeur du moteur droit
    rightangle = false; //remet la valeur global à false
    arret(); //force le robot à faire un stop 
  }
  //si l'angle ets plus petit que 0 (négatif)
  else
  {
    //tand que on est pas arrivé à l'angle voulue
    while(!(getangle(angle)))
    {
      PID(); //PID pour savoir si les 2 moteurs ont un vitesse angulaire égal
      actionGauche(); //tourne à gauche
    }
    ENCODER_ReadReset(LEFT); //reset encodeur du moteur gauche
    ENCODER_ReadReset(RIGHT); //reset encodeur du moteur droit
    rightangle = false; //remet la valeur global à false
    arret(); //force le robot à faire un stop 
  }
}

/*
 * @brief Fonction permet de vérifier si le robot est arrrivé à l'angle voulue
 * @param angle: l'angle que l'on désire savoir si notre robot l'a atteint;
 * @return bool: retourne true si l'angle est atteint, flase si non
 */
bool getangle(float angle){
  float coefficient = 360.0 / angle ; //coefficient utiliser dans le calcul, 360/90, on doit divisr par 4
  float circonference = (22.0*PI); //calcul de la circonférence de rotation (à changer/calculer (voir: https://en.wikipedia.org/wiki/Differential_wheeled_robot ))
  float distance = (circonference/coefficient); //calcul la distance qu'une roue doit parcourir
  //!!!!pourrait utiliser getDistance ici!!!!!
  float nbtours = (distance/23.93); //convertie la distance en nombre de tour de roue
  float nbpulses = (nbtours*3200); // convertie le nombre de tour en nombre de pulse
  sparx.moteurs.encodeurDroite = ENCODER_Read(1); //lecture de l'encodeur droit
  //si on a atteint la valeur d'encodeur 
  if (abs(sparx.moteurs.encodeurDroite) > (nbpulses) ) 
  {
    //retourne true
    return true;
  }
  else
  //sinon false
    return false;
};
