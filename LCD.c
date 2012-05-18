/*
 * LCD.c
 *
 *  Created on: Apr 1, 2012
 *      Author: bobomb
 */

#include "LCD.h"
#include "Cpu.h"

void LCDInitialize()
{
	/** Set data direction on all pins to output */
	LCD_DATA0_DD = 1;
	LCD_DATA1_DD = 1;
	LCD_DATA2_DD = 1;
	LCD_DATA3_DD = 1;
	LCD_RS_DD = 1;	
	LCD_E_DD = 1;

	/** Delay for LCD to power up */
	Cpu_Delay100US(160);
	
	/** Set RS LOW */
	LCD_RS_PORT = 0;
	LCD_E_PORT = 0;

	/** Set output to 0x3*/
	LCDCommand(0x3);
	
	/** 5 ms wait */
	Cpu_Delay100US(50);
	/** Set output to 0x3 again*/
	LCDCommand(0x3);
	Cpu_Delay100US(50);
	LCDCommand(0x3);
	
	Cpu_Delay100US(2);
	
	/** Write 0x2 enable 4 bit mode*/
	LCDCommand(0x2);
	Cpu_Delay100US(20);
	
	/** Set to 4 bit mode, 2 lines */
	LCDCommand(0x28);
	Cpu_Delay100US(20);
	
	LCDCommand(0x8);
	
	LCDCommand(0x01);
	Cpu_Delay100US(20);
	
	LCDCommand(0x0C);
	
}

void LCDCommand(uint8_t command)
{
	LCDWrite(command, 0);
}

void toggleE()
{
	Cpu_Delay100US(1);
	LCD_E_PORT = 1;
	Cpu_Delay100US(1);
	LCD_E_PORT = 0;
}

void LCDClear()
{
	
   	LCDWrite(0, 0);
   	Cpu_Delay100US(5);
}

void LCDHome()
{
   	LCDWrite(1, 0);
   	Cpu_Delay100US(5);
}

void LCDWriteString(char *text)
{
	char c;
    while ( (c = *text++) )  LCDWriteChar(c);
}
void LCDWriteChar(uint8_t data)
{
	/*
	if (data=='\n') 
	{
		if (g_nCurrentLine >= LCD_LINES - 1)
			lcd_setline(0);
		else
			lcd_setline(g_nCurrentLine+1);
	}
	else
	*/
	LCDWrite(data,1);
	Cpu_Delay100US(20);
}
void LCDWrite(uint8_t data,uint8_t rs) 
{
	/** rs = 0 instruction else data */
    if (rs)    
    	LCD_RS_PORT = 1; /** DATA */
    else     
    	LCD_RS_PORT = 0;  /** INSTRUCTION */

    
    /** 4 MSB bits */

	if(data & 0x80) LCD_DATA3_PORT = 1;
	if(data & 0x40) LCD_DATA2_PORT = 1;
	if(data & 0x20) LCD_DATA1_PORT = 1;
	if(data & 0x10) LCD_DATA0_PORT = 1 ;
    toggleE();
    
    Cpu_Delay100US(10);
	LCD_DATA0_PORT = 0;
	LCD_DATA1_PORT = 0;
	LCD_DATA2_PORT = 0;
	LCD_DATA3_PORT = 0;
    /** 4 LSB bits */
	if(data & 0x08) LCD_DATA3_PORT = 1;
	if(data & 0x04) LCD_DATA2_PORT = 1;
	if(data & 0x02) LCD_DATA1_PORT = 1;
	if(data & 0x01) LCD_DATA0_PORT = 1;
    toggleE();       
    Cpu_Delay100US(10);

	LCD_DATA0_PORT = 0;
	LCD_DATA1_PORT = 0;
	LCD_DATA2_PORT = 0;
	LCD_DATA3_PORT = 0;
}
