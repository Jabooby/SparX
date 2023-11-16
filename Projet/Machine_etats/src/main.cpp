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
  TOURNE_GAUCHE, //Robot tourne à gauche, Vitesse roue droite > Vitesse roue gauche
  RECULE, //Antoine
  TOURNE_180, //À voir, peut-être très utile mais pas nécessaire. À confirmer
  LIFT_UP, //Lève la plante
  MAINTIENT_POSITION,  // Maitient la hauteur de la plante
  LIFT_DOWN, //Descend le lift
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
//FAITES JUSTE ÉCRIRE UN COMMENTAIRE POUR DÉCRIRE LE CAPTEUR
enum Sensors_enum 
{
  AUCUN = 0, //rien, on va probablement vouloir que le robot va dans l'état "RECHERCHE_LUMIERE"
  SENSOR_LUM_DR, //Capteur de lumière de droite, tourne vers la droite
  SENSOR_LUM_GA, //Henri
  SENSOR_LUM_AV, //Capteur de lumière avant, avance
  SENSOR_LUM_AR, //Antoine
  SENSOR_IR_DR, //Henri
  SENSOR_IR_GA, //Antoine
  BOTH_IR, //Si les 2 capteurs IR détecte de quoi, c'est parce qu le robot fait face à un mur ou bien est dans un coin
  DOUBLE_LUM //Deux capteurs de lumière ont la même valeur, arrêt du robot et monte le lift.
};

