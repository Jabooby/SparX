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
void getRawData_temp(uint16_t* r, uint16_t* g, uint16_t* b, uint16_t* c);

SharpIR irDroite(IR_PIN[IR_DROITE], MODEL_IR);
SharpIR irGauche(IR_PIN[IR_GAUCHE], MODEL_IR);
uint16_t sensorValues[SENSOR_COUNT];
QTRSensors sensor; //classe QTR
QTRReadMode mode = QTRReadMode::On;
Adafruit_TCS34725 tcs = Adafruit_TCS34725(TCS34725_INTEGRATIONTIME_24MS, TCS34725_GAIN_60X);
int nbTour = 1;
int temp = 0;
char CouleurDebut; //Red, Green, Blue, Clear
bool go = false;
int distanceRobotMur1 = 0;
int distanceRobotMur2 = 0;
int distanceDepart = 0;
double depart = 0;

void setup() {
  // put your setup code here, to run once:
  BoardInit();

  //start de timer
  sparx.startTimer = millis();
  sparx.timerRunning = true;
  SERVO_Enable(SERVO_1);
  SERVO_Enable(SERVO_2);
  SERVO_SetAngle(SERVO_2, 60);
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
  //Serial.println(distanceRobotMur1);
  if (distanceRobotMur1 > 55) //jaune
  {
    distanceRobotMur1 = 60;
    distanceDepart = 60;
    sparx.moteurs.vitesse_voulue = 0.44;
  }
  else //vert 
  {
    sparx.moteurs.vitesse_voulue = 0.3;
    distanceRobotMur1 = 30; // don't touch
    distanceDepart = 30;
  }
  while(!start());
  
  depart = millis();
}

