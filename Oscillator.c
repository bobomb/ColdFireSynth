/*
 * Oscillator.c
 *
 *  Created on: Mar 22, 2012
 *      Author: bobomb
 */

#include "Oscillator.h"


Oscillator Oscillators[MAX_OSCILLATORS];
//this is a bitfield used to determine what oscillators are in use and the next free one
uint32_t oscUsageField = 0x0; 
//keeps track of number of used oscs
uint8_t numOscInUse = MAX_OSCILLATORS;
uint8_t oscillatorStack[MAX_OSCILLATORS];
uint8_t topOfStack=0xFF; //0xFF = uninitialized
Envelope currentEnvelope = {1, 20, 10, 1}; 

void initializeOscillators()
{
	int i;
	//* Setup oscillators
	for(i = 0; i < MAX_OSCILLATORS; i++)
	{
		Oscillators[i].enabled = FALSE;
		Oscillators[i].noteNumber = 0;
		Oscillators[i].frequency = 0;
		Oscillators[i].phaseCounter = 0;
		Oscillators[i].phaseIncrement = 0;
		Oscillators[i].waveForm = WAVE_SINE;
		Oscillators[i].parentId = 0xFFFF;
		Oscillators[i].attack = currentEnvelope.A;
		Oscillators[i].decay = currentEnvelope.D;
		Oscillators[i].sustain = currentEnvelope.S;
		Oscillators[i].release = currentEnvelope.R;
		Oscillators[i].adsrCounter = 0;
		Oscillators[i].velocity = 0xFF;
		Oscillators[i].oscNum = i;
		
		pushOsc(i);
	}
	i=1;
}

uint8_t getFreeOsc()
{
	uint8_t shiftCounter = 0;
	uint32_t oscFree;
	
	if(numOscInUse == MAX_OSCILLATORS)
		return 0xFF;
	
	oscFree = ~oscUsageField;
	//if a bit is 1 then the osc is in use, and if the bit is 0 then an osc is not in use
	//if we take the complement of this, we get a nonzero number if there are free oscs (as free oscs will  be 1, and non free 0)
	//so if all oscs are not free, everhythings is 0
	//now we have to find the first free osc by bitshifting
	for(shiftCounter = 0; shiftCounter < MAX_OSCILLATORS; shiftCounter++)
	{
		if(oscFree & 0x1)
		{
			numOscInUse++;
			oscUsageField |= (1<<shiftCounter);
			return shiftCounter;
		}
		else
		{
			oscFree >>=1; //shift 1 bit to right
		}
	}
	//if we got here then we got nothing
	return 0xFF; //no more oscs free
}

 
void pushOsc(uint8_t oscNum)
{
	if(topOfStack == 0xFF) /* uninitialized case */
	{
		oscillatorStack[0] = oscNum;
		topOfStack = 0;
		numOscInUse--;
	}
	else /* initialized case, 2 possibilities */
	{
		/* Possibility 1: Stack is full - This should NEVER happen */
		if((MAX_OSCILLATORS-1) == topOfStack)
		{
			//err
		}
		else
		{
						/* Possibility 2: Stack is not full, so push the osc back onto the stack */
			oscillatorStack[++topOfStack] = oscNum;
			Oscillators[oscNum].flags &= 0;
			Oscillators[oscNum].parentId = 0xFFFF;
			numOscInUse--;
		}
	}
}
 
uint8_t popOsc()
{	   
	/* Possibility 1: Stack only has 1 element on it */
	if(topOfStack == 0)
	{
		topOfStack = 0xFF;
		numOscInUse++;
		return oscillatorStack[0];
	}
	else
	{
		/* Possibility 2: Stack has more than 1 element */
		if(topOfStack != 0xFF)
		{
			numOscInUse++;
			return oscillatorStack[topOfStack--];
		}
		else
		{
			/* Possibility 3: Stack is uninitialized/empty */
			/* Return 0xFF to signify you got nothing */
			return 0xFF;
		}
	}
}

uint8_t getOscByParent(uint8_t parentID)
{
	int i;
	//if(numOscInUse > 0)
	{
		for(i = 0; i < MAX_OSCILLATORS; i++)
		{
			if(Oscillators[i].parentId == parentID)
				return i;
		}
	}
	
	return 0xFF;
}

void freeOsc(uint8_t oscNumber)
{
	oscUsageField &= (1<<oscNumber);
	numOscInUse--;
}

void setFrequency(uint8_t oscNumber, uint16_t frequency)
{
	Oscillators[oscNumber].phaseIncrement = getIncrement(frequency);
	Oscillators[oscNumber].phaseCounter = 0;
	Oscillators[oscNumber].frequency = frequency;
	
}
