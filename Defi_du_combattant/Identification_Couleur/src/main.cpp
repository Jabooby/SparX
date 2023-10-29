#include <Arduino.h>
#include <LibRobus.h>
#include <Adafruit_TCS34725.h>
#include <Wire.h>



/* Initialise with specific int time and gain values */
Adafruit_TCS34725 tcs = Adafruit_TCS34725(TCS34725_INTEGRATIONTIME_50MS, TCS34725_GAIN_4X);


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


//Renvoie la couleur du sol
char retour_couleur(int r,int g,int b,int c) {
   //variable couleur
   char couleur;
  
        if((r>330)&&(r<410)&&(g>170)&&(g<205)&&(b>200)&&(b<230)&&(c>750)&&(c<900)){
          couleur='R';
        }
        else if((r>140)&&(r<180)&&(g>250)&&(g<310)&&(b>240)&&(b<290)&&(c>700)&&(c<860)){
           couleur='G';
        }
        else if((r<140)&&(g>185)&&(g<280)&&(b>350)&&(b<420)&&(c>800)&&(c<1000)){
           couleur='B';
        }
        else if((r>800)&&(r<980)&&(g>690)&&(g<840)&&(b>370)&&(b<450)&&(c>2000)&&(c<2500)){
           couleur='J';
        }
        else if((r>760)&&(g>800)&&(g<1200)&&(b>840)&&(b<1200)&&(c>2600)){
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
     Serial.println(retour_couleur(r,g,b,c));   
}

