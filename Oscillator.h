/*
 *
 *  Created on: Mar 22, 2012
 *      Author: bobomb
 */

#ifndef OSCILLATOR_H_
#define OSCILLATOR_H_

#include "PE_Types.h"
#include "WaveForms.h"

/* CONSTANTS */
#define MAX_OSCILLATORS 0x8

#define PARENT_NONE 0xFFFF
/* Channel formats are 0xPPNN
 * Where PP is the mask
 * and NN is the number of the key or channel
 */
#define PARENT_MASK_KEY 0x6F00
#define PARENT_MASK_CHANNEL 0x8800
#define OSC_FLAGS_PITCHBEND  0x01

//struct NoteKey;
/* TYPES */
typedef struct 
{
	bool enabled;
	uint8_t noteNumber;
	uint8_t waveForm;
	uint16_t phaseCounter;
	uint32_t phaseIncrement;
	uint32_t frequency;
	uint16_t bendFrequency;
	uint16_t noteId;
	int16_t amplitude;
	int16_t attack;
	int16_t decay;
	int16_t sustain;
	int16_t release;
	uint16_t adsrCounter;
	uint8_t velocity;
	uint16_t flags;
	uint8_t oscNum;
} Oscillator;

typedef struct 
{
	uint16_t A;
	uint16_t D;
	uint16_t S;
	uint16_t R;
} Envelope;

/* VARIABLES */

extern uint32_t oscUsageField;
extern uint8_t numOscInUse;

/* FUNCTIONS */
void initializeOscillators();
uint8_t getFreeOsc();
void freeOsc(uint8_t oscNumber);
void setFrequency(uint8_t oscNumber, uint16_t frequency);
void pushOsc(Oscillator * pOsc);
Oscillator * popOsc();
uint8_t getOscByParent(uint8_t parentID);

/* PANDAS */

#endif /* OSCILLATOR_H_ */
