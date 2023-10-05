#include <Arduino.h>
#include <LibRobus.h>


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
bool rightangle = false;
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
  MOTOR_SetSpeed(RIGHT, 0.5*vitesse);
  MOTOR_SetSpeed(LEFT, -0.5*vitesse);
};

void tourneGauche(){
  MOTOR_SetSpeed(RIGHT, -0.5*vitesse);
  MOTOR_SetSpeed(LEFT, 0.5*vitesse);
};

void getangle(){

float left1 = ENCODER_Read(0);
float right2 = ENCODER_Read(1);

Serial.print("left: ");
    Serial.println(left1);
    Serial.print("right: ");
    Serial.println(right2);

float circonference = (18.5*PI);

float distance = (circonference/4.0);
float nbtours = (distance/23.93);
float nbpulses = (nbtours*3200);

if ((right2 <= (nbpulses+100)) and (right2 >= (nbpulses-100)))
  {
   Serial.println("turn");
   rightangle = true;
   
  }

};

void PID(){
    float left = ENCODER_Read(0);
    float right = ENCODER_Read(1);
   
    Serial.print("left: ");
    Serial.println(left);
    Serial.print("right: ");
    Serial.println(right);
    
    float error = right - left;
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
    else vitesse = vitesse;
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

  counter++;
  timer = millis();
 // int f = analogRead(analog1);
 // int g = analogRead(analog2);
  //Serial.println(f);
    //int read = Serial.read();
  vert = digitalRead(vertpin);
  rouge = digitalRead(rougepin);

  //if (f>g) go=true;
  if ((timer-reset) > interval){
    PID();
    if(!rightangle){
      tourneDroit(); 
    }
    else{
      avance();
   }
    
    
    //status = true;
    reset = timer;
  }
  else if ((timer-reset) <= interval){
    //avance();
    if(!rightangle){
      tourneDroit(); 
    }
    else{
      avance();
   };
    //status = false;
  } 
  
/*  if ((timer-reset) < interval) avance();
  else if (((timer - reset) == interval) or ((timer-reset) <= interval + 10) or ((timer-reset) >= interval - 10)){
  float left = ENCODER_ReadReset(0);
  float right = ENCODER_ReadReset(1);
  Serial.print("left: ");
  Serial.println(left);
  Serial.print("right: ");
  Serial.println(right);
  float error = right - left;
  float proportionalValue = error * kp;
  if (proportionalValue > 0) vitesseLeft = vitesse + proportionalValue;
  else if (proportionalValue < 0) vitesseRight = vitesse + proportionalValue;
  MOTOR_SetSpeed(RIGHT,vitesseRight);
  MOTOR_SetSpeed(LEFT, vitesseLeft); 
  //reset = timer;
  }
 
  */
  /*if (etat == 0){
  //if (go){
    if (rouge){ // aucun obstacle => avance

      //etat = 0;

      avance();

      
      float x = ENCODER_Read(0);
      x = (x/3200) * (PI * 3);
      float distx = (x * 0.0254) * 1000;

      float y = ENCODER_Read(1);
      y = (y/3200) * (PI * 3);
      float disty = (y * 0.0254) * 1000;

      Serial.print("gauche: ");

      Serial.println(distx);

      Serial.print("droite: ");

      Serial.println(disty);

      delay(500);

    }

    if (!rouge){  // obstacle devant => recule

      //etat = 0;

      arret();

 

    }

    if (!vert && rouge){ // obstacle à gauche => tourne droit

       // etat = 3;

      }

    if (vert && !rouge){ // obstacle à droite => tourne gauche

        //etat = 4;

    }
  }
 // }
 */
}