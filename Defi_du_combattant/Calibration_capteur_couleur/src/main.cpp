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
void calibration(char couleur, int temps);
void getRawData_noDelay(uint16_t* r, uint16_t* g, uint16_t* b, uint16_t* c);

SharpIR irDroite(IR_PIN[IR_DROITE], MODEL_IR);
SharpIR irGauche(IR_PIN[IR_GAUCHE], MODEL_IR);
uint16_t sensorValues[SENSOR_COUNT];
QTRSensors sensor; //classe QTR
QTRReadMode mode = QTRReadMode::On;
Adafruit_TCS34725 tcs = Adafruit_TCS34725(TCS34725_INTEGRATIONTIME_24MS, TCS34725_GAIN_4X);
int nbTour = 1;
char CouleurDebut; //Red, Green, Blue, Clear
bool go = false;
int bumperArr = 0;
int distanceRobotMur = 0;

void setup() {
  // put your setup code here, to run once:
  BoardInit();

  //start de timer
  sparx.startTimer = millis();
  sparx.timerRunning = true;
  sparx.moteurs.vitesse_voulue = 0.3;
  SERVO_Enable(SERVO_1);
  SERVO_SetAngle(SERVO_1, 60); //60 == 90 degré
  sparx.orientation = 90.0;
  delay(1000);
  if (tcs.begin()) 
  {
    Serial.println("Found sensor");
    //CouleurDebut = retour_couleur();
    //Serial.print("R: "), Serial.print(valCouleurDebut[0]), Serial.print(", G: "), Serial.print(valCouleurDebut[1]),
    //Serial.print(", B: "), Serial.print(valCouleurDebut[2]), Serial.print(", Clear: "), Serial.println(valCouleurDebut[3]); 
  }
  sensor.setTypeRC();
  sensor.setSensorPins((const uint8_t[]){38, 39, 40, 41, 42, 43, 44, 45}, SENSOR_COUNT);
  sensor.setEmitterPin(37);
  distanceRobotMur = irDroite.distance();
  //while(start());
}

void loop() {
  //Serial.println(ROBUS_IsBumper(3));
  if(ROBUS_IsBumper(3))
  {
    while(ROBUS_IsBumper(3));

    bumperArr++;
  }
  if(bumperArr == 1)
  {
    calibration('W', 5000);
    bumperArr++;
  }
  if(bumperArr == 3)
  {
    calibration('R', 5000);
    bumperArr++;
  }
  if(bumperArr == 5)
  {
    calibration('J', 5000);
    bumperArr++;
  }
  if(bumperArr == 7)
  {
    calibration('V', 5000);
    bumperArr++;
  }
  if(bumperArr == 9)
  {
    calibration('B', 5000);
    bumperArr++;
  }
  if(bumperArr == 11)
  {
    calibration('T', 5000);
    bumperArr++;
  }
}

void calibration(char couleur, int temps)
{
  Serial.print("Début de calibration de la couleur :"), Serial.println(couleur);
  int tempsRestant = temps;
  uint16_t r, g, b, c = 0;
  uint16_t sR,sG, sB = 0;
  uint32_t sC = 0;
  uint16_t rR, rG, rB = 0;
  uint32_t rC = 0;
  int intervalle = 0;
  int ValeurRmax = 0; 
  int ValeurRmin = 69420; 
  int ValeurGmax = 0;
  int ValeurGmin = 69420;
  int ValeurBmax = 0;
  int ValeurBmin = 69420;
  int ValeurCmax = 0;
  int ValeurCmin = 69420; 

  while(tempsRestant > 0)
  {
    if (sparx.timerRunning && ((millis() - sparx.startTimer) > TIMER_TIME))
    {
      sparx.startTimer += TIMER_TIME; //toujours la premiere chose dans le IF
      tempsRestant -= TIMER_TIME;
      intervalle++;
      getRawData_noDelay(&r,&g,&b,&c);
      sR += r;
      sG += g;
      sB += b;
      sC += c;

    if (r > ValeurRmax)
    {
      ValeurRmax = r;
    }
    if (r < ValeurRmin)
    {
      ValeurRmin = r;
    }
    if (g > ValeurGmax)
    {
      ValeurGmax = g;
    }
    if (g < ValeurGmin)
    {
      ValeurGmin = g;
    }
    if (b > ValeurBmax)
    {
      ValeurBmax = b;
    }
    if (b < ValeurBmin)
    {
      ValeurBmin = b;
    }
    if (c > ValeurCmax)
    {
      ValeurCmax = c;
    }
    if (c < ValeurCmin)
    {
      ValeurCmin = c;
    }
    }
  }
  rR = sR / intervalle;
  rG = sG / intervalle;
  rB = sB / intervalle;
  rC = sC / intervalle;
  Serial.print("Valeur rouge moyenne: "),Serial.println(rR),
  Serial.print("Valeur verte moyenne: "),Serial.println(rG),
  Serial.print("Valeur bleue moyenne: "),Serial.println(rB),
  Serial.print("Valeur clear moyenne: "),Serial.println(rC);
  
  Serial.print("Valeur rouge max: "),Serial.println(ValeurRmax),
  Serial.print("Valeur verte max: "),Serial.println(ValeurGmax),
  Serial.print("Valeur bleue max: "),Serial.println(ValeurBmax),
  Serial.print("Valeur clear max: "),Serial.println(ValeurCmax);
  
  Serial.print("Valeur rouge min: "),Serial.println(ValeurRmin),
  Serial.print("Valeur verte min: "),Serial.println(ValeurGmin),
  Serial.print("Valeur bleue min: "),Serial.println(ValeurBmin),
  Serial.print("Valeur clear min: "),Serial.println(ValeurCmin);
  

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
  int distanceGauche = irGauche.distance();
  int distanceDroite = irDroite.distance();
  //Serial.print("Distance Gauche: "), Serial.println(distanceGauche);
  //Serial.print("Distance Droite: "), Serial.println(distanceDroite);
  if(distanceDroite < distanceMur)
  {
    if(sparx.orientation <= 115.0)
      return 3.0;
    else
      return 0.0;
  }
  if (distanceDroite > (distanceMur + 1))
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
   char couleur ;
   getRawData_noDelay(&r, &g, &b, &c);
  if((r>330)&&(r<410)&&(g>170)&&(g<205)&&(b>200)&&(b<230)&&(c>750)&&(c<900)){
      couleur='R';
    }
  else if((r>140)&&(r<180)&&(g>250)&&(g<310)&&(b>240)&&(b<290)&&(c>700)&&(c<860)){
      couleur='G';
  }
  else if((r<140)&&(g>185)&&(g<280)&&(b>350)&&(b<420)&&(c>800)&&(c<1000)){
      couleur='B';
  }
  else if((r>800)&&(r<980)&&(g>690)&&(g<840)&&(b>370)&&(b<450)&&(c>2000)&&(c<2500)){
      couleur='J';
  }
  else if((r>760)&&(g>800)&&(g<1200)&&(b>840)&&(b<1200)&&(c>2600)){
      couleur='W';
  }
  else{
    couleur='T';
  }
  return couleur;
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