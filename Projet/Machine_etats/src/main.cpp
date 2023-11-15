/**
 * @file   Machine_etats.cpp
 * @version V1_0
 * @author P-29 SparX
 * @date  début 14 novembrebre 2023 - ...
 * @brief    
 * Code exemplaire pour les états machine du robot
 */


/************************* INCLUDES *************************/
#include <Arduino.h>
#include <robot_sparX.h>
/************************* DÉCLARATIONS DE VARIABLES *************************/
/*
* Voici le enum de la machine état du robot.
* Une variable "enum" est une variable définie par le programmeur. Elles sont des 
* int avec un nom. Ainsi, STOP est égal à 0, AVANCE à 1 et ainsi de suite.
* C'est moins mélangeant que juste avoir des chiffres partout. 
*/
enum Etat_enum 
{
  STOP = 0, //Arrêt du robot, vitesse du robot = 0
  AVANCE, //Henri
  TOURNE_DROITE, //Robot tourne à droite, Vitesse roue gauche > Vitesse roue droite
  TOURNE_GAUCHE, //Édouard
  RECULE, //Antoine
  TOURNE_180, //À voir, peut-être très utile mais pas nécessaire. À confirmer
  LIFT_UP, //Lève la plante
  MAINTIENT_POSITION,  // Maitient la hauteur de la plante
  LIFT_DOWN, //Édouard
  MANUEL, //Henri
  RECHERCHE_LUMIERE //Antoine
};

/*
* Voici le enum pour les capteurs.
* En gros, on pourra demander à un fonction style "Gestion_capteur()" de retourner
* quels capteurs sont présentement "actif". Example si le capteur de distance IR de droite
* voit un mur à 10cm du robot, la fonction pourra retourner "SENSOR_IR_DR".
* Ça va être facile par la suite décire le restant du code pour gérer le robot selon ce qu'il "voit".
*/
enum Sensors_enum 
{
  AUCUN = 0, //rien, on va probablement vouloir que le robot va dans l'état "RECHERCHE_LUMIERE"
  SENSOR_LUM_DR, //Capteur de lumière de droite, tourne vers la droite
  SENSOR_LUM_GA, //Henri
  SENSOR_LUM_AV, //Édouard
  SENSOR_LUM_AR, //Antoine
  SENSOR_IR_DR, //Henri
  SENSOR_IR_GA, //Antoine
  BOTH_IR, //Si les 2 capteurs IR détecte de quoi, c'est parce qu le robot fait face à un mur ou bien est dans un coin
  DOUBLE_LUM //Édouard
};

/************************* DÉCLARATIONS DE FONCTIONS. *************************/
void etat_machine_run(uint8_t sensors);
uint8_t gestionCapteurs();
/************************* VALEURS GLOBALS. *************************/
//struct robot sparx;

/************************* SETUP. *************************/

void setup() {
  // put your setup code here, to run once:
  BoardInit();
  //start de timer
  sparx.startTimer = millis();
  sparx.timerRunning = true;
  sparx.etat = STOP;
}
/************************* MAIN/LOOP. *************************/

void loop() {
  if (sparx.timerRunning && ((millis() - sparx.startTimer) > TIMER_TIME))
  {
    etat_machine_run(gestionCapteurs());
  }
}


/************************* FONCTIONS. *************************/
/*
 * @brief Fonction qui va gérer toutes les intéractions avec les capteurs. On parle ici
 * de lecture de capteurs, stockage de données (aux besoins), gestion de priorité. 
 * ** Les capteurs IR de distances ont toujours priorités aux restants**
 * @param rien
 * @return uint8_t: la fonction retourne quels capteurs sont présentement actif selon un ordre de priorité. Pas besoin de retourner SENSOR_LUM_AV si
 * le robot détecte aussi un mu en avant
 */
uint8_t gestionCapteurs() {
  /* code pour appeler les différent capteurs ça va tout aller icite*/
  return AUCUN;
}

