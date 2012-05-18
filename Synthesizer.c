/*
 * Synthesizer.c
 *
 *  Created on: Apr 18, 2012
 *      Author: bobomb
 */
#include "Synthesizer.h"

uint8_t currentOctave = 4;
uint16_t currentPitchBend = 0x2000;
NoteKey keyStatus[25];

uint8_t currentSelectedWaveForm = WAVE_SQUARE;
uint16_t currentSelectedMode;

LayerState synthLayers[8] = {0xFF,};
uint8_t currentLayer = LAYER_MELODY_1;

uint16_t beatsPerMinute;
uint16_t delayPerBeat;
uint16_t beatTicksElapsed;
uint8_t currentBeat;
noteListItem * noteList;

void initializeEverything()
{
	int i, j, k;
	uint32_t increment;
	LCDInitialize();
	SPI_Initialize();
	//* setup oscs
	initializeOscillators();
	// Setup the keyboard usb
	SynthStationUsbInitialize();
	//calculate the note frequencies
		WaveformTablesInitialize();
		
	/** Calculate frequency increments */
	for(i = 0; i < 120; i++)
	{
		increment = noteTable[i];
		noteTable[i] = getIncrement(increment);
	}
	
	/** Initialize sequencer layerw */
	
	for(i = 0; i < 8; i++)
	{
		/*
		synthLayers[i].notes[0].noteState = NOTE_NONE;
		synthLayers[i].notes[0].oscNum = 0xFF;
		synthLayers[i].notes[1].noteState = NOTE_NONE;
		synthLayers[i].notes[1].oscNum = 0xFF;
		
		synthLayers[i].notes[2].noteState = NOTE_NONE;
		synthLayers[i].notes[2].oscNum = 0xFF;
		
		synthLayers[i].notes[3].noteState = NOTE_NONE;
		synthLayers[i].notes[3].oscNum = 0xFF;
		*/
		
		for(j = 0; j < SEQUENCER_STEPS; j++)
		{
			
			synthLayers[i].steps[j].noteOn = 0xFF;
			synthLayers[i].steps[j].noteOffIndex = 0;
			for(k = 0; k < 4; k++)
			{
				synthLayers[i].steps[j].noteOff[k] = 0x00;
			}
		}
		

	}
	/** Initialize keystatus bits */
	for(i = 0; i < 25; i++)
	{
		keyStatus[i].phaseIncrement = 0;
		keyStatus[i].oscNum = 0xFF;
		keyStatus[i].noteState = NOTE_NONE;
	}
	currentSelectedMode = SYNTH_RECORDING;
	setBPM(DEFAULT_BPM);
	
	BEAT_LED_START_DD = BEAT_LED_DD = 1;
	BEAT_LED_START = BEAT_LED = 0;
	
	listTest();
	
}

