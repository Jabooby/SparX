#include <Arduino.h>
#include <LibRobus.h>
#include <robot_sparX.h>
#include <SharpIR.h>
#include <Wire.h>
#include <Adafruit_TCS34725.h>

#define MODEL_IR 1080
// put function declarations here:
int myFunction(int, int);
float deplacer(float angle);
void acceleration();
float detectionMur();
void couleurStart();
void detectionVerre();

SharpIR irDroite(IR_PIN[IR_DROITE], MODEL_IR);
SharpIR irGauche(IR_PIN[IR_GAUCHE], MODEL_IR);
Adafruit_TCS34725 tcs = Adafruit_TCS34725(TCS34725_INTEGRATIONTIME_50MS, TCS34725_GAIN_1X);
int nbTour = 1;
uint16_t valCouleurDebut[4] = {0,0,0,0}; //Red, Green, Blue, Clear

void setup() {
  // put your setup code here, to run once:
  BoardInit();

  //start de timer
  sparx.startTimer = millis();
  sparx.timerRunning = true;
  sparx.moteurs.vitesse_voulue = 0.3;
  SERVO_Enable(SERVO_1);
  SERVO_SetAngle(SERVO_1, 60);
  delay(1000);
  if (tcs.begin()) 
  {
    Serial.println("Found sensor");
    couleurStart();
    Serial.print("R: "), Serial.print(valCouleurDebut[0]), Serial.print(", G: "), Serial.print(valCouleurDebut[1]),
    Serial.print(", B: "), Serial.print(valCouleurDebut[2]), Serial.print(", Clear: "), Serial.println(valCouleurDebut[3]); 
  }
}

void loop() {
  // timer de 50ms, le code dans le if est executé au 50 ms 
  // changer TIMER_TIMER dans robot_sparX.h pour changer le temps
  if (sparx.timerRunning && ((millis() - sparx.startTimer) > TIMER_TIME))
  {
    sparx.startTimer += TIMER_TIME; //toujours la premiere chose dans le IF
    if (nbTour >= 2)
      sparx.orientation += detectionMur();
    else
    {
      detectionVerre();
    }
    //Serial.print("Orientation: "), Serial.println(sparx.orientation);
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
 * @brief Store les valeurs RGB du début dans le tableau valCouleurDebut
 * @param void
 * @return void
 */
void couleurStart()
{
  getRawData_noDelay(&valCouleurDebut[0], &valCouleurDebut[1], &valCouleurDebut[2],&valCouleurDebut[3]);
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
  if(((v_Droite >=-0.001) && (v_Droite <=0.001)) || ((v_Gauche >=-0.001) && (v_Gauche <=0.001)))
  {
    sparx.moteurs.vitesse_moteur_droite = v_Droite;
    sparx.moteurs.vitesse_moteur_gauche = v_Gauche;
  }
  else
    PID(v_Gauche, v_Droite); 
  //Serial.println(sparx.moteurs.vitesse_moteur_gauche);
  MOTOR_SetSpeed(RIGHT,sparx.moteurs.vitesse_moteur_droite);
  MOTOR_SetSpeed(LEFT, sparx.moteurs.vitesse_moteur_gauche);
  //calcul radius
  radius = (distance_entre_roues_m/2)*((sparx.moteurs.vitesse_moteur_droite + sparx.moteurs.vitesse_moteur_gauche)/(sparx.moteurs.vitesse_moteur_droite - sparx.moteurs.vitesse_moteur_gauche));
  ENCODER_Reset(0);
  ENCODER_Reset(1);
  return(radius);
}

float detectionMur()
{
  int distanceGauche = irGauche.distance();
  int distanceDroite = irDroite.distance();
  //Serial.print("Distance Gauche: "), Serial.println(distanceGauche);
  //Serial.print("Distance Droite: "), Serial.println(distanceDroite);
  if(distanceDroite < 8)
  {
    if(sparx.orientation <= 115.0)
      return 3.0;
    else
      return 0.0;
  }
  if (distanceDroite > 9)
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
    while(getDistance() < 40.0);
      //Serial.println(getDistance());
    SERVO_SetAngle(SERVO_1, 60);
  }
  if(distanceDroite < 10)
  {
    while(getDistance() < 14.0);
    SERVO_SetAngle(SERVO_1, 130);
    while(getDistance() < 40.0);
    SERVO_SetAngle(SERVO_1, 60);
  }
  else
  {
    SERVO_SetAngle(SERVO_1, 60);
  }

}

