#include <Arduino.h>
#include <RFID.h>
#include <SPI.h>

uchar serNum[5];

uchar* CardRead();
void setup() {
  SPI.begin();
  Serial.begin(9600);
  pinMode(chipSelectPin, OUTPUT);       
  digitalWrite(chipSelectPin, LOW);    
  pinMode(NRSTPD, OUTPUT);               
  MFRC522_Init();
}

void loop() {
  CardRead();
  //Serial.println((int)CardRead); 
}

uchar* CardRead(){
  uchar status;
  uchar str[MAX_LEN];


  
  status = MFRC522_Request(PICC_REQIDL, str);
  if (status != MI_OK)
  {
    return 0;
  }
 
  //ShowCardType(str);

  
  status = MFRC522_Anticoll(str);

  
  if (status == MI_OK)
  {
   // Serial.print("The card's number is: ");
    memcpy(serNum, str, 5);
    ShowCardID(serNum);
    return serNum;
  }
  return 0;

  MFRC522_Halt(); 
  
}