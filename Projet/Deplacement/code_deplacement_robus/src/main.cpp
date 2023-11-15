#include <Arduino.h>

// put function declarations here:
void deplacement_lumiere();

void setup() {
  // put your setup code here, to run once:
  int result = myFunction(2, 3);
}





//detecteur 1 avant gauche, detecteur 2 avant droit, detecteur 3 derriere gauche, detecteur 4 derriere droit, deteceur 5 sur le lift

void loop() 
{
  // put your main code here, to run repeatedly:
deplacement_lumiere();

}

// put function definitions here:
void deplacement_lumiere() {
    int lumiere_attendue;  // vient du rfid
  int lumiere_moyenne; //ce que le detecteur de lumiere capte avant le test
  int lumiere_ambiante;
  unsigned long currentMillis = millis();
  unsigned int intervale = 100;
  unsigned long previousMillis = 0;

  lumiere_moyenne=(DetecteurLumiere1() + DetecteurLumiere2() +DetecteurLumiere3() + DetecteurLumiere4())/4;

  if(currentMillis - previousMillis >= intervale) 
  {
    previousMillis = currentMillis;

    if(lumiere_moyenne < lumiere_attendue)
    {
      lumiere_ambiante=lumiere_moyenne;
    }
  }

    //stop;

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