/**************************************************************************/
/*!
    @file     main.cpp
*/
/**************************************************************************/
#include <Wire.h>
#include <Adafruit_PN532.h>
#include <LiquidCrystal_I2C.h>
#include <Adafruit_AM2320.h>
#include <Adafruit_Sensor.h>

// If using the breakout or shield with I2C, define just the pins connected
// to the IRQ and reset lines.  Use the values below (2, 3) for the shield!
#define PN532_IRQ   (20)
#define PN532_RESET (21)  // Not connected by default on the NFC Shield


// Or use this line for a breakout or shield with an I2C connection:
Adafruit_PN532 nfc(PN532_IRQ, PN532_RESET);
LiquidCrystal_I2C lcd(0x27,16,2);
Adafruit_AM2320 am2320 = Adafruit_AM2320();

// Décalartionde fonction
void lecture_nfc(uint8_t page, uint8_t count, uint8_t* data);
void ecriture_nfc (uint8_t page, uint8_t count, uint8_t* message);
void lecture_NFC_NbByte(uint8_t pageInit, uint8_t nbByte, uint8_t* data);
void ecriture_NFC_NbByte(uint8_t pageInit, uint8_t nbByte, char* message);
void LCDWrite(int column, int row, uint8_t* data);
int getTempAmbiant();
int getHumidityAmbiant();
int getHumidityDirt();
int calculHumidityDirt();
void LCDRFID(uint8_t* donnee);
void LCDCapteurs(uint8_t* donneeNFC, uint8_t* donneeCapteurs);

//valeur global
double timer;

void setup(void) {
  Serial.begin(9600);
  while (!Serial) delay(10); // for Leonardo/Micro/Zero

  Serial.println("Hello!");
  lcd.init();
  lcd.backlight(); // ativer lumière arrière
  nfc.begin();
  uint32_t versiondata = nfc.getFirmwareVersion();
  if (! versiondata) {
    Serial.print("Didn't find PN53x board");
    while (1); // halt
  }
  // Got ok data, print it out!
  Serial.print("Found chip PN5"); Serial.println((versiondata>>24) & 0xFF, HEX);
  Serial.print("Firmware ver. "); Serial.print((versiondata>>16) & 0xFF, DEC);
  Serial.print('.'); Serial.println((versiondata>>8) & 0xFF, DEC);

  Serial.println("Waiting for an ISO14443A Card ...");
  timer = millis();
}


void loop(void) {
  uint8_t nbByte = 32;
  uint8_t donneeNFC[nbByte];
  uint8_t donneeCapteurs[nbByte];
  //uint8_t messageNFC[] = "Cactus      T05 H10 h07 L87   F"; //https://tools.word-counter.com/character-count/result.html
  if((millis() - timer)> 24)
  {
    //lecture_nfc(4, nbByte, donneeNFC);
    //ecriture_nfc(5, nbByte, messageNFC);
    lecture_NFC_NbByte(69, nbByte, donneeNFC);
    //LCDRFID(donneeNFC);
    LCDCapteurs(donneeNFC, donneeCapteurs);
    timer = millis();
    //ecriture_NFC_NbByte(69, nbByte, messageNFC);
  }
}

