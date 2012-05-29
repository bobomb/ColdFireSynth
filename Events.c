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
#include "Oscillator.h"
#include "Synthesizer.h"


int i = 0;
int j = 0;
int k = 0;
uint16_t g_outputValue;
uint16_t g_outputSum = 0;

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

/** This interrupt is called at the sampling rate of 22,050 hZ
 * It's purpose is to send the last sample value to the DAC and
 * then to calculate the next sample value and store it
 */
 void TI1_OnInterrupt(void)
{
	/** As per the MCP4921 DAC instructions, the first 4 bits are control bits
	 * 0x7000 = 0 1 1 1 ...sample[12 bits] 
	 * bit 15 = unused
	 * bit 14 = buffered input on
	 * bit 13 = gain mode 1x on
	 * bit 12 = active low shutdown, on
	 * bits 11-0 = sample value
	 **/
	SPI_Send((uint16_t)(g_outputValue) | 0x7000);
	g_outputValue = 0;
	g_outputSum = 0;

	/** Loop through the oscillator list, checking to see if they are enabled */
	for(i = 0; i < MAX_OSCILLATORS; i++)
	{
		if(Oscillators[i].enabled)
		{
			/** If enabled, increment the phase counter */
			Oscillators[i].phaseCounter += Oscillators[i].phaseIncrement;
			/** Add the sample value for the wave to our running sample sum by first multiplying the sample
			 * by it's amplitude and then dividing by 0xFF. Amplitude ranges from 0-255. Max sample value ranges
			 * from 0 to 255. The final value that gets added to the sum is from 0-255 per oscillator
			 * The DAC accepts a 12 bit value, and it just so happens that 256 * 8 = 4096 = 12 bits
			 */
			g_outputSum += ((getWave(Oscillators[i].waveForm, Oscillators[i].phaseCounter))*(uint32_t)(Oscillators[i].amplitude))/0x000000FF;
			g_outputValue++;
			/* Use outputValue as a counter for the # of oscs we sampled...cheap trick */
		}	
	}
	
	if(g_outputValue)
		g_outputValue = g_outputSum;
	
}

__inline uint16_t getWave(byte waveType, uint32_t pos)
{
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