void noteEvent(uint16_t noteNumber, uint8_t eventType)
{
	uint8_t newOsc;
	Oscillator * pOsc;
	NoteKey * pNote;

	/** 1. Check current mode first
	 * 
	 */	
	pNote = &keyStatus[noteNumber - currentOctave*NOTES_PER_OCTAVE];
	switch(currentSelectedMode)
	{
		case SYNTH_FREESTYLE:
		{
			switch(eventType)
			{
				
				/** Note button is pressed down */
				case NOTE_EVENT_ON:
				{
					switch(currentLayer)
					{
						case LAYER_MELODY_1:
						case LAYER_MELODY_2:
						{
							/** Melody 1 and 2 follow the same logic */
							
							if(pNote->noteState != NOTE_NONE)
							{
								/** The note is currently playing so retrigger it */
								retriggerNote(pNote);
							}
							else
							{
								/** The note is currently NOT playing so trigger it */
								newOsc = playNote(noteNumber, pNote);
							}
							break;
						} /** END CASE MELODY_1 MELODY_2 */
						
						case LAYER_CHORD_1:
						case LAYER_CHORD_2:
						{
							/** Chord 1 and 2 follow the same logic */
							break;
						}
						
						case LAYER_DRUM_1:
						case LAYER_DRUM_2:
						{
							/** Drum 1 and 2 follow the same logic */
							break;
						}
						
						case LAYER_EFFECTS:
						{
							break;
						}
						
						case LAYER_MACRO:
						{
							break;
						}
					} /** END SWITCH currentLayer */
					
					break;
				} /** END CASE eventType NOTE_EVENT_ON */
				
				/** Note button is released */
				case NOTE_EVENT_OFF:
				{
					/* Set note to release state of envelope*/
					pNote->noteState = NOTE_RELEASE;
					break;
				}
			}
			break;
		} /** END CASE SYNTH_FREESTYLE */

		case SYNTH_RECORDING:
		{
			/** We have to add this note to the current loop */
			switch(eventType)
			{
				//pNote = &keyStatus[noteNumber - currentOctave*NOTES_PER_OCTAVE];
				
				case NOTE_EVENT_ON:
				{
					/** 1. Check to see if there is a note currently playing in this spot */
					switch(currentLayer)
					{
						case LAYER_MELODY_1:
						case LAYER_MELODY_2:
						{
							/** Melody 1 and 2 follow the same logic */
								
								/** The note is currently NOT playing so trigger it as well*/
							/*	
							newOsc = playNote(noteNumber, pNote);
								synthLayers[currentLayer].steps[currentBeat].noteOn = noteNumber;
								*/
							
							
								
							break;
						}
						
						case LAYER_CHORD_1:
						case LAYER_CHORD_2:
						{
							/** Chord 1 and 2 follow the same logic */
							break;
						}
						
						case LAYER_DRUM_1:
						case LAYER_DRUM_2:
						{
							/** Drum 1 and 2 follow the same logic */
							break;
						}
						
						case LAYER_EFFECTS:
						{
							break;
						}
						
						case LAYER_MACRO:
						{
							break;
						}
					} /** END SWITCH currentLayer */
					
					break;
				} /** END CASE NOTE_EVENT_ON */
				
				case NOTE_EVENT_OFF:
				{
					switch(currentLayer)
					{
						case LAYER_MELODY_1:
						case LAYER_MELODY_2:
						{
							/** Melody 1 and 2 follow the same logic */
								
								/** The note is currently NOT playing so trigger it as well*/
								synthLayers[currentLayer].steps[currentBeat].noteOff[synthLayers[currentLayer].steps[currentBeat].noteOffIndex] = noteNumber;
								synthLayers[currentLayer].steps[currentBeat].noteOffIndex++;
								pNote->noteState = NOTE_RELEASE;
								
							break;
						}
						
						case LAYER_CHORD_1:
						case LAYER_CHORD_2:
						{
							/** Chord 1 and 2 follow the same logic */
							break;
						}
						
						case LAYER_DRUM_1:
						case LAYER_DRUM_2:
						{
							/** Drum 1 and 2 follow the same logic */
							break;
						}
						
						case LAYER_EFFECTS:
						{
							break;
						}
						
						case LAYER_MACRO:
						{
							break;
						}
					} /** END SWITCH currentLayer of SYNTH RECORDING note OFF*/
					break;
				}
			} /** END SWITCH eventType */
		} /* END CASE SYNTH_RECORDING */
		break;
	} /* END SWITCH currentMode */
	
}

/** Updates the notes that are currently pressed down
 * changes their parameters and such
 */