/************************* DÉCLARATIONS DE FONCTIONS. *************************/
void etat_machine_run(uint8_t sensors);
uint8_t gestionCapteurs();
void bougerAvance();
void bougerDroite();
void bougerGauche();
int* LectureCaptLum();
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
   int*p=LectureCaptLum();
 int capMaxLu=0;
 int emplacement;
 int emplacementMax;
 int lum_pref=1000;
 for(emplacement=0;emplacement<4;emplacement++)
 {
  if(p[emplacement]>capMaxLu)
  {
    capMaxLu=p[emplacement];
    emplacementMax=emplacement;
  }

  if(capMaxLu==lum_pref)
  {
    for(emplacement=0; emplacement<4; emplacement++)
    {
      if(p[emplacement]>capMaxLu-70)
      emplacementMax=4;
    }
  }
  p[emplacementMax];
 }
 switch(emplacementMax)
 {
  case 0:
  return(SENSOR_LUM_AV);

  case 1:
  return(SENSOR_LUM_DR);

  case 2:
  return(SENSOR_LUM_GA);

  case 3:
  return(SENSOR_LUM_AR);

  case 4:
  return(DOUBLE_LUM);
 }
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
        //Change état à tourne droite
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

 //si l'état est à tourne 180
    case TOURNE_180:
      //et le robot voit rien
      if(sensors == AUCUN){
        //Garde état à tourne 180
      }
      //voit un mur à droite
      else if(sensors == IR_DROITE){
        //Garde état à tourne 180
      }
      //voit un mur à gauche
      else if(sensors == IR_GAUCHE){
        //Garde état à tourne 180
      }
      //voit de la lumière en avant
      else if(sensors == SENSOR_LUM_AV){
        //Garde état à tourne 180
      }
      //voit de la lumière à droite
      else if(sensors == SENSOR_LUM_DR){
        //Garde état à tourne 180
      }
      //voit de la lumière à gauche
      else if(sensors == SENSOR_LUM_GA){
        //Garde état à tourne 180
      }
      //voit de la lumière en arrière
      else if(sensors == SENSOR_LUM_AR){
        //Garde état à tourne 180
      }
      //2 capteurs de lumière ont la même valeur
      else if(sensors == DOUBLE_LUM){
        //Change état à STOP
      }
      //2 capteurs IR voient quelque chose
      else if(sensors == BOTH_IR){
        //Garde état à tourne 180
      }
      else
        //ERROR
      break;

    //si l'état est à tourne à droite
    case TOURNE_DROITE:
      //et le robot voit rien
      if(sensors == AUCUN){
        //Change état à STOP
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
        //Change état à STOP
      }
      //voit de la lumière à droite
      else if(sensors == SENSOR_LUM_DR){
        //Garde son état à STOP
      }
      //voit de la lumière à gauche
      else if(sensors == SENSOR_LUM_GA){
        //Change état à STOP
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

    //si l'état est à tourne gauche
    case TOURNE_GAUCHE:
    //et le robot voit rien
      if(sensors == AUCUN){
        //Change état à STOP
      }
      //voit un mur à droite
      else if(sensors == IR_DROITE){
        //Garde état à tourne gauche
      }
      //voit un mur à gauche
      else if(sensors == IR_GAUCHE){
        //Change état à STOP
      }
      //voit de la lumière en avant
      else if(sensors == SENSOR_LUM_AV){
        //Change état à STOP
      }
      //voit de la lumière à droite
      else if(sensors == SENSOR_LUM_DR){
        //Change état à STOP
      }
      //voit de la lumière à gauche
      else if(sensors == SENSOR_LUM_GA){
        //Garde son état tourne à gauche
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

       //si l'état est LIFT UP
    case LIFT_UP:
    //et le robot voit rien
      if(sensors == AUCUN){
        //Garde état à LIFT UP
      }
       //voit un mur à droite
      else if(sensors == IR_DROITE){
        //Garde état à LIFT UP
      }
      //voit un mur à gauche
      else if(sensors == IR_GAUCHE){
        //Garde état à  LIFT UP
      }
      //voit de la lumière en avant
      else if(sensors == SENSOR_LUM_AV){
        //Garde état à LIFT UP
      }
      //voit de la lumière à droite
      else if(sensors == SENSOR_LUM_DR){
        //Garde état à LIFT UP
      }
      //voit de la lumière à gauche
      else if(sensors == SENSOR_LUM_GA){
        //Garde état à LIFT UP
      }
      //voit de la lumière en arrière
      else if(sensors == SENSOR_LUM_AR){
        //Garde état à LIFT UP
      }
      //2 capteurs de lumière ont la même valeur
      else if(sensors == DOUBLE_LUM ){
        //Garde état à LIFT UP
      }
       //2 capteurs IR voient quelque chose
      else if(sensors == BOTH_IR){
        //Garde état à LIFT UP
      }
      else
        //ERROR
      break;
    
      //si l'état est à MAINTIENT Position
    case MAINTIENT_POSITION:
      //et le robot voit rien
      //Vérifie si sa roation est fini
      if(sensors == 0/*ROTATION_LIFT*/){
        //Les autres se vérifie seulement au moment là
      if(sensors == AUCUN){
        //Change état à LIFT DOWN
      }
      //voit un mur à droite
      else if(sensors == IR_DROITE){
        //Change état à LIFT DOWN
      }
      //voit un mur à gauche
      else if(sensors == IR_GAUCHE){
        //CHANGE état à  LIFT DOWN
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
        //Garde son état MAINTIENT Position
      }
      
      else{
        //ERROR
      }
      break;
    //si l'état est LIFT DOWN
    case LIFT_DOWN:
    //et le robot voit rien
      if(sensors == AUCUN){
        //Garde état à LIFT DOWN
      }
       //voit un mur à droite
      else if(sensors == IR_DROITE){
        //Garde état à LIFT DOWN
      }
      //voit un mur à gauche
      else if(sensors == IR_GAUCHE){
        //Garde état à  LIFT DOWN
      }
      //voit de la lumière en avant
      else if(sensors == SENSOR_LUM_AV){
        //Garde état à LIFT DOWN
      }
      //voit de la lumière à droite
      else if(sensors == SENSOR_LUM_DR){
        //Garde état à LIFT DOWN
      }
      //voit de la lumière à gauche
      else if(sensors == SENSOR_LUM_GA){
        //Garde état à LIFT DOWN
      }
      //voit de la lumière en arrière
      else if(sensors == SENSOR_LUM_AR){
        //Garde état à LIFT DOWN
      }
      //2 capteurs de lumière ont la même valeur
      else if(sensors == DOUBLE_LUM ){
        //Change état à MAINTIENT POSITION
      }
       //2 capteurs IR voient quelque chose
      else if(sensors == BOTH_IR){
        //Garde état à LIFT DOWN
      }
      else
        //ERROR
      break;
    //Henri
    case MANUEL:
      break;
    //Antoine
    case RECHERCHE_LUMIERE:
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
        //Change d'état à STOP
      }
      //voit de la lumière en avant
      else if(sensors == SENSOR_LUM_AV){
        //Change état à STOP
      }
      //voit de la lumière à droite
      else if(sensors == SENSOR_LUM_DR){
        //change d'état à STOP
      }
      //voit de la lumière à gauche
      else if(sensors == SENSOR_LUM_GA){
        //Change état à STOP
      }
      //voit de la lumière en arrière
      else if(sensors == SENSOR_LUM_AR){
        //Change soon état à STOP
      }
      //2 capteurs de lumière ont la même valeur
      else if(sensors == DOUBLE_LUM){
        //Change état à stop
      }
      //2 capteurs IR voient quelque chose
      else if(sensors == BOTH_IR){
        //Change son état à STOP
      break;
      }
    }
  }
}

void bougerAvant()
{
  MOTOR_SetSpeed(RIGHT, 0.1);
  MOTOR_SetSpeed(LEFT, 0.1);
}
void bougerDroite()
{
  MOTOR_SetSpeed(RIGHT, -0.1);
  MOTOR_SetSpeed(LEFT, 0.1);
}
void bougerGauche()
{
  MOTOR_SetSpeed(RIGHT, 0.1);
  MOTOR_SetSpeed(LEFT, -0.1);
}

int* LectureCaptLum() 
{
  int valeur_capteur[4];
  int pin_analogue[4] = {A0,A1,A2,A3};
  int i;


  for(i=0; i<4 ;i++)
  { 
    valeur_capteur[i]=analogRead(pin_analogue[i]);
  }
  return(valeur_capteur);
}