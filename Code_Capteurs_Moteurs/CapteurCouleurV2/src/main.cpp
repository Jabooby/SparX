#include <Arduino.h>
#include <LibRobus.h>
#include <Adafruit_TCS34725.h>
#include <Wire.h>


/* Initialise with specific int time and gain values */
Adafruit_TCS34725 tcs = Adafruit_TCS34725(TCS34725_INTEGRATIONTIME_700MS, TCS34725_GAIN_1X);


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

//Imprime les valeurs de rgb en boucle
void loop() {
  delay(500);
        uint16_t r, g, b, c, colorTemp, lux;
        getRawData_noDelay(&r, &g, &b, &c);
        colorTemp = tcs.calculateColorTemperature(r, g, b);
        lux = tcs.calculateLux(r, g, b);

        Serial.print("Color Temp: "); Serial.print(colorTemp, DEC); Serial.print(" K - ");
        Serial.print("Lux: "); Serial.print(lux, DEC); Serial.print(" - ");
        Serial.print("R: "); Serial.print(r, DEC); Serial.print(" ");
        Serial.print("G: "); Serial.print(g, DEC); Serial.print(" ");
        Serial.print("B: "); Serial.print(b, DEC); Serial.print(" ");
        Serial.print("C: "); Serial.print(c, DEC); Serial.print(" ");
        Serial.println(" ");
}
