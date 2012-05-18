/*
 * DAC.h
 *
 *  Created on: Mar 8, 2012
 *      Author: bobomb
 */

#ifndef DAC_H_
#define DAC_H_

#include "IO_Map.h"
#define DD_INPUT 		0x0
#define DD_OUTPUT 		0x1
#define SS_ON 	 		0x0
#define SS_OFF  		0x1;
#define WAIT_CYCLES 	16

//DD of 0 is input
//DD of 1 is output
void SPI_Send(word u16Data);

word SPI_Get(void);

bool SPI_Initialize();


#endif /* DAC_H_ */
