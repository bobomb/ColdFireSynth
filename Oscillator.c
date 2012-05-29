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
Oscillator * oscillatorStack[MAX_OSCILLATORS];
uint8_t topOfStack=0xFF; //0xFF = uninitialized
Envelope currentEnvelope = {255, 3, 10, 1}; 

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
		Oscillators[i].noteId = 0xFFFF;
		Oscillators[i].attack = currentEnvelope.A;
		Oscillators[i].decay = currentEnvelope.D;
		Oscillators[i].sustain = currentEnvelope.S;
		Oscillators[i].release = currentEnvelope.R;
		Oscillators[i].adsrCounter = 0;
		Oscillators[i].velocity = 0xFF;
		Oscillators[i].oscNum = i;
		pushOsc(&Oscillators[i]);
	}
	i=1;
}

void pushOsc(Oscillator * pOsc)
{
	if(topOfStack == 0xFF) /* uninitialized case */
	{
		oscillatorStack[0] = pOsc;
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
			oscillatorStack[++topOfStack] = pOsc;
			pOsc->flags &= 0;
			pOsc->noteId = 0xFFFF;
			numOscInUse--;
		}
	}
}
 
Oscillator * popOsc()
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
			return 0;
		}
	}
}

void setFrequency(uint8_t oscNumber, uint16_t frequency)
{
	Oscillators[oscNumber].phaseIncrement = getIncrement(frequency);
	Oscillators[oscNumber].phaseCounter = 0;
	Oscillators[oscNumber].frequency = frequency;
	
}
