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
#include <RFID.h>
#include <Adafruit_AM2320.h>
#include <Adafruit_Sensor.h>
#include <Wire.h>
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
void LCDCapteurs(uint8_t* donneeNFC, uint8_t* donneeCapteurs);
void gestionTXBT(uint8_t* donneeCapteurs);
int getTempAmbiant();
int getHumidityAmbiant();
int getHumidityDirt();
int calculHumidityDirt();
void LCDWrite(int column, int row, uint8_t* data);
void liftup();
void LiftDown();
void LiftStop();
void RotationDroite();
void RotationGauche();
void RotationStop();
uchar* CardRead();

/************************* VALEURS GLOBALES. *************************/
#define MODEL_IR 1080
int avantpin = A5;
int arrierepin = A6;
int gauchepin = A7;
int droitepin = A8;
bool demitour = false;
LiquidCrystal_I2C lcd(0x27,16,2);
Adafruit_AM2320 am2320 = Adafruit_AM2320();
SoftwareSerial BTSerial(11, 10); 
char receiveChar = 0;
double timerCapteurs;
SharpIR irAvant(IR_PIN[3], MODEL_IR);
SharpIR irGauche(IR_PIN[1], MODEL_IR);
SharpIR irDroite(IR_PIN[2], MODEL_IR);
SharpIR capteursIR[3] = {irAvant, irDroite, irGauche}; 
//Moteur Rotation
int enA = A10;
int in1 = 44;
int in2 = 42;
//Moteur Lift
int enB = A11;
int in3 = 46;
int in4 = 40;
double timerlift;
double timerrotation;
uchar serNum[5];
uint8_t donneeNFC[] = "cactus      T05 H10 h07 L87   F"; //https://tools.word-counter.com/character-count/result.html
//uchar* CardRead();
/************************* SETUP. *************************/

void setup() {
  // put your setup code here, to run once:
  BoardInit();
  //start de timer
  sparx.startTimer = millis();
  timerCapteurs = millis();
  sparx.timerRunning = true;
  sparx.etat = STOP;
  SPI.begin();
  BTSerial.begin(9600); 
  lcd.init();
  lcd.backlight(); // ativer lumière arrière
  //Pinmode for MFRC522
  pinMode(chipSelectPin, OUTPUT);       
  digitalWrite(chipSelectPin, LOW);    
  pinMode(NRSTPD, OUTPUT);               
  MFRC522_Init();
  // Set all the motor control pins to outputs
	pinMode(in1, OUTPUT);
	pinMode(in2, OUTPUT);
	pinMode(in3, OUTPUT);
	pinMode(in4, OUTPUT);
	// Turn off motors lift - Initial state
	digitalWrite(in1, LOW);
	digitalWrite(in2, LOW);
	digitalWrite(in3, LOW);
	digitalWrite(in4, LOW);

}
/************************* MAIN/LOOP. *************************/

