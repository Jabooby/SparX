#ifndef MY_LIBRARY_H
#define MY_LIBRARY_H
#include <Arduino.h>
#include <SPI.h>
#define uchar unsigned char
#define uint unsigned int
#define MAX_LEN 16
#define PCD_IDLE 0x00             //È¡Ïûµ±Ç°ÃüÁî²»ÐÐ¶¯
#define PCD_AUTHENT 0x0E         //ÑéÖ¤ÃÜÂëµÄÔ¿³×
#define PCD_RECEIVE 0x08         //½ÓÊÕÊý¾Ý
#define PCD_TRANSMIT 0x04        //·¢ËÍÊý¾Ý
#define PCD_TRANSCEIVE 0x0C        //·¢ËÍºÍ½ÓÊÕÊý¾Ý
#define PCD_RESETPHASE 0x0F        //ÖØÖÃ
#define PCD_CALCCRC 0x03        //CRCÐ£ÑéºÍµÄ¼ÆËã

//Mifare_One¿¨µÄÃüÁîÎ»
#define PICC_REQIDL 0x26        //ÔÚÌìÏßÇøÓòËÑË÷²»½øÈëË¯ÃßÄ£Ê½µÄ¿¨
#define PICC_REQALL 0x52         //ËÑË÷ÌìÏßÇøÓòÖÐµÄËùÓÐ¿¨
#define PICC_ANTICOLL 0x93        //·ÀÖ¹³åÍ»
#define PICC_SElECTTAG 0x93     //Ñ¡Ôñ¿¨
#define PICC_AUTHENT1A 0x60     //ÑéÖ¤AÃÜÂëÃÜÔ¿
#define PICC_AUTHENT1B 0x61     //ÑéÖ¤BÃÜÂëÃÜÔ¿
#define PICC_READ 0x30            //¶Á
#define PICC_WRITE 0xA0            //Ð´
#define PICC_DECREMENT 0xC0     //¿Û³ýÖµ
#define PICC_INCREMENT 0xC1     //×°ÔØÖµ
#define PICC_RESTORE 0xC2         //»¹Ô­Êý¾Ýµ½»º³åÇø
#define PICC_TRANSFER 0xB0        //±£´æÊý¾Ýµ½»º³åÇø
#define PICC_HALT 0x50             //Ë¯ÃßÄ£Ê½


//Í¨ÐÅÊ±MF522µÄ·µ»ØÖµ
#define MI_OK 0
#define MI_NOTAGERR 1
#define MI_ERR 2

