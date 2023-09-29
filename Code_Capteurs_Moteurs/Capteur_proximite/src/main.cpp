#include <Arduino.h>
#include <LibRobus.h>
//Capteur de proximité


int vertpin = 53;
int rougepin = 49;
bool vert = false;
bool rouge = false;
int etat = 0; // = 0 arrêt 1 = avance



void setup(){
  BoardInit();
  
  pinMode(LED_BUILTIN,OUTPUT);
  //initialisation
  pinMode(vertpin, INPUT);
  pinMode(rougepin, INPUT);
  delay(100);

}

/*
Fonctions de boucle infini
 -> Se fait appeler perpetuellement suite au "setup"
*/
void loop() {
  vert = digitalRead(vertpin);
  rouge = digitalRead(rougepin);
  if (etat > 0){
    if (vert && rouge){ // aucun obstacle => avance
      etat = 1;
      printf("pas d'objet en avant");
      digitalWrite(LED_BUILTIN,LOW);
    }
    else {// obstacle==> avance pas
      etat = 0;
      printf("objet en avant");

    }
   
  }
  delay(200);
}