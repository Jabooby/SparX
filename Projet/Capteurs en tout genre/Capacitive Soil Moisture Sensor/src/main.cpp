#include <Arduino.h>
/*Prototype des fonctions utilisées*/
float calcul();
float gethumid();
void type();

void setup() {
  Serial.begin(9600); // open serial port, set the baud rate as 9600 bps
}
void loop() 
{
  type();
}

float gethumid() //Permet d'avoir le "Raw Data" du détecteur d'humidité
{
  float val;
  val = analogRead(0); //connect sensor to Analog 0
  return val;
}
float calcul() //Transforme le "Raw Data en pourcentage"
{
  float val2;
  val2 = (((gethumid()-781.0)/-422.0)*100);
  return val2;

}

void type() //Sujet à changement, à lier avec le RFID pour connaître ce que chaque type de plante à besoin.
{
  float etat;
  etat = calcul();

  if (etat<35.0)
  {
    Serial.println("J'ai besoin d'être arrosée!");
  }
  if (etat>35.0&&etat<75.0)
  {
    Serial.println("Je suis correcte en eau!");
  }
  if (etat > 75.0)
  {
    Serial.println("Tu me noyes! ");
  }
}
