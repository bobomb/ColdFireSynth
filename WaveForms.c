/*
 * WaveForms.c
 *
 *  Created on: Mar 11, 2012
 *      Author: bobomb
 */
#include "WaveForms.h"
uint32_t r = 0x12345678;
//uint32_t tapsmask = 0x80000062;
uint32_t tapsmask = 0x2532;
#define POLYMASK32	0x6699
#define POLYMASK31	0x7A5BC2E3
uint32_t lfsr32 = 0xABCDE;
uint32_t lfsr31 = 0x23456789;
char * waveformNames[NUMBER_WAVETYPES]= {"Sine", "Square", "Saw", "Triangle", "4-bit Triangle", "Noise"}; 

const uint8_t sineTable[] = {
	    0x80, 0x83, 0x86, 0x89, 0x8C, 0x8F, 0x92, 0x95, 0x98, 0x9B, 0x9E, 0xA2,
	    0xA5, 0xA7, 0xAA, 0xAD, 0xB0, 0xB3, 0xB6, 0xB9, 0xBC, 0xBE, 0xC1, 0xC4,
	    0xC6, 0xC9, 0xCB, 0xCE, 0xD0, 0xD3, 0xD5, 0xD7, 0xDA, 0xDC, 0xDE, 0xE0,
	    0xE2, 0xE4, 0xE6, 0xE8, 0xEA, 0xEB, 0xED, 0xEE, 0xF0, 0xF1, 0xF3, 0xF4,
	    0xF5, 0xF6, 0xF8, 0xF9, 0xFA, 0xFA, 0xFB, 0xFC, 0xFD, 0xFD, 0xFE, 0xFE,
	    0xFE, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFE, 0xFE, 0xFE, 0xFD,
	    0xFD, 0xFC, 0xFB, 0xFA, 0xFA, 0xF9, 0xF8, 0xF6, 0xF5, 0xF4, 0xF3, 0xF1,
	    0xF0, 0xEE, 0xED, 0xEB, 0xEA, 0xE8, 0xE6, 0xE4, 0xE2, 0xE0, 0xDE, 0xDC,
	    0xDA, 0xD7, 0xD5, 0xD3, 0xD0, 0xCE, 0xCB, 0xC9, 0xC6, 0xC4, 0xC1, 0xBE,
	    0xBC, 0xB9, 0xB6, 0xB3, 0xB0, 0xAD, 0xAA, 0xA7, 0xA5, 0xA2, 0x9E, 0x9B,
	    0x98, 0x95, 0x92, 0x8F, 0x8C, 0x89, 0x86, 0x83, 0x80, 0x7D, 0x7A, 0x77,
	    0x74, 0x71, 0x6E, 0x6B, 0x68, 0x65, 0x62, 0x5E, 0x5B, 0x59, 0x56, 0x53,
	    0x50, 0x4D, 0x4A, 0x47, 0x44, 0x42, 0x3F, 0x3C, 0x3A, 0x37, 0x35, 0x32,
	    0x30, 0x2D, 0x2B, 0x29, 0x26, 0x24, 0x22, 0x20, 0x1E, 0x1C, 0x1A, 0x18,
	    0x16, 0x15, 0x13, 0x12, 0x10, 0x0F, 0x0D, 0x0C, 0x0B, 0x0A, 0x08, 0x07,
	    0x06, 0x06, 0x05, 0x04, 0x03, 0x03, 0x02, 0x02, 0x02, 0x01, 0x01, 0x01,
	    0x01, 0x01, 0x01, 0x01, 0x02, 0x02, 0x02, 0x03, 0x03, 0x04, 0x05, 0x06,
	    0x06, 0x07, 0x08, 0x0A, 0x0B, 0x0C, 0x0D, 0x0F, 0x10, 0x12, 0x13, 0x15,
	    0x16, 0x18, 0x1A, 0x1C, 0x1E, 0x20, 0x22, 0x24, 0x26, 0x29, 0x2B, 0x2D,
	    0x30, 0x32, 0x35, 0x37, 0x3A, 0x3C, 0x3F, 0x42, 0x44, 0x47, 0x4A, 0x4D,
	    0x50, 0x53, 0x56, 0x59, 0x5B, 0x5E, 0x62, 0x65, 0x68, 0x6B, 0x6E, 0x71,
	    0x74, 0x77, 0x7A, 0x7D
};

//uint8_t squareTable[256];
//uint8_t sawTable[256];
uint8_t triangleTable[256];
uint8_t triangle4BitTable[256];

uint32_t noteTable[] = 
{   /* STARTING WITH C */
	/* OCTAVE 0 */
	16, 17, 18, 19, 21, 22, 23, 24, 26, 27, 29, 31,
	/* OCTAVE 1 */
	33, 35, 37, 39, 41, 44, 46, 49, 52, 55, 58, 62,
	/* OCTAVE 2 */
	65, 69, 73, 78, 82, 87, 92, 98, 104, 110, 116, 123,
	/* OCTAVE 3 */
	131, 139, 147, 155, 165, 175, 185, 196, 208, 220, 233, 245,
	/* OCTAVE 4 */
	262, 277, 294, 311, 330, 349, 370, 392, 415, 440, 466, 494,
	/* OCTAVE 5 */
	523, 554, 587, 622, 659, 698, 740, 784, 831, 880, 932, 988,
	/* OCTAVE 6 */
	1046, 1109, 1175, 1244, 1328, 1397, 1480, 1568, 1661, 1760, 1865, 1975,
	/* OCTAVE 7 */
	2093, 2217, 2349, 2489, 2637, 2793, 2959, 3135, 3322, 3520, 3729, 3951,
	/* OCTAVE 8 */
	4186, 4434, 3698, 4978, 5274, 5587, 5919, 6271, 6644, 7040, 7458, 7902,
	/* OCTAVE 9 */
	8372, 8869, 9397, 9956, 10548, 11175, 11839, 12543, 13289, 14080, 14917, 15804
};

void WaveformTablesInitialize()
{
	uint16_t i;
	
	//wave form names
	
	
	for(i = 0; i<256; i++)
	{
		//squareTable[i] = (i>>7) ? 0xFFF : 0x000;
		//sawTable[i] = 0xFF-i;
		/** obnoxious formatting ahoy */
		triangleTable[i] = (i >= 0x80) ?
		          		//index is from 128-255
		          		//ramp up from 0 to 255
		          		//so (index - 127)*2
		          		((i - 0x80)<<1) :
		          		//index is from 0 to 127
		          		//ramp down from 255 to 0
		          		(0xFF-(i<<1)); //255 - index*2
		          		
		/** more obnoxius stuff */
		/** if not multiple of 16 then round up to next multiple of 16, otherwise same */
		
	}
	for(i = 0; i <256; i++)
	{
		triangle4BitTable[i]  = triangleTable[(i % 16) ? (((i/16)+1)*16) : i];
	}
}

uint16_t getSaw(uint32_t phase)
{
	return 0xFF-phase;
}

uint16_t getNoise()
{
	return shift(&lfsr32, POLYMASK32);	
}

uint32_t shift(uint32_t *lfsr, uint32_t polymask)
{
	int feedback;
	feedback = *lfsr & 1;
	*lfsr>>=1;
	if(feedback==1)
		*lfsr ^= polymask;
	return *lfsr;
}

uint32_t getIncrement(uint32_t desiredFrequency)
{
	//return (uint32_t)desiredFrequency * 22000;
	//return (uint32_t)((float)desiredFrequency * 2.67514677f);
	return (desiredFrequency * 0x10000)/22050;
}

