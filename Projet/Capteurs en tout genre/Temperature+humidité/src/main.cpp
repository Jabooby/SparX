
#include <Arduino.h>
#include <LibRobus.h>
#include <Adafruit_AM2320.h>
#include <Adafruit_Sensor.h>

Adafruit_AM2320 am2320 = Adafruit_AM2320();

int gettemp();
int wet();


void setup() {
  Serial.begin(9600);
  while (!Serial) {
    delay(10);
  }
  am2320.begin();
}

void loop() 
{

  gettemp();
  wet();
  //Serial.print("\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n");
  
}

int gettemp() 
{
  int temp = am2320.readTemperature();
  Serial.print("Température: "); Serial.println(temp);
  return (temp);
  
}

int wet() 
{
  int humidity = am2320.readHumidity();
  Serial.print("Humidité: "); Serial.println(humidity);
  return (humidity);
}

