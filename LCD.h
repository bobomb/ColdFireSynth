/*
 * LCD.h
 *
 *  Created on: Apr 1, 2012
 *      Author: bobomb
 */

#ifndef LCD_H_
#define LCD_H_

#include "IO_Map.h"

#define LCD_DATA0_PORT   PTBD_PTBD1     	// port for 4bit data bit 0 		//D5 poz:3 connector...[digits 0,1,...]
#define LCD_DATA1_PORT   PTBD_PTBD2    	// port for 4bit data bit 1 		//D6
#define LCD_DATA2_PORT   PTBD_PTBD3     	// port for 4bit data bit 2 		/
#define LCD_DATA3_PORT   PTBD_PTBD4     	// port for 4bit data bit 3 		
#define LCD_DATA0_DD    PTBDD_PTBDD1          // pin for 4bit data bit 0  
#define LCD_DATA1_DD    PTBDD_PTBDD2            	// pin for 4bit data bit 1  
#define LCD_DATA2_DD    PTBDD_PTBDD3            	// pin for 4bit data bit 2  
#define LCD_DATA3_DD    PTBDD_PTBDD4             	// pin for 4bit data bit 3  
#define LCD_RS_PORT     PTGD_PTGD0     	// port for RS line         		
#define LCD_RS_DD       PTGDD_PTGDD0            	// pin  for RS line         
#define LCD_E_PORT      PTBD_PTBD0     	// port for Enable line     		
#define LCD_E_DD        PTBDD_PTBDD0            	// pin  for Enable line   




void LCDInitialize();
void LCDClear();
void LCDWrite(uint8_t data,uint8_t rs);
void LCDCommand(uint8_t command);
void LCDWriteChar(uint8_t data);
void LCDWriteString(char *text);
void LCDHome();
void toggleE();


#endif /* LCD_H_ */
