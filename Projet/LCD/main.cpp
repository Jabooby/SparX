#include <Arduino.h>
#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27,16,2);
void LCDWrite(int x, int y, String data);
void setup() {
  lcd.init();
  
  lcd.backlight(); // ativer lumière arrière
}

void loop() {
  //écrire
  LCDWrite(0,0, "SparX");
  //pour effacer, utilisez lcd.clear();

}

void LCDWrite(int column, int row, String data){
 // 
  //Écrire sur la colonne et la rangée spécifié. (Commence par 0)
  lcd.setCursor(column,row);
  lcd.print(data);

}