void updateKeyboardNotes()
{
	/** Called at 1000 Hz */
	int i;
	for(i = 0; i < 25; i++)
	{
		/** If the note is in ADSR */
		if(keyStatus[i].noteState != NOTE_NONE)
		{
			/** Update the ADSR properties here */
			if(keyStatus[i].pOsc->flags & OSC_FLAGS_PITCHBEND)
			{

				keyStatus[i].pOsc->phaseIncrement = keyStatus[i].pOsc->bendFrequency;
			}
			
			switch(keyStatus[i].noteState)
			{
			case NOTE_ATTACK:
			{
				keyStatus[i].pOsc->amplitude = 0xFF;
				keyStatus[i].noteState = NOTE_SUSTAIN;
				keyStatus[i].pOsc->noteState  = NOTE_SUSTAIN;
				break;
			}
			
			case NOTE_DECAY:
			{
				break;
			}
			
			case NOTE_SUSTAIN:
			{
				break;
			}
			
			case NOTE_RELEASE:
			{
				keyStatus[i].pOsc->enabled = FALSE;
				keyStatus[i].noteState = NOTE_NONE;
				keyStatus[i].pOsc->noteState  = NOTE_NONE;
				pushOsc(keyStatus[i].oscNum);
				break;
				
			}
			};
			
		}
	}
	
}

/** retriggers a note that is already being played */
void retriggerNote(NoteKey * pNote)
{
	pNote->noteState = NOTE_ATTACK;
	pNote->pOsc->phaseCounter = 0;
	pNote->pOsc->phaseIncrement = pNote->pOsc->frequency;
	pNote->pOsc->amplitude = 0;
	pNote->pOsc->noteState = NOTE_ATTACK;
}

/** Play note 
 * Give it an index into the table of notes, it will setup that note
 * and return the oscillator assigned to it
 */


uint8_t playNote(uint8_t noteIndex, NoteKey * pNote)
{
	uint8_t newOsc = 0xFF;
	Oscillator * pNewOsc;
	newOsc = popOsc();
	if(newOsc != 0xFF)
	{
		pNewOsc = &Oscillators[newOsc];
		pNewOsc->amplitude = 0;
		pNewOsc->enabled = TRUE;
		pNewOsc->phaseCounter = 0;
		pNewOsc->phaseIncrement = noteTable[noteIndex];
		pNewOsc->frequency =  noteTable[noteIndex];
		pNewOsc->noteNumber = noteIndex;
		pNewOsc->waveForm = currentSelectedWaveForm;
		pNewOsc->noteState = NOTE_ATTACK;
		pNote->noteState = NOTE_ATTACK;
		pNote->oscNum = newOsc;
		pNote->pOsc = pNewOsc;
		pNewOsc->parentId = 0xFFFF;
		pitchBend(currentPitchBend);
	}
	
	return newOsc;
}


/*
uint8_t playNote(uint16_t noteIndex);
{
	NoteKey * pNewNote;
	pNewNote = reateNote(uint8_t oscNum, Oscillator * pOsc, uint32_t phaseIncrement, uint8_t noteState);
	
}
*/
uint8_t playSequencerNote(uint8_t noteIndex)
{
	uint8_t newOsc = 0xFF;
	Oscillator * pNewOsc;
	newOsc = popOsc();
	if(newOsc != 0xFF)
	{
		pNewOsc = &Oscillators[newOsc];
		pNewOsc->amplitude = 0;
		pNewOsc->enabled = TRUE;
		pNewOsc->phaseCounter = 0;
		pNewOsc->phaseIncrement = noteTable[noteIndex];
		pNewOsc->frequency =  noteTable[noteIndex];
		pNewOsc->noteNumber = noteIndex;
		pNewOsc->waveForm = currentSelectedWaveForm;
		pNewOsc->noteState = NOTE_ATTACK;
		pNewOsc->parentId = 0x6F00 | noteIndex;
		pitchBend(currentPitchBend);
	}
	
	return newOsc;
}

uint8_t endSequencerNote(uint8_t noteIndex);
void updateSynthesizer()
{
	beatTicksElapsed++;
	updateKeyboardNotes();
	
	if(beatTicksElapsed == delayPerBeat)
	{
		beatTicksElapsed = 0;
		updateSequencerNotes();
	}
	
	updateLED();
}