void loop() {
  if (sparx.timerRunning && ((millis() - sparx.startTimer) > TIMER_TIME))
  {
    sparx.startTimer = millis();
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
  uint8_t nbByte = 32;
  uint8_t donneeCapteurs[nbByte];
  CardRead();
  uint8_t msgErreur[] = "ERREUR";
  uint8_t msgCactus[] = "cactus";
  uint8_t msgChampignon[] = "champignon";
  uint8_t msgSunflower[] = "tournesol";
  uint8_t retourLum = gestionLumiere();
  uint8_t retourIR = gestionIR();
  uint8_t idNFC = ((int)serNum[0])+((int)serNum[1])+((int)serNum[2])+((int)serNum[3]);
  BTReceive();
  LCDCapteurs(donneeNFC, donneeCapteurs);
  //Serial.println(idNFC);
  if(idNFC != 0)
  {
    switch(idNFC)
    {
      case(6): //Cactus
        //Serial.println("cactus");
        for(int i = 0; i < 6; i++)
        {
          donneeNFC[i] = msgCactus[i];
        }
        for(int i = 6; i < 12; i++)
        {
          donneeNFC[i] = ' ';
        }
        break;
      case(219): //champignon
        //Serial.println("champignon");
        for(int i = 0; i < 10; i++)
        {
          donneeNFC[i] = msgChampignon[i];
        }
        donneeNFC[10] = ' ';
        donneeNFC[11] = ' ';
        break;
      case(167): //Sunflower
        //Serial.println("tournesol");
        for(int i = 0; i < 9; i++)
        {
          donneeNFC[i] = msgSunflower[i];
        }
        donneeNFC[9] = ' ';
        donneeNFC[10] = ' ';
        donneeNFC[11] = ' ';
        break;
    }
  }
  if(receiveChar == 'M' || sparx.etat == MANUEL)
  {
    if(receiveChar == 'O')
    {
      sparx.etat = STOP;
      return(AUCUN);
    }
    else
    {
      //Serial.println("BLUETOOTH");
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

void LCDCapteurs(uint8_t* donneeNFC, uint8_t* donneeCapteurs)
{
  //tester ici. Timer trop élevé, seulement les methodes dans le millis se font. Ajouter fonctions.
  if((millis() - timerCapteurs)> 15000)
  {
    timerCapteurs = millis();
    uint8_t donneeLCDCapteurs[21];
    uint8_t humidityAmbiant[3];
    uint8_t tempAmbiant[3];
    uint8_t humidityDirt[3];
    uint8_t donneeNOM[12] = "          ";
    uint8_t donneeEspace[]= {32, 32, 32, 32};
     uint8_t nbByte = 32;
    //lecture_NFC_NbByte(69, nbByte, donneeNFC); //NFC PP POOPOO
    //Serial.print("Donnée NFC: "),Serial.println((char*)donneeNFC);
    //Serial.print("Donnée Nom: "),Serial.println((char*)donneeNOM);
    for(int i = 0; i<12; i++)
    {
      donneeNOM[i] = donneeNFC[i];
      //Serial.print("Donnée Nom: "), Serial.print(i), Serial.print(": "),Serial.println((char*)donneeNOM);
      if(i < 5)
        donneeCapteurs[i] = donneeNFC[i];
      else
        donneeCapteurs[i] = 32;
    }
    //Serial.print("Donnée Nom: "),Serial.println((char*)donneeNOM);
    itoa(getHumidityAmbiant(), (char*)humidityAmbiant, 10);
    //Serial.println((char*) humidityAmbiant);
    itoa(getTempAmbiant(), (char*)tempAmbiant, 10);
    //Serial.println((char*) tempAmbiant);
    itoa(calculHumidityDirt(), (char*)humidityDirt, 10);
    //Serial.println((char*) humidityDirt);
    donneeLCDCapteurs[0] = 'T';
    donneeLCDCapteurs[1] = '2';
    if(tempAmbiant[1] < '0')
      tempAmbiant[1] = '0';
    donneeLCDCapteurs[2] = tempAmbiant[1], donneeLCDCapteurs[3] = 32; //temp
    donneeLCDCapteurs[4] = 'H', donneeLCDCapteurs[5] = humidityAmbiant[0];
    if(humidityAmbiant[1] < '0')
      humidityAmbiant[1] = '0';
    donneeLCDCapteurs[6] = humidityAmbiant[1], donneeLCDCapteurs[7] = 32;
    donneeLCDCapteurs[8] = 'h', donneeLCDCapteurs[9] = humidityDirt[0];
    if(humidityDirt[1] < '0')
      humidityDirt[1] = '0';
    donneeLCDCapteurs[10] = humidityDirt[1], donneeLCDCapteurs[11] = 32;
    donneeLCDCapteurs[12] = ' ', donneeLCDCapteurs[13] = ' ', donneeLCDCapteurs[14] = ' ', donneeLCDCapteurs[15] = 32;
    donneeLCDCapteurs[16] = 32, donneeLCDCapteurs[17] = 32, donneeLCDCapteurs[18] = 32, donneeLCDCapteurs[19] = 'F'; //F ou \n(valeur ascii 10)
    //Serial.print("Donnée Nom: "),Serial.println((char*)donneeNOM);
    //Serial.println((char*) donneeLCDCapteurs);
    LCDWrite(0,1, donneeLCDCapteurs);
    LCDWrite(0,0, donneeNOM);
    LCDWrite(12,0, donneeEspace);
    for(int i = 12; i < 32; i++)
    {
      donneeCapteurs[i] = donneeLCDCapteurs[i-12];
    }
    gestionTXBT(donneeCapteurs);
  }
  
}

void gestionTXBT(uint8_t* donneeCapteurs)
{
  Serial.println((char*) donneeCapteurs);
  char donneeBT[17];
  uint8_t j = 0;
  for(int i = 0; i < 32; i ++)
  {
    if(donneeCapteurs[i]!= ' ')
    {
      donneeBT[j] = (char)donneeCapteurs[i];
      //Serial.print("Donnée BT: "), Serial.print(j), Serial.print(": "),Serial.println((char*)donneeBT);
      j++;
    }
  }
  donneeBT[14] = 'F';
  donneeBT[15] = '\n';
  donneeBT[16] = '\0';
  if(receiveChar == 'i')
  {
    Serial.print(donneeBT);
    Serial.println(' ');
    BTSerial.println(donneeBT);
    receiveChar = '\0';
  }
}
int getTempAmbiant() 
{
  int temp = am2320.readTemperature();
  Serial.print("Température A: "); Serial.println(temp);
  return (temp);
  
}
int getHumidityAmbiant() 
{
  int humidity = am2320.readHumidity();
 Serial.print("Humidité A: "); Serial.println(humidity);
  return (humidity);
}

int getHumidityDirt() //Permet d'avoir le "Raw Data" du détecteur d'humidité
{
  int val;
  val = analogRead(A9); //connect sensor to Analog 0
  return val;
}
int calculHumidityDirt() //Transforme le "Raw Data en pourcentage"
{
  float val2;
  val2 = (((getHumidityDirt()-440.0)/-334.0)*100);
  Serial.print("Humidité T: "); Serial.println((int)val2);
  return (int)val2;
}

void LCDWrite(int column, int row, uint8_t* data){
 // 
  //Écrire sur la colonne et la rangée spécifié. (Commence par 0)
  lcd.setCursor(column,row);
  lcd.print((char*)data);

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
  int distanceIR = 13;
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
  //Serial.print("État robot: "), Serial.println(sparx.etat);
  //Serial.print("Sensors robot: "), Serial.println(sensors);
  switch(sparx.etat)
  {
    //si l'état est à STOP
    case STOP:
      //et le robot voit rien
      if(sensors == AUCUN){
        sparx.etat = AVANCE;
        break;
      }
      //voit un mur à droite
      else if(sensors == SENSOR_IR_DR){
        bougerGauche();
        sparx.etat = TOURNE_GAUCHE;
        break;
      }
      //voit un mur à gauche
      else if(sensors == SENSOR_IR_GA){
        bougerDroite();
        sparx.etat = TOURNE_DROITE;
        break;
        //Change état à tourne droite
      }
      //voit de la lumière en avant
      else if(sensors == SENSOR_LUM_AV){
        bougerAvance();
        sparx.etat = AVANCE;
        break;
        //Serial.println("je vais tout droit");
      }
      //voit de la lumière à droite
      else if(sensors == SENSOR_LUM_DR){
        bougerDroite();
        sparx.etat = TOURNE_DROITE;
        break;
        //Serial.println("Je vais à droite");
      }
      //voit de la lumière à gauche
      else if(sensors == SENSOR_LUM_GA){
        bougerGauche();
        sparx.etat = TOURNE_GAUCHE;
        break;
        //Serial.println("Je vais à gauche");
      }
      //voit de la lumière en arrière
      else if(sensors == SENSOR_LUM_AR){
        ENCODER_ReadReset(0);
        ENCODER_ReadReset(1);
        Demitour();
        sparx.etat = TOURNE_180;
        break;
      }
      //2 capteurs de lumière ont la même valeur
      else if(sensors == DOUBLE_LUM){
        //sparx.etat = STOP;
        //stop();
        timerlift= millis();
        liftup();
        stop();
        sparx.etat = LIFT_UP;
        //Serial.println(sparx.etat);
        break;
      }
      //sensor IR avant détecte un mur
      else if(sensors == SENSOR_IR_AV){ //ICI
        Demitour();
        sparx.etat = TOURNE_180;
        break;
        //Change état à recule ou 180
      }
      //Bluetooth manuel
      else if(sensors == BLUETOOTH){
        stop();
        sparx.etat = MANUEL;
        break;
      }
      else
        //ERROR
      break;
       
    //Henri
    case AVANCE:

    if(sensors == AUCUN){ 
        //Serial.println("Je recherche la lumière");
        bougerAvance();
        sparx.etat = AVANCE;
        break;
      }
      //voit un mur à droite
      else if(sensors == SENSOR_IR_DR){
        sparx.etat = STOP;
        break;
      }
      //voit un mur à gauche
      else if(sensors == SENSOR_IR_GA){
        sparx.etat = STOP;
        break;
      }
      //voit de la lumière en avant
      else if(sensors == SENSOR_LUM_AV){
        bougerAvance();
        sparx.etat = AVANCE;
        break;
      }
      //voit de la lumière à droite
      else if(sensors == SENSOR_LUM_DR){
        sparx.etat = STOP;
        break;
      }
      //voit de la lumière à gauche
      else if(sensors == SENSOR_LUM_GA){
        sparx.etat = STOP;
        break;
      }
      //voit de la lumière en arrière
      else if(sensors == SENSOR_LUM_AR){
        sparx.etat = STOP;
        break;
      }
      //sensor IR avant détecte un mur
      else if(sensors == SENSOR_IR_AV){
        sparx.etat = STOP;
        break;
      }
      else if(sensors == DOUBLE_LUM){
        sparx.etat = STOP;
        break;
      }
      //Bluetooth manuel
      else if(sensors == BLUETOOTH){
        stop();
        sparx.etat = MANUEL;
        break;
      }
      else
        //ERROR
      break;

 //si l'état est à tourne 180
    case TOURNE_180:
      //et le robot voit rien
      getangle(180.0);
      if(sensors == AUCUN){
        //Garde état à tourne 180
      }
      else if(demitour) // fin du demi tour
      {
        stop();
        ENCODER_ReadReset(0);
        ENCODER_ReadReset(1);
        demitour = false;
        sparx.etat = STOP;
        break;
      }
      //Bluetooth manuel
      else if(sensors == BLUETOOTH){
        stop();
        sparx.etat = MANUEL;
        break;
      }
      else
        //ERROR
      break;

    //si l'état est à tourne à droite
    case TOURNE_DROITE:
    
      if(sensors == AUCUN){
        sparx.etat = STOP;
        break;
      }
      //voit un mur à droite
      else if(sensors == SENSOR_IR_DR){
        sparx.etat = STOP;
        break;
      }
      //voit un mur à gauche
      else if(sensors == SENSOR_IR_GA){
        bougerDroite();
        sparx.etat = TOURNE_DROITE;
        break;
      }
      //voit de la lumière en avant
      else if(sensors == SENSOR_LUM_AV){
        sparx.etat = STOP;
        break;
      }
      //voit de la lumière à droite
      else if(sensors == SENSOR_LUM_DR){
        bougerDroite();
        sparx.etat = TOURNE_DROITE;
        break;
      }
      //voit de la lumière à gauche
      else if(sensors == SENSOR_LUM_GA){
        sparx.etat = STOP;
        break;
      }
      //voit de la lumière en arrière
      else if(sensors == SENSOR_LUM_AR){
        sparx.etat = STOP;
        break;
      }
      //2 capteurs de lumière ont la même valeur
      else if(sensors == DOUBLE_LUM){
        sparx.etat = STOP;
        break;
      }
      //le capteur IR avant détecte quelque chose
      else if(sensors == SENSOR_IR_AV){
        sparx.etat = STOP;
        break;
      }
      //Bluetooth manuel
      else if(sensors == BLUETOOTH){
        stop();
        sparx.etat = MANUEL;
        break;
      }
      else
        //ERROR
      break;

    //si l'état est à tourne gauche
    case TOURNE_GAUCHE:
    
      if(sensors == AUCUN){
        sparx.etat = STOP;
        break;
      }
      //voit un mur à droite
      else if(sensors == SENSOR_IR_DR){
        bougerGauche();
        sparx.etat = STOP;
        break;
      }
      //voit un mur à gauche
      else if(sensors == SENSOR_IR_GA){
        sparx.etat = STOP;
        break;
      }
      //voit de la lumière en avant
      else if(sensors == SENSOR_LUM_AV){
        sparx.etat = STOP;
        break;
      }
      //voit de la lumière à droite
      else if(sensors == SENSOR_LUM_DR){
        sparx.etat = STOP;
        break;
      }
      //voit de la lumière à gauche
      else if(sensors == SENSOR_LUM_GA){
        bougerGauche();
        sparx.etat = TOURNE_GAUCHE;
        break;
      }
      //voit de la lumière en arrière
      else if(sensors == SENSOR_LUM_AR){
        sparx.etat = STOP;
        break;
      }
      //2 capteurs de lumière ont la même valeur
      else if(sensors == DOUBLE_LUM){
        sparx.etat = STOP;
        break;
      }
      //sensor IR avant détecte un mur
      else if(sensors == SENSOR_IR_AV){
        sparx.etat = STOP;
        break;
      }
      //Bluetooth manuel
      else if(sensors == BLUETOOTH){
        stop();
        sparx.etat = MANUEL;
        break;
      }
      else
        //ERROR
      break;

       //si l'état est LIFT UP
    case LIFT_UP:
      //Serial.println(sparx.etat);
      //Serial.println("LIFTUP");
      if(millis()-timerlift>1600)
      {
        LiftStop();
        timerrotation=millis();
        sparx.etat = MAINTIENT_POSITION;
        break;
      }
      if(sensors == BLUETOOTH)
      {
        stop();
        sparx.etat = MANUEL;
        break;
      }
      
        //ERROR
      break;
    
      //si l'état est à MAINTIENT Position
    case MAINTIENT_POSITION:
      //2 capteurs de lumière ont la même valeur
      //Serial.println("MAINTIENT");
      if(millis()-timerrotation<5000)
      {
        bougerGauche();
      }
      else if (millis()-timerrotation>5000&&millis()-timerrotation<10000)
      {
        bougerDroite();
      }
      else if(millis()-timerrotation>10000)
      {
        if (sensors == DOUBLE_LUM)
        {
          //Serial.println(millis());
          stop();
          RotationStop();
          timerrotation=millis();
          //Serial.print("Rotation millis(): "),Serial.println(timerrotation);
          break;
          //delay(10000);
        }
        else
        {
          stop();
          timerlift=millis();
          LiftDown();
          sparx.etat = LIFT_DOWN;
          break;
        }
      }
      //Bluetooth manuel
      else if(sensors == BLUETOOTH){
        stop();
        sparx.etat = MANUEL;
        break;
      }
      else{
        //ERROR
      }
      break;
    //si l'état est LIFT DOWN
    case LIFT_DOWN:
      if(millis()-timerlift>900){
        LiftStop();
        sparx.etat = STOP;
        break;
      }
      else if(sensors == BLUETOOTH){
        stop();
        sparx.etat = MANUEL;
        break;
      }
      else
        //ERROR
      break;
    
    case MANUEL:
      //Serial.println("Manuel");
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
void liftup()
{
  analogWrite (enA,135);
  digitalWrite(in1,LOW);
  digitalWrite(in2,HIGH);
}

void LiftDown (){
  analogWrite (enA,130);
  digitalWrite(in1,HIGH);
  digitalWrite(in2,LOW);
}

void LiftStop (){
    //arrete le lift
  digitalWrite(in1, LOW);
	digitalWrite(in2, LOW);
}

void RotationDroite (){
  analogWrite(enB, 130);
  digitalWrite(in3, LOW);
	digitalWrite(in4, HIGH);
}

void RotationGauche(){
  analogWrite(enB, 130);
  digitalWrite(in3, HIGH); //antihoraire
	digitalWrite(in4, LOW);
}

void RotationStop (){

  digitalWrite(in3, LOW);
	digitalWrite(in4, LOW);
}

  void getangle(float angle)
  {
  float coefficient = 360.0 /angle ;
  float circonference = (22.0*PI);
  float distance = (circonference/coefficient);
  float nbtours = (distance/23.93);
  float nbpulses = (nbtours*3200.0);
  //Serial.print("Nb bulses"),Serial.println((int)nbpulses);
  
  int moteur_droit = ENCODER_Read(1);
  //Serial.print("Moteur droit"), Serial.println(moteur_droit);
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
  //Serial.println(millis());
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
uchar* CardRead(){
  uchar status;
  uchar str[MAX_LEN];
  status = MFRC522_Request(PICC_REQIDL, str);
  if (status != MI_OK)
  {
    return 0;
  }
  status = MFRC522_Anticoll(str);
  if (status == MI_OK)
  {
   // Serial.print("The card's number is: ");
    memcpy(serNum, str, 5);
    //ShowCardID(serNum);
    return serNum;
  }
  return 0;
  MFRC522_Halt(); 
}