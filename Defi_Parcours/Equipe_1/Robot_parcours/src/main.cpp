/**
 * @file   main.cpp
 * @author P-29 SparX
 * @date  début 28 septembre 2023 - fin ???
 * @brief    
 * 
 */


/************************* INCLUDES *************************/
#include <Arduino.h>
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
  L_NORD        = 0, //ligne au Nord du centre
  L_EST         = 1, //ligne à l'Est du centre
  L_OUEST       = 2, //ligne à l'Ouest du centre
  L_SUD         = 3, //ligne au Sud du centre
  L_NORD_EST    = 4, //ligne Nord et Est
  L_NORD_OUEST  = 5, //ligne Nord et Ouest
  L_NORD_SUD    = 6, //ligne Nord et Sud
  L_EST_OUEST   = 7, //ligne Nord et Ouest
  L_EST_SUD     = 8, //ligne Nord et Sud
  L_OUEST_SUD   = 9, //ligne Nord et Ouest
  L_NORD_EST_OUEST=10, //ligne Nord, Est et Ouest
  L_NORD_EST_SUD=11, //ligne Nord, Est et Sud
  L_NORD_OUEST_SUD=12,//ligne Nord, Ouest et Sud
  L_EST_OUEST_SUD=13, //ligne Est, Ouest et Sud
  L_NORD_EST_OUEST_SUD=14,//ligne Nord, Est, Ouest et Sud
} ligne; //dicte l'emplacement des lignes par rapport au centre d'une case

/************************* DÉCLARATION DE STRUCTURE *************************/
struct robot {
  char orientation = NORD;
  char mouvement = ARRET;
  float vitesse_moteur_gauche = 0.0;
  float vitesse_moteur_droite = 0.0;
  bool detection = false;
  int position[2] = {0,0}; //position X et Y
  bool retour = false;
  bool depart = false;
};
/************************* VARIABLES GLOBALES *************************/
struct robot SparX; //création de la valeur global SparX. SparX est le robot et nous pouvons accéder
// les différentes fonctions du robot. Pour accéder le moteur gauche juste faire "Sparx.vitesse_moteur_gauche"
/************************* DECLARATIONS DE FONCTIONS *************************/
int myFunction(int, int); //ceci est un exemple

void setup() {
  // put your setup code here, to run once:
  int result = myFunction(2, 3);
}

void loop() {
  // put your main code here, to run repeatedly:
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