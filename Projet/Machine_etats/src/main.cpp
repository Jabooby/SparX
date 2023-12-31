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
#include <SoftwareSerial.h>
#include <SharpIR.h>
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
  SENSOR_IR_AV,
  DOUBLE_LUM, //Deux capteurs de lumière ont la même valeur, arrêt du robot et monte le lift.
  BLUETOOTH
};

/************************* DÉCLARATIONS DE FONCTIONS. *************************/
void etat_machine_run(uint8_t sensors);
uint8_t gestionCapteurs();
uint8_t gestionIR();
void bougerAvance();
void bougerDroite();
void bougerGauche();
void LectureCaptLum(int* valeur);
void LectureCaptIr(int* valeur);
void Demitour();
void getangle(float angle);
void stop();
uint8_t gestionLumiere();
int BTReceive();
int BTWrite(String data);
void bougerRecule();

/************************* VALEURS GLOBALES. *************************/
#define MODEL_IR 1080
struct robot sparx;
int avantpin = A5;
int arrierepin = A6;
int gauchepin = A7;
int droitepin = A8;
bool demitour = false;
SoftwareSerial BTSerial(11, 10); 
char receiveChar = 0;
SharpIR irAvant(IR_PIN[3], MODEL_IR);
SharpIR irDroite(IR_PIN[1], MODEL_IR);
SharpIR irGauche(IR_PIN[2], MODEL_IR);
SharpIR capteursIR[3] = {irAvant, irDroite, irGauche}; 

/************************* SETUP. *************************/

void setup() {
  // put your setup code here, to run once:
  BoardInit();
  //start de timer
  sparx.startTimer = millis();
  sparx.timerRunning = true;
  sparx.etat = STOP;
  BTSerial.begin(9600); 
  Serial.print("Hello World");

}
/************************* MAIN/LOOP. *************************/

void loop() {
  if (sparx.timerRunning && ((millis() - sparx.startTimer) > TIMER_TIME))
  {
    Serial.print("Sensors :"), Serial.println(gestionCapteurs());
    etat_machine_run(gestionCapteurs());
  }
}


/************************* FONCTIONS. *************************/
/*
 * @brief Fonction qui va gérer toutes les intéractions avec les capteurs. On parle ici
 * de lecture de capteurs, stockage de données (aux besoins), gestion de priorité. 
 * ** Les capteurs IR de distances ont toujours priorités par rapport au restant**
 * @param rien
 * @return uint8_t: la fonction retourne quels capteurs sont présentement actifs selon un ordre de priorité. Pas besoin de retourner SENSOR_LUM_AV si
 * le robot détecte aussi un mur en avant
 */
uint8_t gestionCapteurs() 
{
  uint8_t retourLum = gestionLumiere();
  uint8_t retourIR = gestionIR();
  BTReceive();
  //Serial.println(receiveChar);
  if(receiveChar == 'M' || sparx.etat == MANUEL)
  {
    if(receiveChar == 'O')
    {
      sparx.etat = STOP;
      return(AUCUN);
    }

    else
    {
      Serial.println("BLUETOOTH");
      return(BLUETOOTH);
    }
  }
  else if(retourIR != AUCUN)
  {
    return(retourIR);
  }
  
  else if(retourLum != AUCUN)
    return(retourLum);
  else
    return(AUCUN);
}

uint8_t gestionLumiere()
{
   /* code pour appeler les différent capteurs ça va tout aller ici*/
  int capMaxLu=0;
  int emplacement;
  int emplacementMax;
  int lum_pref=1000; // valeur préféré de la plante
  int valeur_capteur[4];
  LectureCaptLum(valeur_capteur);
  for(emplacement=0;emplacement<4;emplacement++)
  {
    if(valeur_capteur[emplacement]>capMaxLu)
    {
      capMaxLu=valeur_capteur[emplacement];
      emplacementMax=emplacement;
    }

    if(capMaxLu>=lum_pref)
    {
      for(emplacement=0; emplacement<4; emplacement++)
      {
        if(valeur_capteur[emplacement]>=capMaxLu-70 && emplacement!=emplacementMax) //possibilité de changer le 70
        emplacementMax=4;  
      }
    }
  }
  switch(emplacementMax)
  {
    case 0:
      return(SENSOR_LUM_AV);
      break;

    case 1:
      return(SENSOR_LUM_DR);
      break;

    case 2:
      return(SENSOR_LUM_GA);
      break;

    case 3:
      return(SENSOR_LUM_AR);
      break;

    case 4:
      return(DOUBLE_LUM);
      break;

    default:
      return AUCUN;
      break;
  }
}

