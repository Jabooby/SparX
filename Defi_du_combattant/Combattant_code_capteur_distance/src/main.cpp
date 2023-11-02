//capteur de son A14 = intensite = orange A15 = ambient = brun
#include <Arduino.h>
#include <LibRobus.h>
#include <robot_sparX.h>
#include <SharpIR.h>
#include <Wire.h>
#include <Adafruit_TCS34725.h>
#include <QTRSensors.h>

#define MODEL_IR 1080
#define SENSOR_COUNT 8
// put function declarations here:
int myFunction(int, int);
float deplacer(float angle);
void acceleration();
float detectionMur(int distanceMur);
void couleurStart();
void detectionVerre();
float followV2();
char retour_couleur();
bool start();

SharpIR irDroite(IR_PIN[IR_DROITE], MODEL_IR);
SharpIR irGauche(IR_PIN[IR_GAUCHE], MODEL_IR);
uint16_t sensorValues[SENSOR_COUNT];
QTRSensors sensor; //classe QTR
QTRReadMode mode = QTRReadMode::On;
Adafruit_TCS34725 tcs = Adafruit_TCS34725(TCS34725_INTEGRATIONTIME_24MS, TCS34725_GAIN_4X);
int nbTour = 1;
char CouleurDebut; //Red, Green, Blue, Clear
bool go = false;
int distanceRobotMur1 = 0;
int distanceRobotMur2 = 8;

void setup() {
  // put your setup code here, to run once:
  BoardInit();

  //start de timer
  sparx.startTimer = millis();
  sparx.timerRunning = true;
  SERVO_Enable(SERVO_1);
  //SERVO_SetAngle(SERVO_1, 60); //60 == 90 degré
  sparx.orientation = 90.0;
  delay(1000);
  if (tcs.begin()) 
  {
    //Serial.println("Found sensor");
    CouleurDebut = retour_couleur();
    //Serial.print("R: "), Serial.print(valCouleurDebut[0]), Serial.print(", G: "), Serial.print(valCouleurDebut[1]),
    //Serial.print(", B: "), Serial.print(valCouleurDebut[2]), Serial.print(", Clear: "), Serial.println(valCouleurDebut[3]); 
  }
  retour_couleur();
  retour_couleur();
  retour_couleur();
  retour_couleur();
  sensor.setTypeRC();
  sensor.setSensorPins((const uint8_t[]){38, 39, 40, 41, 42, 43, 44, 45}, SENSOR_COUNT);
  sensor.setEmitterPin(37);
  distanceRobotMur1 = irDroite.distance();
  if (distanceRobotMur1 > 55) //jaune
  {
    distanceRobotMur1 = 60;
    sparx.moteurs.vitesse_voulue = 0.44;
  }
  else //vert 
  {
    sparx.moteurs.vitesse_voulue = 0.3;
    distanceRobotMur1 = 31;
  }
  //while(start());
}

void loop() {
  // timer de 50ms, le code dans le if est executé au 24 ms 
  // changer TIMER_TIMER dans robot_sparX.h pour changer le temps
  if (sparx.timerRunning && ((millis() - sparx.startTimer) > TIMER_TIME))
  {
    char couleur = retour_couleur();
    Serial.println(couleur);
    sparx.startTimer += TIMER_TIME; //toujours la premiere chose dans le IF
    if (nbTour >= 2)
    {
      sparx.moteurs.vitesse_voulue = 0.3;
      if((distanceRobotMur1 > distanceRobotMur2) && (millis()%240 < 24))
        distanceRobotMur1--;
      sparx.orientation += detectionMur(distanceRobotMur1); //shortcut no touchy //8cm il fait le shortcut
    }
    else
    {
      if(couleur == 'W')
      {
        sparx.orientation += followV2();
        sparx.moteurs.vitesse_voulue = 0.18;
      }        
      else
      {
        detectionVerre();
        if (millis() > 14000)
        {
          sparx.orientation = 90.0;
        }
        else
          sparx.orientation += detectionMur(distanceRobotMur1);
      }
    }
    //Serial.print("Orientation: "), Serial.println(sparx.orientation);
    //sparx.orientation == 90 go straight
    deplacer(sparx.orientation); //toujours la dernière chose à faire dans le IF
    
  }

}