void updateLED()
{
	if(beatTicksElapsed == 0 )
	{
		/** New beat, turn LED on and set timer to turn it off */
		if(currentBeat == 0)
			BEAT_LED_START = LED_ON;
		
		BEAT_LED = LED_ON;
	}
	else if(beatTicksElapsed == BEAT_LED_DELAY_MS)
	{
		BEAT_LED = LED_OFF;
		BEAT_LED_START = LED_OFF;
	}
}

/** Sequencer logic goes here */
void updateSequencerNotes()
{
	LayerState ls = synthLayers[currentLayer];
	NoteKey * pNote;
	Oscillator * pOsc;
	int i, j;
	int id;
	                            
	currentBeat++;
	if(currentBeat == SEQUENCER_STEPS)
	{
		/** This is the tick that starts from the beginning */
		currentBeat = 0;
		
	}
	/** 1/16th a beat has elapsed */
	
	if(ls.steps[currentBeat].noteOn != 0xFF)
	{
		playSequencerNote(ls.steps[currentBeat].noteOn);
	}
	
	for(i = 0; i < MAX_OSCILLATORS; i++)
	{
		pOsc = &Oscillators[i];
		if((pOsc->parentId & 0xFF00) == 0x6F00)
		{
			/** Check to see if we can turn a note off, if soo then do it */
			for(j = 0; j < ls.steps[currentBeat].noteOffIndex; j++)
			if(ls.steps[currentBeat].noteOff[j] != 0xFF)
			{
				if((pOsc->parentId & 0x00FF) == ls.steps[currentBeat].noteOff[j])
				{
					pOsc->noteState = NOTE_RELEASE;
				}
			}
			
				if(pOsc->noteState != NOTE_NONE)
				{
					/** Update the ADSR properties here */
					if(pOsc->flags & OSC_FLAGS_PITCHBEND)
					{
	
						pOsc->phaseIncrement = pOsc->bendFrequency;
					}
					
					switch(pOsc->noteState)
					{
						case NOTE_ATTACK:
						{
							pOsc->amplitude = 0xFF;
							pOsc->noteState = NOTE_SUSTAIN;
							break;
						}
						
						case NOTE_DECAY:
						{
							break;
						}
						
						case NOTE_SUSTAIN:
						{
							break;
						}
						
						case NOTE_RELEASE:
						{
							pOsc->enabled = FALSE;
							pOsc->noteState = NOTE_NONE;
							pushOsc(pOsc->oscNum);
							break;
						}
					};
				} /** End switch on keystate */
		}
	} /** END for loop trough Oscillators[] */
}

void notePress(uint16_t noteNumber, uint8_t noteVelocity)
{
	
}
void noteRelease(uint16_t noteNumber, uint8_t noteVelocity)
{
	
}

void controlChange(uint8_t controlNumber, uint8_t controlValue)
{
	switch (controlNumber)
	{
		case PITCH_UP_BUTTON:
		{
			if(controlValue == CONTROL_ON)
			{
				if(currentOctave < 7)
					currentOctave++;
			}
			break;
		}
		case PITCH_DOWN_BUTTON:
		{
			if(controlValue == CONTROL_ON)
			{
				if(currentOctave > 0)
					currentOctave--;
			}
			break;
		}
	}
}
void pitchBend(uint16_t pitch)
{
	int i = 0;
	int pitchIndex = 0;
	currentPitchBend = pitch;
	for(i = 0; i < MAX_OSCILLATORS; i++)
	{
		if(Oscillators[i].enabled)
		{
			if(pitch > 0x2000)
			{
				/** Bend notes up to a max of 2 octaves, so note index + 24*/
				/** Subtract 0x2000 from pitch, multiply by 341 to get offset index to */
				/** Set the bend frequency to the proper note if its within our limits*/
				pitchIndex = ((pitch - 0x2000) / 682) + Oscillators[i].noteNumber;
				Oscillators[i].bendFrequency = (pitchIndex >= 120? noteTable[119] : noteTable[pitchIndex]);
				Oscillators[i].flags |= OSC_FLAGS_PITCHBEND;
				
			}
			else if(pitch < 0x2000)
			{
				/** Bend notes down up to a max of 2 octaves*/
				pitchIndex = Oscillators[i].noteNumber - ((0x2000 - pitch) / 682);
				Oscillators[i].bendFrequency = (pitchIndex < 0 ? noteTable[0] : noteTable[pitchIndex]);
				Oscillators[i].flags |= OSC_FLAGS_PITCHBEND;
			}
			else
			{
				/** Back to normal */
				Oscillators[i].bendFrequency = Oscillators[i].frequency;
				//Oscillators[i].flags &= ~OSC_FLAGS_PITCHBEND;
				
			}
			
		}
	}

}
void listTest()
{
	NoteKey * n1;
	NoteKey * n2;
	NoteKey * n3;
	NoteKey * n4;
	noteList = (noteListItem *)malloc(sizeof(noteListItem));
	noteList->pNextItem = NULL;
	noteList->pNote = NULL;
	
	n1 = createNote(7, 0xFF, NOTE_NONE);
	n2 = createNote(6, 0x33, NOTE_NONE);
	n3 = createNote(5, 0x84, NOTE_NONE);
	n4 = createNote(4, 0xF1, NOTE_NONE);
	
	addNoteItem(n1);
	addNoteItem(n3);
	addNoteItem(n2);
	removeNoteItem(n3);
	addNoteItem(n4);
	removeNoteItem(n1);
	addNoteItem(n3);
	
}