void LCDCapteurs(uint8_t* donneeNFC, uint8_t* donneeCapteurs)
{
  uint8_t donneeLCDCapteurs[20];
  uint8_t humidityAmbiant[3];
  uint8_t tempAmbiant[3];
  uint8_t humidityDirt[3];
  uint8_t donneeNOM[12];
  uint8_t donneeEspace[]= {32, 32, 32, 32};
  for(int i = 0; i<12; i++)
  {
    donneeNOM[i] = donneeNFC[i];
    donneeCapteurs[i] = donneeNFC[i];
  }
  itoa(getHumidityAmbiant(), (char*)humidityAmbiant, 10);
  itoa(getTempAmbiant(), (char*)tempAmbiant, 10);
  itoa(calculHumidityDirt(), (char*)humidityDirt, 10);
  donneeLCDCapteurs[0] = 'T', donneeLCDCapteurs[1] = tempAmbiant[0], donneeLCDCapteurs[2] = tempAmbiant[1], donneeLCDCapteurs[3] = 32; //temp
  donneeLCDCapteurs[4] = 'H', donneeLCDCapteurs[5] = humidityAmbiant[0], donneeLCDCapteurs[6] = humidityAmbiant[1], donneeLCDCapteurs[7] = 32;
  donneeLCDCapteurs[8] = 'h', donneeLCDCapteurs[9] = humidityDirt[0], donneeLCDCapteurs[10] = humidityDirt[1], donneeLCDCapteurs[11] = 32;
  donneeLCDCapteurs[12] = 'L', donneeLCDCapteurs[13] = '7', donneeLCDCapteurs[14] = '7', donneeLCDCapteurs[15] = 32;
  donneeLCDCapteurs[16] = 32, donneeLCDCapteurs[17] = 32, donneeLCDCapteurs[18] = 32, donneeLCDCapteurs[19] = 'F';
  
  LCDWrite(0,0, donneeNOM);
  LCDWrite(12,0, donneeEspace);
  LCDWrite(0,1, donneeLCDCapteurs);
}

void LCDRFID(uint8_t* donnee)
{
  uint8_t donneeNFCLCD[20];
  uint8_t donneeNOM[12];
  uint8_t donneeEspace[]= {32, 32, 32, 32};
  for(int i = 12; i<32; i++)
  {
    donneeNFCLCD[i-12] = donnee[i];
  }
  for(int i = 0; i<12; i++)
  {
    donneeNOM[i] = donnee[i];
  }
  LCDWrite(0,0, donneeNOM);
  LCDWrite(12,1, donneeEspace);
  LCDWrite(0,1, donneeNFCLCD);
}
int getTempAmbiant() 
{
  int temp = am2320.readTemperature();
  Serial.print("Température: "); Serial.println(temp);
  return (temp);
  
}

int getHumidityAmbiant() 
{
  int humidity = am2320.readHumidity();
  Serial.print("Humidité: "); Serial.println(humidity);
  return (humidity);
}

int getHumidityDirt() //Permet d'avoir le "Raw Data" du détecteur d'humidité
{
  int val;
  val = analogRead(A9); //connect sensor to Analog 0
  return val;
}
int calculHumidityDirt() //Transforme le "Raw Data en pourcentage"
{
  float val2;
  val2 = (((getHumidityDirt()-440.0)/-334.0)*100);
  return (int)val2;
}

void LCDWrite(int column, int row, uint8_t* data){
 // 
  //Écrire sur la colonne et la rangée spécifié. (Commence par 0)
  lcd.setCursor(column,row);
  lcd.print((char*)data);

}

void lecture_NFC_NbByte(uint8_t pageInit, uint8_t nbByte, uint8_t* data)
{
  uint8_t success;
  uint8_t uid[] = { 0, 0, 0, 0, 0, 0, 0 };  // Buffer to store the returned UID
  uint8_t uidLength;                        // Length of the UID (4 or 7 bytes depending on ISO14443A card type)
  uint8_t keya[6] = { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF };
  uint8_t page = 0;
  uint8_t nbPage = nbByte / 4;
  uint8_t lectureData[nbPage][4];
  //Serial.println(nbPage);
  success = nfc.readPassiveTargetID(PN532_MIFARE_ISO14443A, uid, &uidLength, 30);
  Serial.println(success);
  //Serial.print("  UID Value: ");
  nfc.PrintHex(uid, uidLength);
  if (success) 
  {
    for(page = pageInit; page < (nbPage + pageInit); page++)
    {
      success = nfc.mifareclassic_AuthenticateBlock(uid, uidLength, page, 0, keya);
      if (success)
      {
        success = nfc.mifareclassic_ReadDataBlock(page, lectureData[(page-pageInit)]);
        if (success)
        {
          //Serial.print(page), Serial.println(" ");;
          //nfc.PrintHexChar(lectureData[(page-pageInit)], 4);
          if(page == ((nbPage + pageInit)-1))
          {
            //ici que on met les données lectureData dans data
            for(int i = 0; i < nbPage; i++)
            {
              for(int j = 0; j < 4; j++)
              {
                data[(i*4)+j] = lectureData[i][j];
              }
            }
          }
        }
      }
    }
  }
  for(int i = 0; i < nbByte; i++)
  {
    Serial.print((char)data[i]);
  }
  Serial.println(' ');
}

