#include <Arduino.h>
#include <LibRobus.h>
int analog1 = A0;
int analog2 = A1;


void setup() {
  pinMode(analog1, INPUT_PULLUP);
  pinMode(analog2, INPUT_PULLUP);
 
}

bool start() {
  bool go = false;
  int frequency = analogRead(analog1);
  int ambiant = analogRead(analog2);
  if (frequency > ambiant) go=true;
  return go;
}
