#include <Arduino.h>

// put function declarations here:
int DetecteurLumiere1();
int DetecteurLumiere2();
int DetecteurLumiere3();
int DetecteurLumiere4();
int DetecteurLumiere5();
void setup() {
  // put your setup code here, to run once:
  pinMode(A8, INPUT);
  pinMode(A9, INPUT);
  pinMode(A10, INPUT);
  pinMode(A11, INPUT);
  pinMode(A12, INPUT);
  Serial.begin(9600);
}

void loop() {
  // put your main code here, to run repeatedly:
  DetecteurLumiere1();
Serial.println(analogRead(A8));
}

// put function definitions here:
int DetecteurLumiere1() {
  int lumiere;
  lumiere=analogRead(A8);
  return lumiere;
}

int DetecteurLumiere2() {
  int lumiere;
  lumiere=analogRead(A9);
  return lumiere;
}

int DetecteurLumiere3() {
  int lumiere;
  lumiere=analogRead(A10);
  return lumiere;
}

int DetecteurLumiere4() {
  int lumiere;
  lumiere=analogRead(A11);
  return lumiere;
}
int DetecteurLumiere5() {
  int lumiere;
  lumiere=analogRead(A12);
  return lumiere;
}