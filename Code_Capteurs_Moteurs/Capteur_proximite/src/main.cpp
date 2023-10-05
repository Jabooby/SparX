#include <Arduino.h>
#include <LibRobus.h>
//Capteur de proximité


int vertpin = 53;
int rougepin = 49;

void setup(){//fait
  BoardInit();
  
  pinMode(vertpin, INPUT);
  pinMode(rougepin, INPUT);
  delay(100);

}

/*
Fonctions de boucle infini
 -> Se fait appeler perpetuellement suite au "setup"
*/
bool capteur_distance() {
  bool etat;
  bool vert = digitalRead(vertpin);
  bool rouge = digitalRead(rougepin);

  if (vert && rouge){ // aucun obstacle => avance
      etat = 0;
    }
  else {// obstacle==> avance pas
      etat = 1;
    }
  return etat;
}