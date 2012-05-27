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
	arpeggioNoteCounter=0;
	
	LFOCounter +=30;
	for(i = 0; i < MAX_OSCILLATORS; i++)
	{
		if(Oscillators[i].enabled)
		{
			Oscillators[i].phaseCounter += Oscillators[i].phaseIncrement;
			if(numOscInUse>7)
			{
				if(arpeggioNoteIndex  == arpeggioNoteCounter)
				{
					outputSum += ((getWave(Oscillators[i].waveForm, Oscillators[i].phaseCounter)<<2)*Oscillators[i].amplitude)/0xFF;
					outputValue++;
				}
				
			}
			else
			{
				outputSum += ((getWave(Oscillators[i].waveForm, Oscillators[i].phaseCounter))*(uint32_t)(Oscillators[i].amplitude))/0x000000FF;
				outputValue++;
			}
			/* Use outputValue as a counter for the # of oscs we sampled */
			arpeggioNoteCounter++;
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

//void TI2_OnInterrupt(void)
//{
//  /* Write your code here ... */
//	
//	uint8_t noteId = 0;
//	uint8_t j = 0;
//	NoteKey * note;
//	Oscillator * currentOsc;
//	int i = 0;
//	
//	/* LED Display routine */
//	beatTicks++;
//	beatTicksBar++;
//	
//	if(beatTicksOff)
//	{
//		beatTicksOff--;
//	}
//	else
//	{
//		/* turn off LED */
//		BEAT_LED = 0x0;
//		BEAT_LED_START = 0x0;
//	}
//	if(beatTicks == beatsInterval/2)
//	{
//		/* 1 beat has occured */
//		beatTicksOff = 10; /* 10 ms to turnit off */
//		/* turn on LED */
//		if((beatsCounter % 2)==1)
//			BEAT_LED = 0x1;
//		/* reset beat ticks */
//		beatTicks = 0;
//		/* increment beats counter */
//		if(beatsCounter > (SEQUENCER_STEPS-2))
//		{
//			/* reset to 0 */
//			beatsCounter = 0;
//			BEAT_LED_START = 0x1;
//		}
//		else
//		{
//			beatsCounter++;
//		}
//		
//		/* since  a tick has occured, we have to turn a note on */
//	}
//	
//	/* Go through all the beats items and update as necessary
//	 * 
//	 */
//		if(ADSR)
//		{
//		noteId = nextSequenceIndex();
//		for(j = 0; j < 2; j++)
//		{
//		for(i = 0; i < SEQUENCER_STEPS; i++)
//		{
//			switch(Sequencer[j][i].flags)
//			{
//				case SEQUENCER_NOTE_RECORDING:
//				{
//					break;
//				}
//				
//				case SEQUENCER_NOTE_OFF:
//				{
//					if(i == beatsCounter)
//					{
//						if(beatTicks > Sequencer[j][i].noteOn)
//						{
//							Sequencer[j][i].flags = SEQUENCER_NOTE_PLAYING;
//							Sequencer[j][i].beatDuration = 0;
//							notePress(Sequencer[j][i].noteNumber, 0);
//						}
//					}
//					break;
//				}
//				case SEQUENCER_NOTE_PLAYING:
//				{
//					Sequencer[j][i].beatDuration++;
//					/* if time elapse OR next note is the same fucking note, then turn it off */
//					if(Sequencer[j][i].beatDuration > Sequencer[j][i].noteOff) //|| (Sequencer[currentSequence][i].noteNumber == Sequencer[currentSequence][noteId].noteNumber))
//					{
//						Sequencer[j][i].flags = SEQUENCER_NOTE_OFF;
//						Sequencer[j][i].beatDuration = 0;
//						noteRelease(Sequencer[j][i].noteNumber,0);
//					}
//					break;
//				}
//			}
//		} /* END INNER FOR */
//		} /* END OUTER FOR */
//	
//	} /* end seq */
//		
//	for(i = 0; i < MAX_OSCILLATORS; i++)
//	{
//		if(Oscillators[i].Enabled)
//		{
//			currentOsc = &Oscillators[i];
//			noteId = currentOsc->ParentId;
//			note = &keyStatus[noteId];
//			currentOsc->ADSRCounter++;
//			if(currentOsc->PhaseIncrement != currentOsc->noteSlur)
//			{
//				if(currentOsc->PhaseIncrement < currentOsc->noteSlur)
//					currentOsc->PhaseIncrement++;
//				else
//					currentOsc->PhaseIncrement--;
//				/*
//				if(((currentOsc->noteSlur)/512) == 0)
//					currentOsc->PhaseIncrement++;
//				else
//					currentOsc->PhaseIncrement += (currentOsc->noteSlur)/128;
//				
//				if(currentOsc->PhaseIncrement > currentOsc->noteSlur)
//					currentOsc->PhaseIncrement = currentOsc->noteSlur;
//					*/
//			}
//			
//			switch(note->noteState)
//			{
//				case NOTE_ATTACK:
//				{
//					if((currentOsc->ADSRCounter % currentOsc->Attack) == 0)
//					{
//						if(currentOsc->Amplitude < 0xFF)
//						{
//							currentOsc->Amplitude+=10;
//						}
//						else
//						{
//							currentOsc->Amplitude = 0xFF;
//							note->noteState = NOTE_DECAY;
//							currentOsc->ADSRCounter=0;
//						}
//					}
//				}
//				break;
//				
//				case NOTE_DECAY:
//				{
//					if((currentOsc->ADSRCounter % currentOsc->Decay) == 0)
//					{
//						if(currentOsc->Amplitude > 200)
//						{
//							currentOsc->Amplitude-=5;
//						}
//						else
//						{
//							currentOsc->Amplitude = 200;
//							note->noteState = NOTE_SUSTAIN;
//							currentOsc->ADSRCounter=0;
//						}
//					}
//				}
//				break;
//				
//				case NOTE_SUSTAIN:
//				{
//					/* STUFF */
//					/* If this fires every 10 MS @ 100HZ, Divide by 10 to get 10HZ LFO */
//					//currentOsc->Amplitude += note->VibratoRate;
//					if(GET_SINE(LFOCounter) > 0x80)
//					{
//						/* add*/
//						currentOsc->Amplitude++;
//					}
//					else
//					{
//						/* substract */
//						currentOsc->Amplitude--;
//					}
//					
//					if(currentOsc->Amplitude > 200)
//						currentOsc->Amplitude = 200;
//					else if (currentOsc->Amplitude <150)
//						currentOsc->Amplitude = 150;
//					
//				}
//				break;
//				
//				case NOTE_RELEASE:
//				{
//					if((currentOsc->ADSRCounter % currentOsc->Release) == 0)
//					{
//						if(currentOsc->Amplitude > 0)
//						{
//							currentOsc->Amplitude-=5;
//						}
//						else
//						{
//							/* Set amplitude to 0, disable the oscillator and set the note to nothingness */
//							currentOsc->Enabled = FALSE;
//							currentOsc->Amplitude = 0;
//							
//							currentOsc->ParentId = 0xFF;
//							note->noteState = NOTE_NONE;
//							pushOsc(note->oscNum);
//							note->oscNum = 0xFF;
//						}
//					}
//				}
//				break;
//			}/* END SWITCH */
//		} /* END IF ENABLED */
//	} /* END FOR LOOP */
//}


