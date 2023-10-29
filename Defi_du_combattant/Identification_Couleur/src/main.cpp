#include <Arduino.h>
#include <LibRobus.h>
#include <Adafruit_TCS34725.h>
#include <Wire.h>



/* Initialise with specific int time and gain values */
Adafruit_TCS34725 tcs = Adafruit_TCS34725(TCS34725_INTEGRATIONTIME_50MS, TCS34725_GAIN_1X);


//Lit les valeurs clear, red, green et bleu
void getRawData_noDelay(uint16_t* r, uint16_t* g, uint16_t* b, uint16_t* c) {
    *c = tcs.read16(TCS34725_CDATAL);
    *r = tcs.read16(TCS34725_RDATAL);
    *g = tcs.read16(TCS34725_GDATAL);
    *b = tcs.read16(TCS34725_BDATAL);
}

//Marque si le capteur est bien branché
void setup() {

    Serial.begin(9600);

    if (tcs.begin()) {
        Serial.println("Found sensor");
    } else {
        Serial.println("No TCS34725 found ... check your connections");
        while (1);
    }
}

//variable couleur
char couleur;

//Renvoie la couleur du sol
int retour_couleur(int r,int g,int b,int c) {
  
        if((c<500)&&(r>100)){
          couleur='R';
        }
        else if((c<500)&&(r<100)&&(b<150)&&(g>140)){
           couleur='G';
        }
        else if((c<500)&&(r<100)&&(b>150)){
           couleur='B';
        }
        else if((c>750)&&(c<1000)){
           couleur='J';
        }
        else if(c>1000){
           couleur='W';
        }
        else{
          couleur='T';
        }
        return couleur;
}

//loop de la fonction
void loop() {
  delay(500);
   uint16_t r, g , b, c;
        getRawData_noDelay(&r, &g, &b, &c);

        Serial.print("R: "); Serial.print(r, DEC); Serial.print(" ");
        Serial.print("G: "); Serial.print(g, DEC); Serial.print(" ");
        Serial.print("B: "); Serial.print(b, DEC); Serial.print(" ");
        Serial.print("C: "); Serial.print(c, DEC); Serial.print(" ");
        Serial.println(" ");
     Serial.print(retour_couleur(r,g,b,c));   
}

