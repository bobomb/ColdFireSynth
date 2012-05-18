/*
 * DAC.c
 *
 *  Created on: Mar 8, 2012
 *      Author: bobomb
 */
#include "DAC.h"


bool SPI_Initialize()
{
	SPI1C1 = 0x00;
	SPI1C1 = 0x52; //enable, SSOE = 1
	SPI1C2 = 0x50; //16 bit mode, MODFEN = 1
	SPI1BR = 0x10; //max output      		

	//set the SS to output	
	return TRUE;
}
//then writing to the data register
void SPI_Send(word u16Data)
{
	while(!SPI1S_SPTEF); //wait until SPTEF = 1 which means the buffer is empty and we can write
	SPI1DL = (byte)u16Data; //set lower byte
	SPI1DH = (byte)(u16Data>>8); //set higher byte
	while(!SPI1S_SPRF); //wait until we have recieved a byte

}

//SPI1_SPRF = spi read buffer full flag, is set to 1 when there is data in read buffer
//SPRF is cleared by reading SPRF when it is 1
//then reading the data register SPI1DL (we are in 8 bit mode)
word SPI_Get(void)
{	
	while(!SPI1S_SPTEF); //wait until any pending transmits are done
	SPI1DL=0xFF; //dummy write to wait for a read
	while(!SPI1S_SPRF);	 //wait until SPRF = 1 which means data is available in the read buffer
	return(SPI1DL); //returns data and resets SPRF
	
}
