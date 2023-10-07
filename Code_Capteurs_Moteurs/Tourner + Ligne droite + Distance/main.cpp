#include <Arduino.h>
#include <LibRobus.h>

int tourval = 3200;
int vertpin = 53;
int rougepin = 49;
bool vert = false;
bool rouge = false;
int etat = 0; // = 0 arrêt 1 = avance 2 = recule 3 = TourneDroit 4 = TourneGauche
int etatPast = 0;
float vitesseLeft = 0.3;
float vitesseRight = 0.3;
float vitesse = 0.3;
bool go = false;
int analog1 = A0;
int analog2 = A1;
bool stop = false;
bool rightangle = false;
int left = 0;
int right = 0;

//215 or 2155 very straight
//float kp = 0.0002156;
float kp = 0.00061;
float ki = 0.000001;
float kd = 0.000016;
float counter = 0;
float errsum = 0.0;
float prevErr = 0.0;
float errdiff = 0;
float desiredsum = 0;
bool status = false;
int timer = 0;
int reset = 0;
int interval = 50;
// put function declarations here:
void arret(){
  MOTOR_SetSpeed(RIGHT, 0);
  MOTOR_SetSpeed(LEFT, 0);
};


void avance(){
  MOTOR_SetSpeed(RIGHT,vitesseRight);
  MOTOR_SetSpeed(LEFT, vitesseLeft);
};

void recule(){
  MOTOR_SetSpeed(RIGHT, -0.5*vitesse);
  MOTOR_SetSpeed(LEFT, -vitesse);
};

void tourneDroit(){
  MOTOR_SetSpeed(RIGHT, -0.5*vitesse);
  MOTOR_SetSpeed(LEFT, 0.5*vitesse);
};

void tourneGauche(){
  MOTOR_SetSpeed(RIGHT, 0.5*vitesse);
  MOTOR_SetSpeed(LEFT, -0.5*vitesse);
};




void getangle(){

left = ENCODER_Read(0);
right = ENCODER_Read(1);

//Serial.print("left: ");
    //Serial.println(left1);
    //Serial.print("right: ");
    //Serial.println(right2);

int moyenne = (abs(left)+abs(right))/2;
float circonference = (18.5*PI);

float distance = (circonference/4.0);
float nbtours = (distance/23.93);
float nbpulses = (nbtours*3200);
//Serial.print("nbpulses: ");
//Serial.println(nbpulses);

if ((abs(right) <= (nbpulses+50)) and (abs(right) >= (nbpulses-50)))
  {
   rightangle = true;
   
  }

};
void PID(){
    left = ENCODER_Read(0);
    right = ENCODER_Read(1);
   /*
    Serial.print("left: ");
    Serial.println(left);
    Serial.print("right: ");
    Serial.println(right);
    */
    float error = abs(right) - abs(left);
    //Serial.print("Error: ");
    //Serial.println(error);
    float proportionalValue = abs(error * kp);

    errsum += error;
    float integralValue = abs(ki * errsum);

    float errdiff = error-prevErr;
    prevErr = error;
    float derivativeValue = abs(kd*errdiff);

    
    
    
    /*
    Serial.print("proportionalValue: ");
    Serial.println(proportionalValue);
    Serial.print("integralValue: ");
    Serial.println(integralValue);
    */
    if (error > 0){
      vitesseLeft = vitesse + proportionalValue + integralValue + derivativeValue;

    } 
    else if (error < 0) {
      vitesseLeft = vitesse - (proportionalValue + integralValue + derivativeValue);
      
    }
    //else vitesse = vitesse;
    else{
      vitesseLeft = vitesseLeft;
      vitesseRight = vitesseRight;
    }
}
void travel(int distance){
  PID();
  float moyenne = (abs(left) + abs(right))/2;
  float circonference = 2*PI*38.1;
  float goalVal = (3200.0f*500.0f)/circonference;
 
    
  if((moyenne == goalVal) or ((moyenne >= goalVal-100) and (moyenne <= goalVal+100))) stop = true;
}
void setup() {
  pinMode(vertpin, INPUT);
  pinMode(rougepin, INPUT);
  //pinMode(analog1, INPUT_PULLUP);
  //pinMode(analog2, INPUT_PULLUP);
  BoardInit();
 // Serial.begin(9600);
  errsum = 0.0;
}

void loop() {
  timer = millis();
 // int f = analogRead(analog1);
 // int g = analogRead(analog2);
  //Serial.println(f);
    //int read = Serial.read();
  vert = digitalRead(vertpin);
  rouge = digitalRead(rougepin);

  //if (f>g) go=true;
  if ((timer-reset) > interval){
    //travel(500);
    getangle();
    
    if(!rightangle){
      tourneDroit(); 
      //avance();
    }
    else{
      //avance();
      arret();
   }
    
    
    //status = true;
    reset = timer;
  }
  else if ((timer-reset) <= interval){

    if(!rightangle){
      //avance();
      tourneDroit(); 
    }
    else{
      arret();
   };
   
    //status = false;
  } 
  
}