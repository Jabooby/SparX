/*
Projet: Le nom du script
Equipe: Votre numero d'equipe
Auteurs: Les membres auteurs du script
Description: Breve description du script
Date: Derniere date de modification
*/
/*
Inclure les librairies de functions que vous voulez utiliser
*/

#include <Arduino.h>
#include <LibRobus.h>

/*
Variables globales et defines
 -> defines...
 -> L'ensemble des fonctions y ont acces
*/

#define pi        3.14159
#define diametre  7.62 //les roues ont 2 pouces de diametre
#define radius    diametre / 2

bool bumperArr;
int vertpin = 48;
int rougepin = 49;
bool vert = false;
bool rouge = false;
int etat = 0; // = 0 arrêt 1 = avance 2 = recule 3 = TourneDroit 4 = TourneGauche
int etatPast = 0;
float vitesse = 0.30;

// MES FONCTIONS
// ----------------------------------------------------------------------------------------------

int positionsLignes[20][2] =
{{2,0},{2,1},{2,1},{3,1},
{1,3},{2,3},{2,3},{3,3},
{1,5},{2,5},{2,5},{3,5},
{1,7},{2,7},{2,7},{3,7},
{1,9},{2,9},{2,9},{3,9}};

int left = 0, right = 0;
int x1 = 2, y1 = 0, x2 = 2, y2 = 1;
int currentOrientation  = 1; // variable globale de l'orientation
bool goLigne;
bool goMur;
bool stop = false;
bool rightangle;

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
 
int timer;
int reset = 0;
int interval = 500;
 
void arret(){
  MOTOR_SetSpeed(RIGHT, 0);
  MOTOR_SetSpeed(LEFT, 0);
};
 
void avance(){
  MOTOR_SetSpeed(RIGHT,vitesse);
  MOTOR_SetSpeed(LEFT, vitesse);
};
 
void recule(){
  MOTOR_SetSpeed(RIGHT, -0.5*vitesse);
  MOTOR_SetSpeed(LEFT, -vitesse);
};
 
void tourneDroit(){
  MOTOR_SetSpeed(RIGHT, 0.5*vitesse);
  MOTOR_SetSpeed(LEFT, -0.5*vitesse);
};
 
void tourneGauche(){
  MOTOR_SetSpeed(RIGHT, -0.5*vitesse);
  MOTOR_SetSpeed(LEFT, 0.5*vitesse);
};
void Forward(int distance){
timer = millis();

if ((timer-reset) > interval){
  PID();
  float moyenne = (abs(left) + abs(right))/2;
  float circonference = 2*PI*38.1;
  float goalVal = (3200.0f*distance)/circonference;
 
  if((moyenne == goalVal) or ((moyenne >= goalVal-100) and (moyenne <= goalVal+100))) stop = true;
   
   
    if(!stop){
      //tourneDroit();
      avance();
    }
    else arret();
   //}
   
   
    reset = timer;
  }
  else if ((timer-reset) <= interval){
    if(!stop){
      avance();
      //tourneDroit();
    }
   // else{
   //   arret();
   //};
   
    //status = false;
  }
}
 
void RightTurn(float distance){
timer = millis();
//getangle();
if ((timer-reset) > interval){
    PID();
    float circonference = 2*PI*38.1;
    float goalVal = (3200.0f*distance)/circonference;
    if((right == goalVal) or ((right >= goalVal-100) and (right <= goalVal+100))) rightangle = true;
    if(!rightangle){
      tourneGauche();
    }
    //else{
    //  arret();
   //}

    reset = timer;
  }
  else if ((timer-reset) <= interval){
 
    if(!rightangle){
      tourneGauche();
    }
   // else{
   //   arret();
   //};
  }
}
void LeftTurn(float distance){
timer = millis();
//getangle();
if ((timer-reset) > interval){
    PID();
    float circonference = 2*PI*38.1;
    float goalVal = (3200.0f*distance)/circonference;
    if((right == goalVal) or ((right >= goalVal-100) and (right <= goalVal+100))) rightangle = true;
    if(!rightangle){
      tourneDroit();
    }
    //else{
    //  arret();
    //}
   
   
    reset = timer;
  }
  else if ((timer-reset) <= interval){
    if(!rightangle){
      tourneDroit();
    }
   // else{
   //   arret();
   //};
  }
}
 
