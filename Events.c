/** ###################################################################
**     Filename  : Events.c
**     Project   : ProcessorExpert
**     Processor : MCF51JM64VLD
**     Component : Events
**     Version   : Driver 01.02
**     Compiler  : CodeWarrior ColdFireV1 C Compiler
**     Date/Time : 2011-10-02, 19:57, # CodeGen: 0
**     Abstract  :
**         This is user's event module.
**         Put your event handler code here.
**     Settings  :
**     Contents  :
**         No public methods
**
** ###################################################################*/
/* MODULE Events */

#include "Cpu.h"
#include "Events.h"

/* User includes (#include below this line is not maintained by Processor Expert) */
#include "SynthStationUSB.h"
#include "Oscillator.h"
#include "Synthesizer.h"


extern byte waveSelector;
int i;

extern uint8_t numOscInUse;
uint16_t arpeggioRate = 5000;
uint16_t arpeggioCounter = 0;
uint8_t arpeggioNoteIndex = 0;
uint8_t arpeggioNoteCounter = 0;
uint16_t LFOCounter = 0;

extern uint16_t beatTicks;
extern uint16_t beatTicksOff;
extern uint8_t beatCounter;
extern uint8_t currentSequence;

bool ADSR = TRUE;
extern NoteKey keyStatus[];
uint16_t outputValue;
uint8_t waveCounter = 0;


//frequency incrementer
//these values were found through trial and error
//17440, not 15625
//(2^16)/17440) = 3.756
/* incr = freq * (2^16 / 17440) 
 * So for 440Hz, 
 */

/* oscillator position */

/*
** ===================================================================
**     Event       :  TI1_OnInterrupt (module Events)
**
**     Component   :  TI1 [TimerInt]
**     Description :
**         When a timer interrupt occurs this event is called (only
**         when the component is enabled - <Enable> and the events are
**         enabled - <EnableEvent>). This event is enabled only if a
**         <interrupt service/event> is enabled.
**     Parameters  : None
**     Returns     : Nothing
** ===================================================================
*/
 void TI1_OnInterrupt(void)
{
	uint16_t outputSum = 0;
	SPI_Send((uint16_t)(outputValue) | 0x7000);
	outputValue = 0;

	for(i = 0; i < MAX_OSCILLATORS; i++)
	{
		if(Oscillators[i].enabled)
		{
			Oscillators[i].phaseCounter += Oscillators[i].phaseIncrement;
			outputSum += ((getWave(Oscillators[i].waveForm, Oscillators[i].phaseCounter))*(uint32_t)(Oscillators[i].amplitude))/0x000000FF;
			outputValue++;
			/* Use outputValue as a counter for the # of oscs we sampled */
		}
		
	}
	
	if(outputValue)
	{
		/* experimental */ 
		//outputSum /= outputValue;
		//outputSum <<=2;
		outputValue = outputSum;
	}
	
}

__inline uint16_t getWave(byte waveType, uint32_t pos)
{
	waveType = waveType % NUMBER_WAVETYPES;
	switch(waveType)
	{
		case WAVE_SINE:
			return GET_SINE(pos);
			break;
		case WAVE_SQUARE:
			return GET_SQUARE(pos);
			break;
		case WAVE_SAWTOOTH:
			return GET_SAWTOOTH(pos);
			break;
		case WAVE_TRIANGLE:
			return GET_TRIANGLE(pos);
			break;
		case WAVE_TRIANGLE4BIT:
			return GET_TRIANGLE4BIT(pos);
			break;
		case WAVE_NOISE:
			return GET_NOISE();
			break;
	}
}

/*
** ===================================================================
**     Event       :  TI2_OnInterrupt (module Events)
**
**     Component   :  TI2 [TimerInt]
**     Description :
**         When a timer interrupt occurs this event is called (only
**         when the component is enabled - <Enable> and the events are
**         enabled - <EnableEvent>). This event is enabled only if a
**         <interrupt service/event> is enabled.
**     Parameters  : None
**     Returns     : Nothing
** ===================================================================
*/

void TI2_OnInterrupt(void)
{
	updateSynthesizer();
}
