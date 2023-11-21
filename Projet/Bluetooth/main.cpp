#include <SoftwareSerial.h>
#include <Arduino.h>
SoftwareSerial BTSerial(11, 10); 
// Function prototypes
int BTReceive();
int BTWrite(String data);
void setup() {
  Serial.begin(9600);   
  BTSerial.begin(9600); 
}

void loop() {
  BTReceive();
}
//version qui retourne la connexion: 1-Connected 0-!Connected
int BTReceive(){
   if (BTSerial.available()) {
    char receivedChar = BTSerial.read();
    UDR0 = receivedChar; //Synonyme a Serial.print();
    return 1;
  }
  else return 0; // pas de connexion
}
//version qui retourne la valeure lu. Retourne 0 quand rien est recu
/*
char BTReceive() {
  if (BTSerial.available()) {
    return BTSerial.read();
  } else {
    return '\0'; // Sentinel value indicating no data
  }
}
*/
int BTWrite(String data){
  BTSerial.println(data);
  return 1;
}