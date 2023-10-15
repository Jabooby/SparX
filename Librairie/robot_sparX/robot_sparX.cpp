/**
 * @file   robot_sparX.cpp
 * @version V1_0
 * @author P-29 SparX
 * @date  début 28 septembre 2023 - 14 octobre 2023
 * @brief    
 * Librairie des fonctions utilisées lors du défi du parcours. S'il vous plaît voir les commentaire plus bas pour comprendre leurs 
 * fonctionnement.
 */


/************************* INCLUDES *************************/
#include <robot_sparX.h>


/************************* VARIABLES GLOBALES *************************/
struct robot sparx; //création de la valeur global SparX. SparX est le robot et nous pouvons accéder
// les différentes fonctions du robot. Pour accéder le moteur gauche juste faire "Sparx.vitesse_moteur_gauche"


/************************* FONCTIONS *************************/

////////////////////////// FOCNTIONS MOUVEMENTS //////////////////////////
/*
 * @brief Fonction qui permet de tourner le robot en mode pivot. (voir: https://docs.idew.org/code-robotics/references/robot-behaviors/turning)
 * @param angle: si l'angle est positif on tourne à droite , sinon à gauche. Angle définie l'angle que le robot doit tourner;
 * @return void
 */
void tourner(float angle)
{
  //si angle plus grand que 0
  if (angle > 0.0)
  {
    //tand que on est pas arrivé à l'angle voulue
    while(!(getAngle(angle)))
    {
      PID(); //PID pour savoir si les 2 moteurs ont un vitesse angulaire égal
      actionDroite(); //tourne à droite
    }
    ENCODER_ReadReset(LEFT); //reset encodeur du moteur gauch
    ENCODER_ReadReset(RIGHT); //reset encodeur du moteur droit
    arret(); //force le robot à faire un stop 
  }
  //si l'angle ets plus petit que 0 (négatif)
  else
  {
    //tand que on est pas arrivé à l'angle voulue
    while(!(getAngle(angle)))
    {
      PID(); //PID pour savoir si les 2 moteurs ont un vitesse angulaire égal
      actionGauche(); //tourne à gauche
    }
    ENCODER_ReadReset(LEFT); //reset encodeur du moteur gauche
    ENCODER_ReadReset(RIGHT); //reset encodeur du moteur droit
    arret(); //force le robot à faire un stop 
  }
}

/*
 * @brief Fonction permet de changer la vitesse des moteurs selon la méthode PID (mettre lien ici plus tard avec explication).
 * La fonction change juste la vitesse du moteur gauche, la vitesse du moteur droit reste stable. Autrement dit, 
 * le moteur gauche est Follower (dépendant du Leader) et le moteur droit est Leader (influence le follower).
 * 
 * @param rien
 * @return rien
 */
void PID(){
    //lire les valeurs des encodeurs
    sparx.moteurs.encodeurGauche = ENCODER_Read(0);
    sparx.moteurs.encodeurDroite = ENCODER_Read(1);
    //calcul d'erreur
    float error = abs(sparx.moteurs.encodeurDroite) - abs(sparx.moteurs.encodeurGauche);
    //calcul propotionelle
    float proportionalValue = abs(error * sparx.pid.kp);
    //calcul intégrale
    sparx.pid.errsum += error;
    float integralValue = abs(sparx.pid.ki * sparx.pid.errsum);
    //calcul dérivé
    float errdiff = error-sparx.pid.prevErr;
    sparx.pid.prevErr = error;
    float derivativeValue = abs(sparx.pid.kd*errdiff);
    //si la moteur gauche est plus lente
    if (error > 0){
      sparx.moteurs.vitesse_moteur_gauche = sparx.moteurs.vitesse + proportionalValue + integralValue + derivativeValue;

    } 
    //si la moteur gauche est plus vite
    else if (error < 0) {
      sparx.moteurs.vitesse_moteur_gauche = sparx.moteurs.vitesse - (proportionalValue + integralValue + derivativeValue);
      
    }
    else
    {
      //sparx.moteurs.vitesse = sparx.moteurs.vitesse;
      sparx.moteurs.vitesse_moteur_gauche = sparx.moteurs.vitesse;
      sparx.moteurs.vitesse_moteur_droite = sparx.moteurs.vitesse;
    }
    
}
/*
 * @brief Fonction dit au moteur de tourner à droite.
 * @param rien
 * @return rien
 */
