#include <librobus.h>

float vitesse1 = 0.6;
float vitesse0 = vitesse1*allerdroit();
float circonference = 23.9389;

/*int timer;
int interval = 1000;*/

void avance()
{
  MOTOR_SetSpeed(RIGHT,vitesse1);
  MOTOR_SetSpeed(LEFT, vitesse0);
}

int * encodeur()

{
  int x = ENCODER_Read(1); //lecture encodeur droit
  int y = ENCODER_Read(0); //lecture encodeur gauche
  int r[2] = {x, y};
  /*timer = millis()%1001;
  if ( timer == interval )
  {
    Serial.print("x: ");
    Serial.println(x);
    Serial.print("y: ");
    Serial.println(y);
  }*/
  return (r);
}

float allerdroit()
{
  int * r;
  int calcul = r[0]/r[1];
  int facteur = calcul;
  return facteur;
}
  

void setup()
{
  BoardInit();

}

void loop()
{
  int *r;
  avance();
  //encodeur();
}
