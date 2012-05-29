/*
 * WaveForms.h
 *
 *  Created on: Mar 11, 2012
 *      Author: bobomb
 */

#ifndef WAVEFORMS_H_
#define WAVEFORMS_H_

#include "PE_Types.h"
#define NUM_ELEMENTS 1816
extern const uint8_t sineTable[];

extern uint8_t squareTable[];
extern uint8_t sawTable[];
extern uint8_t triangleTable[];
extern uint8_t triangle4BitTable[];
extern  uint32_t noteTable[];
extern const uint8_t sumdatam[];
extern char * waveformNames[];

#define GET_SINE(phase) (sineTable[phase>>8])
#define GET_SQUARE(phase) (phase & 0x8000) ? 0xFF : 0x00
//#define GET_SQUARE(phase) ((phase>>8)>0x40) ? 0xFF : 0x00
//#define GET_SAWTOOTH(phase) getSaw(phase>>8)
#define GET_SAWTOOTH(phase) (0xFF - (phase>>8))
//#define GET_SAWTOOTH(phase) ((phase>>8)>0x80) ? 0xFF : 0x00
#define GET_TRIANGLE(phase) triangleTable[phase>>8]
//#define GET_TRIANGLE(phase) ((phase>>8)>0xC0) ? 0xFF : 0x00
#define GET_TRIANGLE4BIT(phase) triangle4BitTable[phase>>8]
#define GET_NOISE() getNoise()
#define GET_WAVENAME(wave) waveformNames[wave]


#define WAVE_SINE 0
#define WAVE_SQUARE 1
#define WAVE_SAWTOOTH 2
#define WAVE_TRIANGLE 3
#define WAVE_TRIANGLE4BIT 4
#define WAVE_NOISE 5
//the # of wavetypes i have defined
#define NUMBER_WAVETYPES 6


uint32_t getIncrement(uint32_t desiredFrequency);
uint16_t getNoise();
uint16_t getSaw(uint32_t phase);
uint32_t shift(uint32_t *lfsr, uint32_t polymask);
void WaveformTablesInitialize();


#endif /* WAVEFORMS_H_ */
