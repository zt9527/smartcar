#include <EEPROM.h>
#include <DRV8825.h>
#include <SPI.h>
#include <MFRC522.h>
#include "ESP8266WiFi.h"

//#define MOTOR_STEPS 200
#define SS_PIN 		4
#define RST_PIN 	5
#define MARK_NUM	8


#define SERIAL_SUPPORT	TRUE
// #define YLD0		0
// #define YLD1		2
// #define YLD2		16
// #define YLD3		15

//int motor_a_e = 0; //0
//int motor_a_d = 1; //PWM
//int motor_a_s = 2; //1 0
//int motor_b_e = 3;
//int motor_b_d = 4;
//int motor_b_s = 5;


MFRC522 rfid(SS_PIN, RST_PIN); 
MFRC522::MIFARE_Key key; 
byte nuidPICC[4];

byte rfid_id[4*MARK_NUM];
char mark_num = 0;
bool mark_flag = 1;
bool write_flag = 1;

void YL70_init()
{

}

void motor_init()
{
  pinMode(0,OUTPUT); // Enable: EN可以使用单片机端口控制,也可以直接连接GND使能
  pinMode(15,OUTPUT); // steps:脉冲个数
  pinMode(2,OUTPUT); // dir:为方向控制
  digitalWrite(6,LOW);
}
void car_go_straight()
{
    digitalWrite(2,HIGH); // Set Dir high
    for(byte x = 0; x < 200; x++) // Loop 200 times
  {
      digitalWrite(15,HIGH); // Output high
      delayMicroseconds(800); // Wait 1/2 a ms
      digitalWrite(15,LOW); // Output low
      delayMicroseconds(800); // Wait 1/2 a ms
    }
}
void car_go_left()
{

}
void car_go_right()
{

}
void car_go_around()
{

}

void RC522_init()
{
	//EEPROM.begin(4*MARK_NUM);
	#ifdef SERIAL_SUPPORT
	Serial.begin(115200);
	#endif

	SPI.begin(); // Init SPI bus
	rfid.PCD_Init(); // Init MFRC522 
	for (byte i = 0; i < 6; i++) {
	key.keyByte[i] = 0xFF;
	}
	// Serial.println(F("This code scan the MIFARE Classsic NUID."));
	// Serial.print(F("Using the following key:"));
	// printHex(key.keyByte, MFRC522::MF_KEY_SIZE);
	}
void RC522_work()
{
	// Look for new cards
	if ( ! rfid.PICC_IsNewCardPresent())
	return;
	// Verify if the NUID has been readed
	if ( ! rfid.PICC_ReadCardSerial())
	return;
	// // Serial.print(F("PICC type: "));
	// MFRC522::PICC_Type piccType = rfid.PICC_GetType(rfid.uid.sak);
	// // Serial.println(rfid.PICC_GetTypeName(piccType));
	// // Check is the PICC of Classic MIFARE type
	// if (piccType != MFRC522::PICC_TYPE_MIFARE_MINI &&  
	//   piccType != MFRC522::PICC_TYPE_MIFARE_1K &&
	//   piccType != MFRC522::PICC_TYPE_MIFARE_4K) {
	//   // Serial.println(F("Your tag is not of type MIFARE Classic."));
	//   return;
	// }
  	if (rfid.uid.uidByte[0] != nuidPICC[0] || 
    rfid.uid.uidByte[1] != nuidPICC[1] || 
    rfid.uid.uidByte[2] != nuidPICC[2] || 
    rfid.uid.uidByte[3] != nuidPICC[3] ) 
    {
    // Serial.println(F("A new card has been detected."));
    // Store NUID into nuidPICC array
	  	if(mark_flag)
	  	{
	  		for(byte i = 0; i < (4*mark_num); i++)
			{
				if(rfid.uid.uidByte[0] == rfid_id[i])
				{
					for(byte j = 1; j < 4; j++)
					{
						if(rfid.uid.uidByte[j] != rfid_id[(i+j)])
							break;
						if(j == 3)
						{
							#ifdef SERIAL_SUPPORT
							Serial.print(F("this rfid_id in the array!"));
							Serial.println();
							#endif
							write_flag = 0;
			    			for (byte i = 0; i < 4; i++) 
					    	{
					      		nuidPICC[i] = rfid.uid.uidByte[i];
					    	}
						}
						else
							write_flag = 1;
					}
				}
			}
			if(write_flag)
    		{
    			for (byte i = 0; i < 4; i++) 
		    	{
		    		rfid_id[i+(4*mark_num)] = rfid.uid.uidByte[i];//write id to lab
		    		//EEPROM.write(i+(4*mark_num),rfid.uid.uidByte[i]);
		      		nuidPICC[i] = rfid.uid.uidByte[i];
		    	}
		    	mark_num++;
				if(mark_num == MARK_NUM)	//filsh write to lab
					mark_flag = 0;
	    	}
		}
		else
		{
			for (byte i = 0; i < 4; i++)	//get marking point rfid
			{
	      		nuidPICC[i] = rfid.uid.uidByte[i];
	    	}
		}
		#ifdef SERIAL_SUPPORT
	    Serial.print(F("In dec: "));
	    printDec(rfid.uid.uidByte, rfid.uid.size);
	    Serial.println();

	    Serial.print(F("rfid_id: "));
	    printDec(rfid_id, 4*mark_num);
	    Serial.println();
	    #endif
  	}
	else 
		#ifdef SERIAL_SUPPORT
		Serial.println(F("Card read previously."));
		#endif
	// Halt PICC
	rfid.PICC_HaltA();
	// Stop encryption on PCD
	rfid.PCD_StopCrypto1();
}

#ifdef SERIAL_SUPPORT
void printHex(byte *buffer, byte bufferSize) {
  for (byte i = 0; i < bufferSize; i++) {
    Serial.print(buffer[i] < 0x10 ? " 0" : " ");
    Serial.print(buffer[i], HEX);
  }
}

void printDec(byte *buffer, byte bufferSize) 
{
  for (byte i = 0; i < bufferSize; i++) 
  {
    Serial.print(buffer[i] < 0x10 ? " 0" : " ");
    Serial.print(buffer[i], DEC);
  }
}
#endif

void setup() 
{
	// YL70_init();
  	//RC522_init();
  	//motor_init();
}

void loop() 
{
 	//RC522_work();
 	//car_go_straight();
}