//------------------MFRC522 ×¢²á ---------------
//Page 0:ÃüÁîºÍ×´Ì¬
#define Reserved00 0x00             //±£Áô½«À´Ö®ÓÃ
#define CommandReg 0x01             //Æô¶¯ºÍÍ£Ö¹ÃüÁîµÄÖ´ÐÐ
#define CommIEnReg 0x02             //ÖÐ¶ÏÇëÇó´«µÝµÄÊ¹ÄÜºÍ½ûÄÜ¿ØÖÆÎ»¡£
#define DivlEnReg 0x03                 //ÖÐ¶ÏÇëÇó´«µÝµÄÊ¹ÄÜºÍ½ûÄÜ¿ØÖÆÎ»¡£
#define CommIrqReg 0x04             //°üº¬ÖÐ¶ÏÇëÇó±êÖ¾
#define DivIrqReg 0x05                //°üº¬ÖÐ¶ÏÇëÇó±êÖ¾
#define ErrorReg 0x06                 //´íÎó±êÖ¾£¬Ö¸Ê¾Ö´ÐÐÐÐµÄÉÏ¸öÃüÁîµÄ´íÎó×´Ì¬
#define Status1Reg 0x07             //°üº¬Í¨ÐÅµÄ×´Ì¬±êÖ¾
#define Status2Reg 0x08             //°üº¬½ÓÊÕÆ÷ºÍ·¢ÉäÆ÷µÄ×´Ì¬±êÖ¾
#define FIFODataReg 0x09            //64×Ö½ÚFIFO»º³åÆ÷µÄÊäÈëÊä³ö
#define FIFOLevelReg 0x0A            //Ö¸Ê¾FIFOÖÐ´æ´¢µÄ×Ö½ÚÊý
#define WaterLevelReg 0x0B            //¶¨ÒåFIFOÏÂÒçºÍÉÏÒç±¨¾¯µÄFIFOÉî¶È¡£
#define ControlReg 0x0C                //²»Í¬µÄ¿ØÖÆ¼Ä´æÆ÷
#define BitFramingReg 0x0D            //ÃæÏòÎ»µÄÖ¡µ÷½Ú
#define CollReg 0x0E                //RF½Ó¿ÚÉÏ¼ì²âµ½µÄµÚÒ»¸öÎ»³åÍ»µÄÎ»µÄÎ»ÖÃ
#define Reserved01 0x0F                //±£Áô½«À´Ö®ÓÃ
//Page 1:¿ØÖÆ
#define Reserved10 0x10                //±£Áô½«À´Ö®ÓÃ
#define ModeReg 0x11                //¶¨Òå·¢ÉäºÍ½ÓÊÕµÄ³£ÓÃÄ£Ê½
#define TxModeReg 0x12                //¶¨Òå·¢Éä¹ý³ÌµÄÊý¾Ý´«ÊäËÙÂÊ
#define RxModeReg 0x13                //¶¨Òå½ÓÊÕ¹ý³ÌÖÐµÄÊý¾Ý´«ÊäËÙÂÊ
#define TxControlReg 0x14            //¿ØÖÆÌìÏßÇý¶¯¹Ü½ÅTX1ºÍTX2µÄÂß¼­ÌØÕ÷
#define TxAutoReg 0x15                //¿ØÖÆÌìÏßÇý¶¯µÄÉèÖÃ
#define TxSelReg 0x16                //Ñ¡ÔñÌìÏßÇý¶¯Æ÷µÄÄÚ²¿Ô´
#define RxSelReg 0x17                //Ñ¡×ÅÄÚ²¿µÄ½ÓÊÕÆ÷ÉèÖÃ
#define RxThresholdReg 0x18            //Ñ¡ÔñÎ»ÒëÂëÆ÷µÄ·§Öµ
#define DemodReg 0x19                //¶¨Òå½âµ÷Æ÷µÄÉèÖÃ
#define Reserved11 0x1A                //±£Áô½«À´Ö®ÓÃ
#define Reserved12 0x1B                //±£Áô½«À´Ö®ÓÃ
#define MifareReg 0x1C                //¿ØÖÆISO 14443/MIFARE Ä£Ê½106kbit/sµÄÍ¨ÐÅ
#define Reserved13 0x1D                //±£Áô½«À´Ö®ÓÃ
#define Reserved14 0x1E                //±£Áô½«À´Ö®ÓÃ
#define SerialSpeedReg 0x1F            //Ñ¡Ôñ´®ÐÐUART½Ó¿ÚµÄËÙÂÊ
//Page 2:·¢ÉúÆ÷
#define Reserved20 0x20                //±£Áô½«À´Ö®ÓÃ
#define CRCResultRegM 0x21            //ÏÔÊ¾CRC¼ÆËãµÄÊµ¼ÊMSBºÍLSBÖµ£¨MSB£©
#define CRCResultRegL 0x22            //ÏÔÊ¾CRC¼ÆËãµÄÊµ¼ÊMSBºÍLSBÖµ£¨LSB£©
#define Reserved21 0x23                //±£Áô½«À´Ö®ÓÃ
#define ModWidthReg 0x24            //¿ØÖÆModWidthµÄÉèÖÃ
#define Reserved22 0x25                //±£Áô½«À´Ö®ÓÃ
#define RFCfgReg 0x26                //ÅäÖÃ½ÓÊÜÆ÷ÔöÒæ
#define GsNReg 0x27                    //Ñ¡ÔñÌìÏßÇý¶¯Æ÷¹Ü½ÅTX1ºÍTX2µÄµ÷ÖÆµçµ¼
#define CWGsPReg 0x28                //Ñ¡ÔñÌìÏßÇý¶¯Æ÷¹Ü½ÅTX1ºÍTX2µÄµ÷ÖÆµçµ¼
#define ModGsPReg 0x29                //Ñ¡ÔñÌìÏßÇý¶¯Æ÷¹Ü½ÅTX1ºÍTX2µÄµ÷ÖÆµçµ¼
#define TModeReg 0x2A                //¶¨ÒåÄÚ²¿¶¨Ê±Æ÷µÄÉèÖÃA
#define TPrescalerReg 0x2B            //¶¨ÒåÄÚ²¿¶¨Ê±Æ÷µÄÉèÖÃB
#define TReloadRegH 0x2C            //ÃèÊö16Î»³¤µÄ¶¨Ê±Æ÷ÖØ×°Öµ£¨C£©
#define TReloadRegL 0x2D            //ÃèÊö16Î»³¤µÄ¶¨Ê±Æ÷ÖØ×°Öµ£¨D£©
#define TCounterValueRegH 0x2E        //ÏÔÊ¾16Î»³¤µÄÊµ¼Ê¶¨Ê±Æ÷Öµ£¨E£©
#define TCounterValueRegL 0x2F        //ÏÔÊ¾16Î»³¤µÄÊµ¼Ê¶¨Ê±Æ÷Öµ£¨F£©
//Page 3:¼ÇÂ¼
#define Reserved30 0x30                //±£Áô½«À´Ö®ÓÃ
#define TestSel1Reg 0x31            //³£ÓÃ²âÊÔÐÅºÅµÄÅäÖÃ
#define TestSel2Reg 0x32            //³£ÓÃ²âÊÔÐÅºÅµÄÅäÖÃºÍPRBS¿ØÖÆ
#define TestPinEnReg 0x33            //D1¡ªD7Êä³öÇý¶¯Æ÷µÄÊ¹ÄÜ¹Ü½Å£¨×¢£º½öÓÃÓÚ´®ÐÐ½Ó¿Ú£©
#define TestPinValueReg 0x34        //¶¨ÒåD1-D7ÓÃ×öI/O×ÜÏßµÄÖµ
#define TestBusReg 0x35                //ÏÔÊ¾ÄÚ²¿²âÊÔ×ÜÏßµÄ×´Ì¬
#define AutoTestReg 0x36            //¿ØÖÆÊý×Ö×Ô²âÊÔ
#define VersionReg 0x37                //ÏÔÊ¾°æ±¾
#define AnalogTestReg 0x38            //¿ØÖÆ¹Ü½ÅAUX1ºÍAUX2
#define TestDAC1Reg 0x39             //¶¨Òå TestDAC1µÄ²âÊÔÖµ
#define TestDAC2Reg 0x3A             //¶¨Òå TestDAC2µÄ²âÊÔÖµ
#define TestADCReg 0x3B             //ÏÔÊ¾ ADC IºÍ QÍ¨µÀµÄÊµ¼ÊÖµ
#define Reserved31 0x3C             //±£ÁôÓÃÓÚ²úÆ·²âÊÔ
#define Reserved32 0x3D             //±£ÁôÓÃÓÚ²úÆ·²âÊÔ
#define Reserved33 0x3E             //±£ÁôÓÃÓÚ²úÆ·²âÊÔ
#define Reserved34 0x3F                //±£ÁôÓÃÓÚ²úÆ·²âÊÔ
const int chipSelectPin = 48;    //SDAÒý½ÅÆ¬Ñ¡Òý½Å
const int NRSTPD = 49; 

void ShowCardID(uchar *id);
void ShowCardType(uchar* type);
void Write_MFRC522(uchar addr, uchar val);
uchar Read_MFRC522(uchar addr);
void SetBitMask(uchar reg, uchar mask);
void ClearBitMask(uchar reg, uchar mask);
void AntennaOn(void);
void AntennaOff(void);
void MFRC522_Reset(void);
void MFRC522_Init(void);
uchar MFRC522_Request(uchar reqMode, uchar *TagType);
uchar MFRC522_ToCard(uchar command, uchar *sendData, uchar sendLen, uchar *backData, uint *backLen);
uchar MFRC522_Anticoll(uchar *serNum);
void CalulateCRC(uchar *pIndata, uchar len, uchar *pOutData);
uchar MFRC522_Write(uchar blockAddr, uchar *writeData);
void MFRC522_Halt(void);

#endif