void ecriture_NFC_NbByte(uint8_t pageInit, uint8_t nbByte, char* message)
{
  uint8_t success;
  uint8_t uid[] = { 0, 0, 0, 0, 0, 0, 0 };  // Buffer to store the returned UID
  uint8_t uidLength;                        // Length of the UID (4 or 7 bytes depending on ISO14443A card type)
  uint8_t keya[6] = { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF };
  uint8_t page = 0;
  uint8_t nbPage = nbByte / 4;
  uint8_t lectureData[nbPage][4];
  success = nfc.readPassiveTargetID(PN532_MIFARE_ISO14443A, uid, &uidLength);
  for(int i = 0; i < nbPage; i++)
  {
    for(int j = 0; j < 4; j++)
    {
      lectureData[i][j] = message[(i*4)+j];
    }
  }
  //Serial.print("  UID Value: ");
  //nfc.PrintHex(uid, uidLength);
  if (success) 
  {
    for(page = pageInit; page < (nbPage + pageInit); page++)
    {
      success = nfc.mifareclassic_AuthenticateBlock(uid, uidLength, page, 0, keya);
      if (success)
      {
        success = nfc.mifareclassic_WriteDataBlock(page, lectureData[(page-pageInit)]);
        if (success)
        {
          //Serial.print("SUCCESS "), Serial.println(page);;
        }
      }
    }
  }
}

void lecture_nfc(uint8_t page, uint8_t count, uint8_t* data)
{
  uint8_t success;
  uint8_t uid[] = { 0, 0, 0, 0, 0, 0, 0 };  // Buffer to store the returned UID
  uint8_t uidLength;                        // Length of the UID (4 or 7 bytes depending on ISO14443A card type)
  Serial.println(uidLength);
  // Wait for an ISO14443A type cards (Mifare, etc.).  When one is found
  // 'uid' will be populated with the UID, and uidLength will indicate
  // if the uid is 4 bytes (Mifare Classic) or 7 bytes (Mifare Ultralight)
  success = nfc.readPassiveTargetID(PN532_MIFARE_ISO14443A, uid, &uidLength);

  if (success) {
    // Display some basic information about the card
    /*Serial.println("Found an ISO14443A card");
    Serial.print("  UID Length: ");Serial.print(uidLength, DEC);Serial.println(" bytes");
    Serial.print("  UID Value: ");
    nfc.PrintHex(uid, uidLength);
    Serial.println("");*/

    if (uidLength == 4)
    {
      // We probably have a Mifare Classic card ...
      //Serial.println("Seems to be a Mifare Classic card (4 byte UID)");

      // Now we need to try to authenticate it for read/write access
      // Try with the factory default KeyA: 0xFF 0xFF 0xFF 0xFF 0xFF 0xFF
      //Serial.println("Trying to authenticate block 4 with default KEYA value");
      uint8_t keya[6] = { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF };

	  // Start with block 4 (the first block of sector 1) since sector 0
	  // contains the manufacturer data and it's probably better just
	  // to leave it alone unless you know what you're doing
      success = nfc.mifareclassic_AuthenticateBlock(uid, uidLength, page, 0, keya);

      if (success)
      {
        //Serial.println("Sector 1 (Blocks 4..7) has been authenticated");

        // If you want to write something to block 4 to test with, uncomment
		    // the following line and this text should be read back in a minute
        //memcpy(data, (const uint8_t[]){ 'T', 'R', 'I', 'C', 'K', 'Y', ' ', 'C', 'R', 'A', 'Z', 'Y', 'F', 'R', 'O', 'G' }, sizeof data);
        //success = nfc.mifareclassic_WriteDataBlock (4, data);

        // Try to read the contents of block 4
        success = nfc.mifareclassic_ReadDataBlock(page, data);

        if (success)
        {
          // Data seems to have been read ... spit it out
          //Serial.print("Reading Block "), Serial.print(page), Serial.println(":");
          for(int i = 0; i < uidLength; i++)
          {
            Serial.print(uid[i], HEX), Serial.print(' ');
          }
          Serial.println(' ');
          for(int i = 0; i < count; i++)
          {
            Serial.print((char)data[i]);
          }
          Serial.println(' ');
        }
        else
        {
          Serial.println("Ooops ... unable to read the requested block.  Try another key?");
        }
      }
      else
      {
        Serial.println("Ooops ... authentication failed: Try another key?");
      }
    }

    if (uidLength == 7)
    {
      // We probably have a Mifare Ultralight card ...
      Serial.println("Seems to be a Mifare Ultralight tag (7 byte UID)");

      // Try to read the first general-purpose user page (#4)
      Serial.println("Reading page 4");
      uint8_t data[4];
      success = nfc.mifareultralight_ReadPage (4, data);
      if (success)
      {
        // Data seems to have been read ... spit it out
        for(int i = 0; i < uidLength; i++)
          {
            Serial.print(uid[i], HEX), Serial.print(' ');
          }
          Serial.println(' ');
        nfc.PrintHexChar(data, 4);
        Serial.println("");

        // Wait a bit before reading the card again
        delay(1000);
      }
      else
      {
        Serial.println("Ooops ... unable to read the requested page!?");
      }
    }
  }
}

