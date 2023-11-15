#include <Arduino.h>

// put function declarations here:
int myFunction(int, int);

void setup() {
  // put your setup code here, to run once:
  int result = myFunction(2, 3);
}





//detecteur 1 avant gauche, detecteur 2 avant droit, detecteur 3 derriere gauche, detecteur 4 derriere droit, deteceur 5 sur le lift

void loop() 
{
  // put your main code here, to run repeatedly:

  int lumiere_attendue;  // vient du rfid
  int lumiere_sans_rien; //ce que le detecteur de lumiere capte avant le test
  int lumiere_ambiante;

    //stop;

  if(//stop && Detecteur_lumiere!=lumiere_attendue) // lumiere ambiante
  {
  lumiere_sans_rien=DetecteurLumiere1();  // 
  lumiere_ambiante=lumiere_sans_rien+30; // pour que le robot detecte quelque chose, il faut qu'il detecte une valeur plus grande que la lumiere sans rien
  }
  if(DetecteurLumiere1()!=lumiere_attendue || 
     DetecteurLumiere2()!=lumiere_attendue || 
     DetecteurLumiere3()!=lumiere_attendue || 
     DetecteurLumiere4()!=lumiere_attendue)
  {
        //bouge;
  }
  if(DetecteurLumiere1()=lumiere_attendue && 
     DetecteurLumiere2()=lumiere_attendue && 
     DetecteurLumiere3()=lumiere_attendue && 
     DetecteurLumiere4()=lumiere_attendue) 
  {
   // stop;
  }
  else if(DetecteurLumiere1()>lumiere_ambiante &&
          DetecteurLumiere2()>lumiere_ambiante && 
          DetecteurLumiere3()<lumiere_ambiante && 
          DetecteurLumiere4()<lumiere_ambiante)
  {
    //Bouge(distance pour ce mettre dessous la lumiere);
  }
  
  else if(DetecteurLumiere1()>lumiere_ambiante && 
          DetecteurLumiere3()>lumiere_ambiante && 
          DetecteurLumiere2()<lumiere_ambiante && 
          DetecteurLumiere4()<lumiere_ambiante)
  {
    //tourne gauche:
    //bouge(distance pour ce mettre dessous la lumiere)
  }
  else if(DetecteurLumiere2()>lumiere_ambiante && 
          DetecteurLumiere4()>lumiere_ambiante && 
          DetecteurLumiere1()<lumiere_ambiante && 
          DetecteurLumiere2()<lumiere_ambiante)
  {
   // tourne droite:
   // bouge(distance pour ce mettre dessous la lumiere)
  }
   else if(DetecteurLumiere4()>lumiere_ambiante &&
          DetecteurLumiere3()>lumiere_ambiante &&
          DetecteurLumiere1()<lumiere_ambiante && 
          DetecteurLumiere2()<lumiere_ambiante)
  {
    //tourne 180 degre;
   // bouge(distance pour ce mettre dessous la lumiere)
  }
}

// put function definitions here:
int myFunction(int x, int y) {
  return x + y;
}