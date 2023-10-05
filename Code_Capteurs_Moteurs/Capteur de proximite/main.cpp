#include <Arduino.h>
#include <LibRobus.h>
bool vert = false;
bool rouge = false;
int vertpin = 53;
int rougepin = 49;

void setup() {
pinMode(vertpin, INPUT);
pinMode(rougepin, INPUT);  
BoardInit(); 
}

void loop() {
  vert = digitalRead(vertpin);
  rouge = digitalRead(rougepin);
  
  if (rouge && vert){

  }
  if (!rouge && !vert){

  }
  if (!rouge && vert){

  }
  if (rouge && !vert){

  }
  
}