/** Create a new notekey object given an oscillator, phase and state */
NoteKey * createNote(uint8_t oscNum, uint32_t phaseIncrement, uint8_t noteState)
{
	NoteKey * pNewNote;
	pNewNote = (NoteKey *)malloc(sizeof(NoteKey));
	
	pNewNote->oscNum = oscNum;
	pNewNote->pOsc = &Oscillators[oscNum];
	pNewNote->phaseIncrement = phaseIncrement;
	pNewNote->noteState = noteState;
	return pNewNote;
}

/** Adds a notekey object to the linked list of notes in use */
void addNoteItem(NoteKey * pItem)
{
	noteListItem * pLastItem;
	noteListItem * pNewItem;
	pLastItem = noteList;
	while(pLastItem->pNextItem)
	{
		pLastItem = pLastItem->pNextItem;
	}
	
	if((pLastItem == noteList) && !noteList->pNote)
	{
		noteList->pNote = pItem;
		return;
	}
	/** lastItem now points to the laste item in the list */
	pNewItem = (noteListItem*)malloc(sizeof(noteListItem));
	pLastItem->pNextItem = pNewItem;
	pNewItem->pNote = pItem;
	pNewItem->pNextItem = NULL;
}

void removeNoteItem(NoteKey * pItem)
{
	noteListItem * pCurrentItem = noteList;
	noteListItem * pLastItem = NULL;
	
	while(pCurrentItem)
	{
		if(pCurrentItem->pNote == pItem)
		{
			if(pCurrentItem == noteList)
			{
				/** first item is removed */
				free(pCurrentItem->pNote);
				pCurrentItem->pNote = NULL;
				pLastItem = pCurrentItem->pNextItem;
				free(pCurrentItem);
				noteList = pLastItem;
				return;
			}
			/** delete this itm, destroy the notekey */
			if(pLastItem)
			{
				pLastItem->pNextItem = pCurrentItem->pNextItem;
				
				free(pCurrentItem->pNote);
				free(pCurrentItem);
				
			}
			return;
		}
		pLastItem = pCurrentItem;
		pCurrentItem = pCurrentItem->pNextItem;
	}
}

void setBPM(uint16_t bpm)
{
	beatsPerMinute = bpm;
	/* 60,000 ms in 1 min / beats per minute = (ms/min)* (min/beat) = milliseconds interval per beat */
	/* Each beat is divided into 4 sub beats to give us 16 steps per 4 beat bar */
	delayPerBeat = (60000/(beatsPerMinute))/4;
	beatTicksElapsed = 0;
	currentBeat = 0;
}