/*
 * @brief Lit les valeur reçues du capteur en I2C, on vient mettre ses valeur dans une variable grâce à un pointeur
 * @param r: pointeur pour le rouge
 * @param g: pointeur pour le vert
 * @param b: pointeur pour le bleu
 * @param c: pointeur pour le clear
 * @return float: retourne le radius
 */
void getRawData_noDelay(uint16_t* r, uint16_t* g, uint16_t* b, uint16_t* c) {
    *c = tcs.read16(TCS34725_CDATAL);
    *r = tcs.read16(TCS34725_RDATAL);
    *g = tcs.read16(TCS34725_GDATAL);
    *b = tcs.read16(TCS34725_BDATAL);
}

/*
 * @brief déplace selon l'angle voulue. 0 est à droite, 180 à gauche, 90 tout droit
 * @param angle : <90 droite (horaire), >90 gauche (antihoraire)
 * @return float: retourne le radius
 */
float deplacer(float angle)
{
  float radius = 0;
  float radians = DEG_TO_RAD *(angle+45);
  acceleration();
  //voir excel pour les formules
  float v_Droite = -(sparx.moteurs.vitesse_moment*cos(radians))/COS_45_RAD;
  float v_Gauche = (sparx.moteurs.vitesse_moment*sin(radians))/COS_45_RAD;
  if(((v_Droite >= v_Gauche - 0.005) && (v_Droite <= v_Gauche +0.005)))
  {
    PID(v_Gauche, v_Droite);
  }
  else
  {
    sparx.moteurs.vitesse_moteur_droite = v_Droite;
    sparx.moteurs.vitesse_moteur_gauche = v_Gauche;
  }
  //Serial.println(sparx.moteurs.vitesse_moteur_gauche);
  MOTOR_SetSpeed(RIGHT,sparx.moteurs.vitesse_moteur_droite);
  MOTOR_SetSpeed(LEFT, sparx.moteurs.vitesse_moteur_gauche);
  //calcul radius
  radius = (distance_entre_roues_m/2)*((sparx.moteurs.vitesse_moteur_droite + sparx.moteurs.vitesse_moteur_gauche)/(sparx.moteurs.vitesse_moteur_droite - sparx.moteurs.vitesse_moteur_gauche));
  ENCODER_Reset(0);
  ENCODER_Reset(1);
  return(radius);
}

float detectionMur(int distanceMur)
{
  //int distanceGauche = irGauche.distance();
  int distanceDroite = irDroite.distance() + 1;
  int buffer = distanceMur / 10;
  //Serial.print("Distance Gauche: "), Serial.println(distanceGauche);
  //Serial.print("Distance Droite: "), Serial.println(distanceDroite);
  if(distanceDroite < distanceMur)
  {
    if(sparx.orientation <= 115.0)
      return 3.0;
    else
      return 0.0;
  }
  if (distanceDroite > (distanceMur + buffer + 1))
  {
    if(sparx.orientation >= 65.0)
      return -3.0;
    else
      return 0.0;
  }
  else
  {
    sparx.orientation = 90.0;
    return 0.0;
  }
}

/*
 * @brief fait une accélération de la vitesse et déceleration dépendamment de valeur voulue
 * @param rien
 * @return 
 */
void acceleration()
{
  //augmente la vitesse
  if(sparx.moteurs.vitesse_moment < sparx.moteurs.vitesse_voulue)
    sparx.moteurs.vitesse_moment += 0.02;
  //diminue la vitesse
  else if (sparx.moteurs.vitesse_moment > sparx.moteurs.vitesse_voulue)
    sparx.moteurs.vitesse_moment -= 0.02;
  // reste pareille
  else
    sparx.moteurs.vitesse_moment = sparx.moteurs.vitesse_voulue;
}

// put function definitions here:
int myFunction(int x, int y) {
  return x + y;
}