void ChangerOrientation(int orientation)
{
  int localC, localO;
  if (currentOrientation == 1)
    localC = 2;
  else
    localC = 2 * currentOrientation - 3;
  if (orientation == 1)
    localO = 2;
  else
    localO = 2 * orientation - 3;
  int deltaPos = localO - localC;
  if (deltaPos < 0)
    LeftTurn(abs(deltaPos) * 120); // tourne à gauche : abs(deltaPos) * 90 degrés
  if (deltaPos > 0)
    RightTurn(abs(deltaPos) * 120); // tourne à droite : abs(deltaPos) * 90 degrés
 
  currentOrientation = orientation;
}
 

bool seeWall; 
void CheckMur()
{
  seeWall = false;
  vert = digitalRead(vertpin);
  rouge = digitalRead(rougepin);
 
  if (vert && rouge)
    { // obstacle => avance pas
      goMur = false;
      seeWall = true;
      //Serial.println("There's a wall");
    }
  else {// aucun obstacle==> avance
      goMur = true;
      seeWall = false;
      //Serial.println("There isn't a wall, go ahead");
    }
 }


bool seeLine;
void CheckLigne()
{
  seeLine = false;
  //Serial.println("Checking for a line");
  bool ligne = false;
  goLigne = false;
  while (goLigne == false)
  {
    for (int i = 0; i < 20; i++)
    {
      if (positionsLignes[i][0] == x1 && positionsLignes[i][1] == y1)
      {
        //Serial.println("Your position is in the line list");
        if (i%2 == 0)
        {
          //Serial.println("Seems like we gotta check before");
          if (positionsLignes[i+1][0] == x2 && positionsLignes[i+1][1] == y2)
          {
            //Serial.println("There's a line where you want to go ! Changing path");
            seeLine = true;
            ChangerOrientation(currentOrientation + 1);
            x2 = 2 * currentOrientation - 3;
            y2 = y1;
            ligne = false;
            break;
          }
          else
            ligne = true;
        }
        else
        {
          //Serial.println("Seems like we gotta check after");
          if (positionsLignes[i-1][0] == x2 && positionsLignes[i-1][1] == y2)
          {
            //Serial.println("There's a line where you want to go ! Changing path");
            seeLine = true;
            ChangerOrientation(currentOrientation + 1);
            x2 = 2 * currentOrientation - 3;
            y2 = y1;
            ligne = false;
            break;
          }
          else
            ligne = true;
        }
      }
      else
      {
        ligne = true;
      }
    }
    /*
    if (ligne)
    {
      CheckMur();
      if (!seeWall)
      {
        goLigne = true;
      }
      else
      {
        ChangerOrientation(currentOrientation + 1);
        x2 = 2 * currentOrientation - 3;
        y2 = y1;
      }
    }  
    else
    {
      goLigne = false;
    }
    */
    goLigne = ligne;
  }
}

// ---------------------------------------------------------------------------------------------
//MES FONCTIONS
 
void beep(int count){
  for(int i=0;i<count;i++){
    AX_BuzzerON();
    delay(100);
    AX_BuzzerOFF();
    delay(100);  
  }
  delay(400);
}
/*
Fonctions d'initialisation (setup)
 -> Se fait appeler au debut du programme
 -> Se fait appeler seulement un fois
 -> Generalement on y initilise les varibbles globales
*/
void setup(){
  currentOrientation = 1;
  BoardInit();
  x1 = 2, y1 = 0, x2 = 2, y2 = 1;
  Serial.begin(9600);
  //initialisation
  pinMode(vertpin, INPUT);
  pinMode(rougepin, INPUT);
  delay(100);
  beep(3);
}
 
/*
Fonctions de boucle infini
 -> Se fait appeler perpetuellement suite au "setup"
*/
void loop()
{
  /*
  //Serial.println("Let's check your path");
  delay(500);
  
  ChangerOrientation(1);
  
  //Serial.println("I'm turning to position 1");
  delay(500);
  while(!goLigne && !goMur)
  {
    delay(500);
    CheckLigne();
    //Serial.println("CheckLigne is done :)");
    delay(500);
    if (x2 == 0)
    {
      ChangerOrientation(currentOrientation + 1);
      x2 = 2 * currentOrientation - 3;
      y2 = y1;
    }
  }
  
  delay(500);
  //Serial.println("Moving Forward");
  Forward(500); // Bouge
  delay(500);
  x2 = x1;
  y2 = y1 + 1;
  */
 beep(1);
 LeftTurn(8000);
 beep(2);
 Forward(500);
 beep(3);
 RightTurn(8000);
 beep(4);
}