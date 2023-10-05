#include <Arduino.h>
#include <LibRobus.h>
//variables de vitesse
float vitesseLeft = 0.3;
float vitesseRight = 0.3;
float vitesse = 0.3;
//constantes PID
float kp = 0.00;
float ki = 0.0;
float kd = 0.0;
//variables de calcul PID
float counter = 0;
float errsum  = 0.0;
float prevErr = 0.0;
float errdiff = 0;
//variables timer
int timer = 0;
int reset = 0;
int interval = 50; //50 mS

void arret(){
  MOTOR_SetSpeed(RIGHT, 0);
  MOTOR_SetSpeed(LEFT, 0);
};
void avance(){
  MOTOR_SetSpeed(RIGHT,vitesseRight);
  MOTOR_SetSpeed(LEFT, vitesseLeft);
};

void recule(){
  MOTOR_SetSpeed(RIGHT, -0.5*vitesseRight);
  MOTOR_SetSpeed(LEFT, -vitesseLeft);
};

void tourneDroit(){
  MOTOR_SetSpeed(RIGHT, 0.5*vitesseRight);
  MOTOR_SetSpeed(LEFT, -0.5*vitesseLeft);
};

void tourneGauche(){
  MOTOR_SetSpeed(RIGHT, -0.5*vitesseRight);
  MOTOR_SetSpeed(LEFT, 0.5*vitesseLeft);
};

void PID(){
    //lire les valeurs des encodeurs
    float left = ENCODER_Read(0);
    float right = ENCODER_Read(1);
    //calcul d'erreur
    float error = right - left;
    //calcul propotionelle
    float proportionalValue = error * kp;
    //calcul intégrale
    errsum += error;
    float integralValue = ki * errsum;
    //calcul dérivé
    float errdiff = error-prevErr;
    prevErr = error;
    float derivativeValue = kd*errdiff;
    //si la moteur gauche est plus lente
    if (error > 0){
      vitesseLeft = vitesse + proportionalValue + integralValue + derivativeValue;

    } 
    //si la moteur gauche est plus vite
    else if (error < 0) {
      vitesseLeft = vitesse - (proportionalValue + integralValue + derivativeValue);
      
    }
    else vitesse = vitesse;
}

void setup() {
  //initialisation
  BoardInit();
  errsum = 0.0;
  
}

void loop() {
  //compteur de chaque itération du loop
  counter++;
  //lecture du clock
  timer = millis();
  //calcul des données après chaque 50 mS
  if ((timer-reset) > interval){
    PID(); //calculs PID
    avance();
    reset = timer; //recommencer la clock
  }
  //Capture de données pendant 50 mS
  else if ((timer-reset) <= interval){
    avance();
  }
}