void actionDroite(){
  MOTOR_SetSpeed(RIGHT, -0.5*sparx.moteurs.vitesse_moteur_droite); //moteur droit, on réduit la vitesse pour ne pas dérapper
  MOTOR_SetSpeed(LEFT, 0.5*sparx.moteurs.vitesse_moteur_gauche); //moteur gauche, on réduit la vitesse pour ne pas dérapper
};

/*
 * @brief Fonction dit au moteur de tourner à gauche.
 * @param rien
 * @return rien
 */
void actionGauche(){
  MOTOR_SetSpeed(RIGHT, 0.5*sparx.moteurs.vitesse_moteur_droite); //moteur droit, on réduit la vitesse pour ne pas dérapper
  MOTOR_SetSpeed(LEFT, -0.5*sparx.moteurs.vitesse_moteur_gauche); //moteur gauche, on réduit la vitesse pour ne pas dérapper
};

/*
 * @brief Fonction dit au moteur d'avancer en ligne droite selon une distance donnée en cm.
 * @param ditance: distance en cm que nous voulons avancer en ligne droite.
 * @return rien
 */
void avance(float distance){
  PID(); //fait une premièere itération de PID
  while(getDistance() < distance) //continue si la distance parcourue est plus petite que celle voulue
  {
    PID(); //on refait un PID àa chaque boucle While
    //on donne les nouvelles vitesses calculer grâce à la fonction PID aux moteurs
    MOTOR_SetSpeed(RIGHT,sparx.moteurs.vitesse_moteur_droite); 
    MOTOR_SetSpeed(LEFT, sparx.moteurs.vitesse_moteur_gauche);
    
  }
  arret(); //il va falloir changer ça ici
};

/*
 * @brief Fonction dit au moteur d'arrêter, on fait un reset des deux encodeurs ici aussi.
 * @param rien
 * @return rien
 */
void arret(){
  //arrêt des moteurs
  MOTOR_SetSpeed(RIGHT, 0);
  MOTOR_SetSpeed(LEFT, 0);
  //Reset des encodeurs
  ENCODER_Reset(0);
  ENCODER_Reset(1);
};



////////////////////////// FOCNTIONS CAPTEURS //////////////////////////
/*
 * @brief Fonction permet de coir l'état du capteur infrarouge 
 * @param rien
 * @return bool: retourne true si il a un obstacle, flase si il en a pas.
 */
bool lectureInfrarouge() {
  bool vert = digitalRead(sparx.capteurs.infrarougePin[0]); //lecture de la pin verte
  bool rouge = digitalRead(sparx.capteurs.infrarougePin[1]); //lecture de la pin rouge
  if (vert && rouge) //si la pin verte et rouge sont à un état haut (aucun objet)
      return false; //retourne haut
  else // sinon
      return true; //retourne vrai
}

/*
 * @brief Fonction permet de vérifier si le robot est arrrivé à l'angle voulue
 * @param angle: l'angle que l'on désire savoir si notre robot l'a atteint;
 * @return bool: retourne true si l'angle est atteint, flase si non
 */
bool getAngle(float angle){
  float coefficient = 360.0 / angle ; //coefficient utiliser dans le calcul, 360/90, on doit divisr par 4
  float circonference = (22.0*PI); //calcul de la circonférence de rotation (à changer/calculer (voir: https://en.wikipedia.org/wiki/Differential_wheeled_robot ))
  float distance = (circonference/coefficient); //calcul la distance qu'une roue doit parcourir
  //!!!!pourrait utiliser getDistance ici!!!!!
  float nbtours = (distance/23.93); //convertie la distance en nombre de tour de roue
  float nbpulses = (nbtours*nbpulses); // convertie le nombre de tour en nombre de pulse
  sparx.moteurs.encodeurDroite = ENCODER_Read(1); //lecture de l'encodeur droit
  //si on a atteint la valeur d'encodeur 
  if (abs(sparx.moteurs.encodeurDroite) > (nbpulses) ) 
  {
    //retourne true
    return true;
  }
  else
  //sinon false
    return false;
};

/*
 * @brief Fonction permet de retourner la distance lu par l'encodeur de la roue gauche
 * @param rien
 * @return float: retourne la distance parcourue par le roue gauche en cm
 */
float getDistance(){
  sparx.moteurs.encodeurGauche = ENCODER_Read(LEFT); //lecture de l'encodeur de gauche
  float distancel = (sparx.moteurs.encodeurGauche/pulse_tour) * circonference_cm; //conversion du nombre de pulses en cm
  return distancel; //retourne la distance
}