void etat_machine_run(uint8_t sensors) 
{
  //selon l'état du robot
  switch(sparx.etat)
  {
    //si l'état est à STOP
    case STOP:
      //et le robot voit rien
      if(sensors == AUCUN){
        //Change état à recherche lumière
      }
      //voit un mur à droite
      else if(sensors == IR_DROITE){
        //Change état à tourne gauche
      }
      //voit un mur à gauche
      else if(sensors == IR_GAUCHE){
        //Change état à tourne gauche
      }
      //voit de la lumière en avant
      else if(sensors == SENSOR_LUM_AV){
        //Change état à avance tout droit
      }
      //voit de la lumière à droite
      else if(sensors == SENSOR_LUM_DR){
        //Change état à tourne droite
      }
      //voit de la lumière à gauche
      else if(sensors == SENSOR_LUM_GA){
        //Change état à tourne gauche
      }
      //voit de la lumière en arrière
      else if(sensors == SENSOR_LUM_AR){
        //Change état à recule ou 180
      }
      //2 capteurs de lumière ont la même valeur
      else if(sensors == DOUBLE_LUM){
        //Change état à LIFT UP
      }
      //2 capteurs IR voient quelque chose
      else if(sensors == BOTH_IR){
        //Change état à recule ou 180
      }
      else
        //ERROR
      break;
       
    //Henri
    case AVANCE:
      break;
 
    //si l'état est à tourne à droite
    case TOURNE_DROITE:
      //et le robot voit rien
      if(sensors == AUCUN){
        //Change état à recherche lumière
      }
      //voit un mur à droite
      else if(sensors == IR_DROITE){
        //Change état à STOP
      }
      //voit un mur à gauche
      else if(sensors == IR_GAUCHE){
        //Garde état à tourne droite
      }
      //voit de la lumière en avant
      else if(sensors == SENSOR_LUM_AV){
        //Change état à avance tout droit
      }
      //voit de la lumière à droite
      else if(sensors == SENSOR_LUM_DR){
        //Garde son état à tourne droite
      }
      //voit de la lumière à gauche
      else if(sensors == SENSOR_LUM_GA){
        //Change état à tourne gauche
      }
      //voit de la lumière en arrière
      else if(sensors == SENSOR_LUM_AR){
        //Change état à STOP
      }
      //2 capteurs de lumière ont la même valeur
      else if(sensors == DOUBLE_LUM){
        //Change état à STOP
      }
      //2 capteurs IR voient quelque chose
      else if(sensors == BOTH_IR){
        //Change état à STOP
      }
      else
        //ERROR
      break;
    //Édouard
    case TOURNE_GAUCHE:
      break;
    //Antoine
    case RECULE:
      break;
    //si l'état est tourne 180
    //Il faudrait rajouter une fonction qui regarde si le 180 du robot est complet...
    case TOURNE_180:
      //et le robot voit rien
      if(sensors == AUCUN /*et 180 complété*/){
        //Change état à recherche lumière
      }
      //voit un mur à droite
      else if(sensors == IR_DROITE /*et 180 complété*/){
        //Change état à tourne gauche
      }
      //voit un mur à gauche
      else if(sensors == IR_GAUCHE/*et 180 complété*/){
        //Change état à tourne gauche
      }
      //voit de la lumière en avant
      else if(sensors == SENSOR_LUM_AV/*et 180 complété*/ ){
        //Change état à avance tout droit
      }
      //voit de la lumière à droite
      else if(sensors == SENSOR_LUM_DR/*et 180 complété*/){
        //Change état à tourne droite
      }
      //voit de la lumière à gauche
      else if(sensors == SENSOR_LUM_GA/*et 180 complété*/){
        //Change état à tourne gauche
      }
      //voit de la lumière en arrière
      else if(sensors == SENSOR_LUM_AR){
        //refait un 180????? Ou bien continue son 180
      }
      //2 capteurs de lumière ont la même valeur
      else if(sensors == DOUBLE_LUM){
        //refait un 180????? ou bien continue son 180??
      }
      //2 capteurs IR voient quelque chose
      else if(sensors == BOTH_IR){
        ///refait un 180????? ou bien continue son 180??
      }
      else
        //ERROR
      break;
      //si l'état et à LIFT UP
    case LIFT_UP:
      //et le robot voit rien
      if(sensors == AUCUN){
        //Change état à LIFT DOWN
      }
      //voit un mur à droite
      else if(sensors == IR_DROITE){
        //Change état à LIFT DOWN
      }
      //voit un mur à gauche
      else if(sensors == IR_GAUCHE){
        //Change état à LIFT DOWN
      }
      //voit de la lumière en avant
      else if(sensors == SENSOR_LUM_AV){
        //Change état à LIFT DOWN
      }
      //voit de la lumière à droite
      else if(sensors == SENSOR_LUM_DR){
        //Change état à LIFT DOWN
      }
      //voit de la lumière à gauche
      else if(sensors == SENSOR_LUM_GA){
        //Change état à LIFT DOWN
      }
      //voit de la lumière en arrière
      else if(sensors == SENSOR_LUM_AR){
        //Change état à LIFT DOWN
      }
      //2 capteurs de lumière ont la même valeur
      else if(sensors == DOUBLE_LUM /*ET le lift up est terminer*/){
        //Change état à MAINTIENT POSITION
      }
      //2 capteurs IR voient quelque chose
      else if(sensors == BOTH_IR ){
        //Change état à LIFT DOWN
      }
      else
        //ERROR
      break;
      //si l'état et à MAINTIENT Position
    case MAINTIENT_POSITION:
      //et le robot voit rien
      if(sensors == AUCUN){
        //Change état à LIFT DOWN
      }
      //voit un mur à droite
      else if(sensors == IR_DROITE){
        //Change état à LIFT DOWN
      }
      //voit un mur à gauche
      else if(sensors == IR_GAUCHE){
        //Change état à LIFT DOWN
      }
      //voit de la lumière en avant
      else if(sensors == SENSOR_LUM_AV){
        //Change état à LIFT DOWN
      }
      //voit de la lumière à droite
      else if(sensors == SENSOR_LUM_DR){
        //Change état à LIFT DOWN
      }
      //voit de la lumière à gauche
      else if(sensors == SENSOR_LUM_GA){
        //Change état à LIFT DOWN
      }
      //voit de la lumière en arrière
      else if(sensors == SENSOR_LUM_AR){
        //Change état à LIFT DOWN
      }
      //2 capteurs de lumière ont la même valeur
      else if(sensors == DOUBLE_LUM){
        //Garde son état actuel
      }
      //2 capteurs IR voient quelque chose
      else if(sensors == BOTH_IR){
        ////Change état à LIFT DOWN
      }
      else
        //ERROR
      break;
    //Édouard
    case LIFT_DOWN:
      break;
    //Henri
    case MANUEL:
      break;
    //Antoine
    case RECHERCHE_LUMIERE:
      break;
  }
}