void detectionVerre()
{
  int distanceGauche = irGauche.distance();
  int distanceDroite = irDroite.distance();
  if(distanceGauche < 10)
  {
    while(getDistance() < 14.0);
    SERVO_SetAngle(SERVO_1, 0);
    while(retour_couleur() != 'W');
      //Serial.println(getDistance());
    SERVO_SetAngle(SERVO_1, 60);
  }
  if(distanceDroite < 10)
  {
    while(getDistance() < 14.0);
    SERVO_SetAngle(SERVO_1, 130);
    while(retour_couleur() != 'W');

    SERVO_SetAngle(SERVO_1, 60);
  }
  else
  {
    SERVO_SetAngle(SERVO_1, 60);
  }
}

float followV2()
{
  int position = 0;
  int error = 0;
  sensor.read(sensorValues, mode);
  for (uint8_t i = 0; i < SENSOR_COUNT; i++)
  {
    if(sensorValues[i] > 2000)
    {
      position = 1000 * i;
      break;
    }
    else
    {
      position = 8000;
    }
  }
  error = position - 3500; //si capteur 3 ou 4 on va tout droit
  if((error < 1000) && (error > -1000))
  {
    sparx.orientation = 90.0;
    return 0.0;
  }
  else if (position == 8000 && CouleurDebut == 'J') //8000 == dans le blanc, aucune ligne
  {
    sparx.orientation = 75.0;
    return 0.0;
  }
  else if (position == 8000 && CouleurDebut == 'V') //8000 == dans le blanc, aucune ligne
  {
    sparx.orientation = 115.0;
    return 0.0;
  }
  else
  {
    if ((sparx.orientation < 130) && (sparx.orientation > 50)) //limite les angles
      return (error / 250);
    else
      return 0.0;
  }
}

//Renvoie la couleur du sol
char retour_couleur() {
   uint16_t r, g , b, c;
   char couleur;
   getRawData_noDelay(&r, &g, &b, &c);
  if((r>60-10)&&(r<80+10)&&(g>50-10)&&(g<60+10)&&(b>55-10)&&(b<70+10)&&(c>190-10)&&(c<225+10)){
      couleur='R';
    }
  else if((r>35-10)&&(r<50+10)&&(g>60-10)&&(g<75+10)&&(b>60-10)&&(b<75+10)&&(c>190-10)&&(c<210+10)){
      couleur='G';
  }
  else if((r>30)&&(r<45+10)&&(g>55-10)&&(g<70+10)&&(b>75-10)&&(b<90+10)&&(c>190)&&(c<210+20)){
      couleur='B';
  }
  else if((r>130-5)&&(r<155+30)&&(g>120)&&(g<145+20)&&(b>80-5)&&(b<90+15)&&(c>375-10)&&(c<500)){
      couleur='J';
  }
  else if((r>160)&&(r<195+10)&&(g>170)&&(g<205+10)&&(b>200-10)&&(b<210+10)&&(c>600-10)){
      couleur='W';
  }
  else if ((r>55)&&(r<75)&&(g>65)&&(g<90)&&(b>75)&&(b<95)&&(c>230)&&(c<270))
  {
    couleur='T';
  }
  else
    couleur = 'N';
  Serial.print(r), Serial.print(' '), Serial.print(g), Serial.print(' '), Serial.print(b),Serial.print(' '), Serial.println(c);
  return couleur;
}

void dropcup()// nous devons initialiser le servo
{
  SERVO_SetAngle(SERVO_2, 25);//choisis l'angle du servo moteur
  delay(500);
  SERVO_SetAngle(SERVO_2, 69);//Remet l'angle de base du servo moteur
  delay(500);
  SERVO_SetAngle(SERVO_2, 25);//choisis l'angle du servo moteur
  delay(500);
  delay(1000);
  MOTOR_SetSpeed(RIGHT, 0.4);
  delay(800);
  MOTOR_SetSpeed(RIGHT, 0);
  delay(500);

  MOTOR_SetSpeed(RIGHT, -0.4);
  delay(800);
  MOTOR_SetSpeed(RIGHT, 0);
  delay(1000);
}

//ne marche pas
bool start() {
  //bool go = false;
  int frequency = analogRead(A7);
  int ambiant = analogRead(A6);
  
  Serial.println(frequency);
  Serial.println(ambiant);

  if (frequency > 800){
    go=true;
  } 
  else go = false;
  return go;
}