uint8_t gestionIR()
{
 /* code pour appeler les 3 capteurs IR */
  int distanceIR = 10;
  int emplacement;
  int emplacementIR = 4;
  int valeur_capteur[3];

  LectureCaptIr(valeur_capteur);
  for (emplacement = 0; emplacement < 3; emplacement++)
  {
    //Serial.print("Capteur: "), Serial.print(emplacement), Serial.print(" Valeur: "), Serial.println(valeur_capteur[emplacement]);
    if (valeur_capteur[emplacement] <= distanceIR)
    {
      distanceIR = valeur_capteur[emplacement];
      emplacementIR = emplacement;
    }
  }
  switch (emplacementIR)
  {
  case 0:
    return (SENSOR_IR_AV);
    //Serial.print(valeur_capteur[0]);
    break;

  case 1:
    return (SENSOR_IR_DR);
    //Serial.print(valeur_capteur[1]);
    break;

  case 2:
    return (SENSOR_IR_GA);
    //Serial.print(valeur_capteur[2]);
    break;

  case 4:

    return (AUCUN);
    break;

  default: 
    return (AUCUN);
    break;
  }
}

void etat_machine_run(uint8_t sensors) 
{
  //selon l'état du robot
   Serial.print("État robot: "), Serial.println(sparx.etat);
  Serial.print("Sensors robot: "), Serial.println(sensors);
  switch(sparx.etat)
  {
    //si l'état est à STOP
    case STOP:
      //et le robot voit rien
      if(sensors == AUCUN){
        Serial.print("AVANCE TBNK");
        sparx.etat = AVANCE;
      }
      //voit un mur à droite
      else if(sensors == SENSOR_IR_DR){
        bougerGauche();
        sparx.etat = TOURNE_GAUCHE;
      }
      //voit un mur à gauche
      else if(sensors == SENSOR_IR_GA){
        bougerDroite();
        sparx.etat = TOURNE_DROITE;
        //Change état à tourne droite
      }
      //voit de la lumière en avant
      else if(sensors == SENSOR_LUM_AV){
        bougerAvance();
        sparx.etat = AVANCE;
        //Serial.println("je vais tout droit");
      }
      //voit de la lumière à droite
      else if(sensors == SENSOR_LUM_DR){
        bougerDroite();
        sparx.etat = TOURNE_DROITE;
        //Serial.println("Je vais à droite");
      }
      //voit de la lumière à gauche
      else if(sensors == SENSOR_LUM_GA){
        bougerGauche();
        sparx.etat = TOURNE_GAUCHE;
        //Serial.println("Je vais à gauche");
      }
      //voit de la lumière en arrière
      else if(sensors == SENSOR_LUM_AR){
        Demitour();
        sparx.etat = TOURNE_180;
      }
      //2 capteurs de lumière ont la même valeur
      else if(sensors == DOUBLE_LUM){
        sparx.etat = LIFT_UP;
        //Serial.println("Je lift up");
      }
      //sensor IR avant détecte un mur
      else if(sensors == SENSOR_IR_AV){ //ICI
        Demitour();
        sparx.etat = TOURNE_180;
        //Change état à recule ou 180
      }
      //Bluetooth manuel
      else if(sensors == BLUETOOTH){
        stop();
        sparx.etat = MANUEL;
      }
      else
        //ERROR
      break;
       
    //Henri
    case AVANCE:

    if(sensors == AUCUN){ 
        //Serial.println("Je recherche la lumière");
        bougerAvance();
      }
      //voit un mur à droite
      else if(sensors == SENSOR_IR_DR){
        sparx.etat = STOP;
      }
      //voit un mur à gauche
      else if(sensors == SENSOR_IR_GA){
        sparx.etat = STOP;
      }
      //voit de la lumière en avant
      else if(sensors == SENSOR_LUM_AV){
        sparx.etat = STOP;
      }
      //voit de la lumière à droite
      else if(sensors == SENSOR_LUM_DR){
        sparx.etat = STOP;
      }
      //voit de la lumière à gauche
      else if(sensors == SENSOR_LUM_GA){
        sparx.etat = STOP;
      }
      //voit de la lumière en arrière
      else if(sensors == SENSOR_LUM_AR){
        sparx.etat = STOP;
      }
      //sensor IR avant détecte un mur
      else if(sensors == SENSOR_IR_AV){
        sparx.etat = STOP;
      }
      //2 capteurs IR voient quelque chose
      else if(sensors == SENSOR_IR_AV){
        //Change état à recule ou 180
        sparx.etat = AVANCE;
        bougerAvance(); //ICI
      }
      //Bluetooth manuel
      else if(sensors == BLUETOOTH){
        stop();
        sparx.etat = MANUEL;
      }
      else
        //ERROR
      break;

 //si l'état est à tourne 180
    case TOURNE_180:
      //et le robot voit rien
      if(sensors == AUCUN){
        //Garde état à tourne 180
      }
      else if(demitour) // fin du demi tour
      {
        stop();
        sparx.etat = STOP;
        ENCODER_ReadReset(0);
        ENCODER_ReadReset(1);
        demitour = false;
      }
      //Bluetooth manuel
      else if(sensors == BLUETOOTH){
        stop();
        sparx.etat = MANUEL;
      }
      else
        //ERROR
      break;

    //si l'état est à tourne à droite
    case TOURNE_DROITE:
    
      if(sensors == AUCUN){
        sparx.etat = STOP;
      }
      //voit un mur à droite
      else if(sensors == SENSOR_IR_DR){
        sparx.etat = STOP;
      }
      //voit un mur à gauche
      else if(sensors == SENSOR_IR_GA){
        sparx.etat = STOP;
      }
      //voit de la lumière en avant
      else if(sensors == SENSOR_LUM_AV){
        sparx.etat = STOP;
      }
      //voit de la lumière à droite
      else if(sensors == SENSOR_LUM_DR){
        sparx.etat = STOP;
      }
      //voit de la lumière à gauche
      else if(sensors == SENSOR_LUM_GA){
        sparx.etat = STOP;
      }
      //voit de la lumière en arrière
      else if(sensors == SENSOR_LUM_AR){
        sparx.etat = STOP;
      }
      //2 capteurs de lumière ont la même valeur
      else if(sensors == DOUBLE_LUM){
        sparx.etat = STOP;
      }
      //le capteur IR avant détecte quelque chose
      else if(sensors == SENSOR_IR_AV){
        sparx.etat = STOP;
      }
      //Bluetooth manuel
      else if(sensors == BLUETOOTH){
        stop();
        sparx.etat = MANUEL;
      }
      else
        //ERROR
      break;

    //si l'état est à tourne gauche
    case TOURNE_GAUCHE:
    
      if(sensors == AUCUN){
        sparx.etat = STOP;
      }
      //voit un mur à droite
      else if(sensors == SENSOR_IR_DR){
        sparx.etat = STOP;
      }
      //voit un mur à gauche
      else if(sensors == SENSOR_IR_GA){
        sparx.etat = STOP;
      }
      //voit de la lumière en avant
      else if(sensors == SENSOR_LUM_AV){
        sparx.etat = STOP;
      }
      //voit de la lumière à droite
      else if(sensors == SENSOR_LUM_DR){
        sparx.etat = STOP;
      }
      //voit de la lumière à gauche
      else if(sensors == SENSOR_LUM_GA){
        sparx.etat = STOP;
      }
      //voit de la lumière en arrière
      else if(sensors == SENSOR_LUM_AR){
        sparx.etat = STOP;
      }
      //2 capteurs de lumière ont la même valeur
      else if(sensors == DOUBLE_LUM){
        sparx.etat = STOP;
      }
      //sensor IR avant détecte un mur
      else if(sensors == SENSOR_IR_AV){
        sparx.etat = STOP;
      }
      //Bluetooth manuel
      else if(sensors == BLUETOOTH){
        stop();
        sparx.etat = MANUEL;
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
      else if(sensors == SENSOR_IR_DR){
        //Garde état à LIFT UP
      }
      //voit un mur à gauche
      else if(sensors == SENSOR_IR_GA){
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
       //
      else if(sensors == SENSOR_IR_AV){
        //Garde état à LIFT UP
      }
      //Bluetooth manuel
      else if(sensors == BLUETOOTH){
        stop();
        sparx.etat = MANUEL;
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
      }
      if(sensors == AUCUN){
        //Change état à LIFT DOWN
      }
      //voit un mur à droite
      else if(sensors == SENSOR_IR_DR){
        //Change état à LIFT DOWN
      }
      //voit un mur à gauche
      else if(sensors == SENSOR_IR_GA){
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
      //Bluetooth manuel
      else if(sensors == BLUETOOTH){
        stop();
        sparx.etat = MANUEL;
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
      else if(sensors == SENSOR_IR_DR){
        //Garde état à LIFT DOWN
      }
      //voit un mur à gauche
      else if(sensors == SENSOR_IR_GA){
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
       //sensor IR avant détecte un mur
      else if(sensors == SENSOR_IR_AV){
        //Garde état à LIFT DOWN
      }
      //Bluetooth manuel
      else if(sensors == BLUETOOTH){
        stop();
        sparx.etat = MANUEL;
      }
      else
        //ERROR
      break;
    //Henri 
    case MANUEL:
      Serial.println("Manuel");
      switch(receiveChar)
      {
        case('W'):
          bougerAvance();
          break;
        case('A'):
          bougerGauche();
          break;
        case('S'):
          bougerRecule();
          break;
        case('D'):
          bougerDroite();
          break;
        case('Q'):
          stop();
          break;
        default:
          stop();
          break;
      }
        break;


    /*  break;
    case RECHERCHE_LUMIERE:
    //et le robot voit rien
      if(sensors == AUCUN){
        //Change état à recherche lumière
      }
      //voit un mur à droite
      else if(sensors == SENSOR_IR_DR){
        //Change état à STOP
      }
      //voit un mur à gauche
      else if(sensors == SENSOR_IR_GA){
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
    }*/
  }
}

void bougerAvance()
{
  MOTOR_SetSpeed(RIGHT, 0.1);
  MOTOR_SetSpeed(LEFT, 0.1);
}
void bougerRecule()
{
  MOTOR_SetSpeed(RIGHT, -0.1);
  MOTOR_SetSpeed(LEFT, -0.1);
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
void stop()
{
  MOTOR_SetSpeed(RIGHT, 0);
  MOTOR_SetSpeed(LEFT, 0);
}
void Demitour()
{
  bougerDroite();
  getangle(180.0);
}

  void getangle(float angle)
  {
  float coefficient = 360.0 /angle ;
  float circonference = (22.0*PI);
  float distance = (circonference/coefficient);
  float nbtours = (distance/23.93);
  float nbpulses = (nbtours*3200.0);
  //Serial.println((int)nbpulses);
  int moteur_droit = ENCODER_Read(1);

  if (abs(moteur_droit) > (nbpulses-10))
  {
    //Serial.println("fin de 180");
    demitour = true;
  }

}

void LectureCaptLum(int* valeur) {

  int pin_analogue[4] = {A5,A8,A7,A6}; //A5 = Avant,  A6 = Arrière, A7 = Gauche, A8 = Droite
  int i;


  for(i=0; i<4 ;i++)
  { 
    valeur[i]=analogRead(pin_analogue[i]); //valeurs pour les 4 capteurs
    //Serial.println(valeur_capteur[i]);
  }
}

void LectureCaptIr(int* valeur)
{
  for (int i = 0; i < 3; i++)
  {
    valeur[i] = capteursIR[i].distance(); // valeurs pour les trois capteurs IR
  }
}

//version qui retourne la connexion: 1-Connected 0-!Connected
int BTReceive(){
   if (BTSerial.available()) {
    receiveChar = BTSerial.read();
    UDR0 = receiveChar; //Synonyme a Serial.print();
    return 1;
  }
  else return 0; // pas de connexion
}

//version qui retourne la valeure lu. Retourne 0 quand rien est recu

/*char BTReceive() {
  if (BTSerial.available()) {
    return BTSerial.read();
  } else {
    return '0'; // Sentinel value indicating no data
  }
}*/

int BTWrite(String data){
  BTSerial.println(data);
  return 1;
}