void ecriture_nfc (uint8_t page, uint8_t count, uint8_t* message)
{
  uint8_t success;
  uint8_t uid[] = { 0, 0, 0, 0, 0, 0, 0 };  // Buffer to store the returned UID
  uint8_t uidLength;                        // Length of the UID (4 or 7 bytes depending on ISO14443A card type)

  // Wait for an ISO14443A type cards (Mifare, etc.).  When one is found
  // 'uid' will be populated with the UID, and uidLength will indicate
  // if the uid is 4 bytes (Mifare Classic) or 7 bytes (Mifare Ultralight)
  success = nfc.readPassiveTargetID(PN532_MIFARE_ISO14443A, uid, &uidLength);

  if (success) {
    // Display some basic information about the card
    /*Serial.println("Found an ISO14443A card");
    Serial.print("  UID Length: ");Serial.print(uidLength, DEC);Serial.println(" bytes");
    Serial.print("  UID Value: ");
    nfc.PrintHex(uid, uidLength);
    Serial.println("");*/

    if (uidLength == 4)
    {
      // We probably have a Mifare Classic card ...
      //Serial.println("Seems to be a Mifare Classic card (4 byte UID)");

      // Now we need to try to authenticate it for read/write access
      // Try with the factory default KeyA: 0xFF 0xFF 0xFF 0xFF 0xFF 0xFF
      //Serial.println("Trying to authenticate block 4 with default KEYA value");
      uint8_t keya[6] = { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF };

	  // Start with block 4 (the first block of sector 1) since sector 0
	  // contains the manufacturer data and it's probably better just
	  // to leave it alone unless you know what you're doing
      success = nfc.mifareclassic_AuthenticateBlock(uid, uidLength, page, 0, keya);

      if (success)
      {
        // If you want to write something to block 4 to test with, uncomment
		    // the following line and this text should be read back in a minute
        success = nfc.mifareclassic_WriteDataBlock (page, message);
        if (success)
        {
          Serial.println("SUCCESS");
        }
        else
        {
          Serial.println("Ooops ... unable to read the requested block.  Try another key?");
        }
      }
      else
      {
        Serial.println("Ooops ... authentication failed: Try another key?");
      }
    }

    if (uidLength == 7)
    {
      // We probably have a Mifare Ultralight card ...
      Serial.println("Seems to be a Mifare Ultralight tag (7 byte UID)");

      // Try to read the first general-purpose user page (#4)
      Serial.println("Reading page 4");
      uint8_t data[32];
      success = nfc.mifareultralight_ReadPage (4, data);
      if (success)
      {
        // Data seems to have been read ... spit it out
        nfc.PrintHexChar(data, 4);
        Serial.println("");

        // Wait a bit before reading the card again
        delay(1000);
      }
      else
      {
        Serial.println("Ooops ... unable to read the requested page!?");
      }
    }
  }
}