void loop() {
  // timer de 50ms, le code dans le if est executé au 24 ms 
  // changer TIMER_TIMER dans robot_sparX.h pour changer le temps
  if (sparx.timerRunning && ((millis() - sparx.startTimer) > TIMER_TIME))
  {
    //char couleur = retour_couleur();
    //Serial.println(couleur);
    uint16_t r, g , b, c;
    getRawData_temp(&r ,&g, &b, &c);
    temp = tcs.calculateColorTemperature(r,g,b);
    Serial.println(temp);
    sparx.startTimer += TIMER_TIME; //toujours la premiere chose dans le IF
    if (distanceDepart > 45)
    {
      if ((millis()-depart) > 9200 && (millis()-depart) < 12200)
        {
          detectionVerre();
        }
       if ((millis()-depart) > 12200 && (millis()-depart) < 16500)
        {
          detectionVerre();
          //sparx.moteurs.vitesse_voulue = 0.3;
          sparx.orientation = 90.3;
        }
        else if ((millis()-depart) > 16500 && (millis()-depart) < 17300)
        {
          sparx.orientation = 65.0;
          /*sparx.moteurs.vitesse_voulue = 0.4;
          distanceRobotMur1 = 35;
          sparx.orientation += detectionMur(distanceRobotMur1);
          SERVO_SetAngle(SERVO_1, 60);*/
        }
        else if ((millis()-depart) > 17300 && (millis()-depart) < 19100)
        {
          sparx.orientation = 90.0;
          SERVO_SetAngle(SERVO_1, 60);
        }
        else if ((millis()-depart) > 19100 && (millis()-depart) < 23650)
        {
          sparx.moteurs.vitesse_voulue = 0.4;
          distanceRobotMur1 = 35;
          sparx.orientation += detectionMur(distanceRobotMur1);
          SERVO_SetAngle(SERVO_1, 60);
        }
        else if ((millis()-depart) > 23650 && (millis()-depart) < 30800)
        {
          sparx.moteurs.vitesse_voulue = 0.3;
          sparx.orientation = 88.4;
          SERVO_SetAngle(SERVO_2, 150);
          distanceRobotMur2 = irDroite.distance();
        }
        else if ((millis()-depart) > 30800 || distanceRobotMur2 == 10)
        {
          SERVO_SetAngle(SERVO_2, 150);
          distanceRobotMur1 = 10;
          sparx.orientation += detectionMur(distanceRobotMur1);
        }
        else
          sparx.orientation += detectionMur(distanceRobotMur1);
    }
    else
    {
      //if(couleur == 'W')
      /*if((temp < 10500) && (temp > 8200))
      {
        distanceRobotMur1 = 54;
        sparx.orientation += detectionMur(distanceRobotMur1);
        sparx.moteurs.vitesse_voulue = 0.5;
        SERVO_SetAngle(SERVO_1, 60);
      }      */  
      
        
        if ((millis()-depart) > 12000 && (millis()-depart) < 18000)
        {
          detectionVerre();
          //sparx.moteurs.vitesse_voulue = 0.3;
          sparx.orientation = 89.8;
        }
        else if ((millis()-depart) > 18000 && (millis()-depart) < 25400)
        {
          sparx.moteurs.vitesse_voulue = 0.4;
          distanceRobotMur1 = 35;
          sparx.orientation += detectionMur(distanceRobotMur1);
          SERVO_SetAngle(SERVO_1, 60);
        }
        else if ((millis()-depart) > 25400 && (millis()-depart) < 30250)
        {
          sparx.moteurs.vitesse_voulue = 0.3;
          sparx.orientation = 88.2;
          SERVO_SetAngle(SERVO_1, 60);
          SERVO_SetAngle(SERVO_2, 150);
          distanceRobotMur2 = irDroite.distance();
        }
        else if ((millis()-depart) > 30250 || distanceRobotMur2 == 10)
        {
          SERVO_SetAngle(SERVO_2, 150);
          distanceRobotMur1 = 10;
          sparx.orientation += detectionMur(distanceRobotMur1);
        }
        else
          sparx.orientation += detectionMur(distanceRobotMur1);
      

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
  //Serial.print("Distance: "), Serial.println(distanceDroite);
  int buffer = distanceMur / 10;
  //Serial.print("Distance Gauche: "), Serial.println(distanceGauche);
  //Serial.print("Distance Droite: "), Serial.println(distanceDroite);
  if(distanceDroite < distanceMur)
  {
    if(sparx.orientation <= 105.0)
      return 3.0;
    else
      return 0.0;
  }
  if (distanceDroite > (distanceMur + buffer))
  {
    if(sparx.orientation >= 69.0)
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
    
    SERVO_SetAngle(SERVO_1, 0);
    if((temp < 10500) && (temp > 8200))
      {
        SERVO_SetAngle(SERVO_1, 60);
      }
  }
  if(distanceDroite < 10)
  {
    SERVO_SetAngle(SERVO_1, 130);
    if((temp < 10500) && (temp > 8200))
    {
      SERVO_SetAngle(SERVO_1, 60);
    }
  }
}

float followV2()
{
  int position = 0;
  int error = 0;
  sensor.read(sensorValues, mode);
  for (uint8_t i = 0; i < SENSOR_COUNT; i++)
  {
    //Serial.print("Position "), Serial.print(i),Serial.print(" : "), Serial.println(sensorValues[i]);
    if(sensorValues[i] > 2200)
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
  if((temp > 11000) || (temp < 9700))
  {
    //nbTour++;
  }
  //Serial.print("Position: "), Serial.println(position);
  if((error < 1000) && (error > -1000))
  {
    sparx.orientation = 90.0;
    return 0.0;
  }
  else if (position == 8000 && distanceRobotMur1 > 45 ) //8000 == dans le blanc, aucune ligne
  {
    sparx.orientation = 80.0;
    return 0.0;
  }
  else if (position == 8000 && distanceRobotMur1 < 45) //8000 == dans le blanc, aucune ligne
  {
    sparx.orientation = 100.0;
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
  //Serial.print(r), Serial.print(' '), Serial.print(g), Serial.print(' '), Serial.print(b),Serial.print(' '), Serial.println(c);
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

  if (frequency > 500){
    go=true;
  } 
  else go = false;
  return go;
}

void getRawData_temp(uint16_t* r, uint16_t* g, uint16_t* b, uint16_t* c) {
   // *c = tcs.read16(TCS34725_CDATAL);
   // *r = tcs.read16(TCS34725_RDATAL);
   // *g = tcs.read16(TCS34725_GDATAL);
   // *b = tcs.read16(TCS34725_BDATAH);
    tcs.getRawData(r,g,b,c);
}