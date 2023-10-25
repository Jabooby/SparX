#include <Arduino.h>
#include <LibRobus.h>

void setup(){
void BoardInit();//initialise la board en demi-lune

int angle_base=90;
int angle;
/*if(couleur=vert){
    angle=180;
    else{
    angle =0;}
 */

SERVO_Enable(SERVO_1);//active le servo moteur mettre 0 ou 1
/*if(capteurIR<50){
  SERVO_SetAngle(SERVO_1, angle);//choisis l'angle du servo moteur
  delay(10000);
  SERVO_SetAngle(SERVO_1, angle_base);//Remet l'angle de base du servo moteur
}*/
}