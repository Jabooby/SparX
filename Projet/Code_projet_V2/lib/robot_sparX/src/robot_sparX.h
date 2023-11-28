#ifndef robot_sparX_H_
#define robot_sparX_H_
/**
 * @file   robot_sparX.h
 * @version V1_0
 * @author P-29 SparX
 * @date  début 28 septembre 2023 - 14 octobre 2023
 * @brief    
 * Librairie des fonctions utilisées lors du défi du parcours. S'il vous plaît voir les commentaire plus bas pour comprendre leurs 
 * fonctionnement.
 */


/************************* INCLUDES *************************/
#include <Arduino.h>
#include <LibRobus.h>
/************************* CONSTANTES *************************/
#define diametre_roue_cm  7.62 //les roues ont 3 pouces de diametre
#define circonference_cm diametre_roue_cm * PI
#define circonference_mm  76.2 * PI
#define radius_cm    diametre_roue_cm / 2
#define radius_mm    diametre_roue_cm / 2
#define pin_rouge           49 //infrarouge
#define pin_vert            48 //infrarouge
#define pulse_tour          3200.0f
#define TIMER_TIME          24 //en ms 
#define COS_45_RAD          cos(45*DEG_TO_RAD)
#define distance_entre_roues_m 0.19
/************************* DECLARATION DES ÉTATS MACHINE *************************/

/************************* DÉCLARATION DE STRUCTURE *************************/
/*
Sructure PID: structure qui aide à organiser les valeurs PID. Elle sera utilisée dans la structure robot.
*/
struct PID {
  //constantes PID
  float kp = 0.00081; //Constante proportionnel
  float ki = 0.00025; //Constante d'intégrale (shake mcguy) 0000001
  float kd = 0.00005; //Constante de dérivée 0.000032
  //variables de calcul PID
  float errsum  = 0.0; //somme d'erreur
  float prevErr = 0.0; //erreur avant
  float errdiff = 0.0; //différence d'erreur
};
/*
Sructure moteurs: structure qui aide à organiser les moteurs. Elle sera utilisée dans la structure robot.
*/
struct moteurs {
  float vitesse_moteur_gauche = 0.0; //vitesse du moteur gauche
  float vitesse_moteur_droite = 0.0; //vitesse du moteur froit
  float vitesse_moment = 0.0; //vitesse des deux moteurs
  float vitesse_voulue = 0.3;
  int encodeurGauche = 0; //variable qui store la valeur lue sur l'encodeur du moteur gauche
  int encodeurDroite = 0; //variable qui store la valeur lue sur l'encodeur du droit gauche
  
};
/*
Sructure capteurs: structure qui aide à organiser les différents capteurs. Elle sera utilisée dans la structure robot.
*/
struct capteurs {
    int infrarougePin[2] = {pin_vert,pin_rouge}; //position 0 == pinVert // position 1 == pinRouge
};
struct robot {
  struct PID pid;
  struct moteurs moteurs;
  struct capteurs capteurs;
  float orientation = 90.0; //store l'orientation du robot, à changer
  //char mouvement = ARRET; //store le mouvement voulue selon l'algorithme, à incorporer dans le code éventuellement
  int position[2] = {0,1}; //position Y et X du robot, à changer
  unsigned long startTimer = millis();
  bool timerRunning = true;
  uint8_t etat = 0;
};

/************************* VARIABLES GLOBALES *************************/
extern struct robot sparx; //création de la valeur global SparX. SparX est le robot et nous pouvons accéder
// les différentes fonctions du robot. Pour accéder le moteur gauche juste faire "Sparx.vitesse_moteur_gauche"
const uint8_t IR_PIN[4] =  {A0, A1, A2, A3};

/************************* FONCTIONS *************************/

////////////////////////// FOCNTIONS MOUVEMENTS //////////////////////////
/*
 * @brief Fonction qui permet de tourner le robot en mode pivot. (voir: https://docs.idew.org/code-robotics/references/robot-behaviors/turning)
 * @param angle: si l'angle est positif on tourne à droite , sinon à gauche. Angle définie l'angle que le robot doit tourner;
 * @return void
 */
//void tourner(float angle);
/*
 * @brief Fonction permet de changer la vitesse des moteurs selon la méthode PID (mettre lien ici plus tard avec explication).
 * La fonction change juste la vitesse du moteur gauche, la vitesse du moteur droit reste stable. Autrement dit, 
 * le moteur gauche est Follower (dépendant du Leader) et le moteur droit est Leader (influence le follower).
 * 
 * @param rien
 * @return rien
 */
void PID(float vGauche, float vDroite);
/*
 * @brief Fonction dit au moteur de tourner à droite.
 * @param rien
 * @return rien
 */
void actionDroite();
/*
 * @brief Fonction dit au moteur de tourner à gauche.
 * @param rien
 * @return rien
 */
void actionGauche();
/*
 * @brief Fonction dit au moteur d'avancer en ligne droite selon une distance donnée en cm.
 * @param ditance: distance en cm que nous voulons avancer en ligne droite.
 * @return rien
 */
void avance(float distance);
/*
 * @brief Fonction dit au moteur d'arrêter, on fait un reset des deux encodeurs ici aussi.
 * @param rien
 * @return rien
 */
void arret();

////////////////////////// FOCNTIONS CAPTEURS //////////////////////////
/*
 * @brief Fonction permet de voir l'état du capteur infrarouge 
 * @param rien
 * @return bool: retourne true si il a un obstacle, flase si il en a pas.
 */
bool lectureInfrarouge();
/*
 * @brief Fonction permet de vérifier si le robot est arrrivé à l'angle voulue
 * @param angle: l'angle que l'on désire savoir si notre robot l'a atteint;
 * @return bool: retourne true si l'angle est atteint, flase si non
 */
//bool getAngle(float angle);
/*
 * @brief Fonction permet de retourner la distance lu par l'encodeur de la roue gauche
 * @param rien
 * @return float: retourne la distance parcourue par le roue gauche en cm
 */
float getDistance();
#endif