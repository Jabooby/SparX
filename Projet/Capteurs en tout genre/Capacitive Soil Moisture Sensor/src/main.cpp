#include <Arduino.h>
void calcul();
int gethumid();

void setup() {
  Serial.begin(9600); // open serial port, set the baud rate as 9600 bps
}
void loop() 
{
  calcul();
}


//Valeur à 100% d'humidité = 359
//Valeur à 0% d'humidité = 781
int gethumid()
{
  float val;
  val = analogRead(0); //connect sensor to Analog 0
  return val;
}
void calcul()
{
  float val2;
  val2 = (((gethumid()-781.0)/-422.0)*100);
  Serial.println(val2);

}