/*
 * Synthesizer.c
 *
 *  Created on: Apr 18, 2012
 *      Author: bobomb
 */
#include "Synthesizer.h"

uint8_t currentOctave = 4;
uint16_t currentPitchBend = 0x2000;

uint8_t currentSelectedWaveForm = WAVE_SQUARE;
uint16_t currentSelectedMode;

LayerState synthLayers[LAYER_COUNT] = {0x00,};
uint8_t currentLayer = LAYER_MELODY_1;

uint16_t beatsPerMinute;
uint16_t delayPerBeat;
uint16_t delayPerSubBeat;
uint16_t beatTicksElapsed;
uint16_t subBeatTicksElapsed;
uint8_t currentBeat;
noteListItem * noteListHead;
noteListItem * noteListTail;
uint8_t currentNoteDown = 0xFF;
uint16_t keyStatus[25] = {0,};
uint8_t g_i =0;

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
		for(j = 0; j < SEQUENCER_STEPS; j++)
		{
				synthLayers[i].sequenceNotes[j] = 0xFFFF;
		}
		synthLayers[i].waveType = currentSelectedWaveForm;
	}
	
	synthLayers[0].layerFlags |= LAYER_FLAGS_ENABLED;
	synthLayers[1].layerFlags |= LAYER_FLAGS_ENABLED;
	
	noteListHead = (noteListItem *)malloc(sizeof(noteListItem));
	noteListHead->pNextItem = NULL;
	noteListHead->pNote = NULL;
	noteListTail = noteListHead;
		
	currentSelectedMode = SYNTH_RECORDING;
	setBPM(DEFAULT_BPM);
	
	BEAT_LED_START_DD = BEAT_LED_DD = 1;
	BEAT_LED_START = BEAT_LED = 0;
	
	//listTest();
	
}

void noteEvent(uint16_t noteNumber, uint8_t eventType)
{
	Oscillator * pOsc = NULL;
	NoteKey * pNote = NULL;

	/** 1. Check current mode first
	 * 
	 */	
	//pNote = &keyStatus[noteNumber - currentOctave*NOTES_PER_OCTAVE];
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
							newPlayNote(noteNumber, NOTE_SRC_KB);
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
					pNote = findNote(NOTE_ID(NOTE_SRC_KB, noteNumber));
					
					if(pNote)
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
							/** The note is currently NOT playing so trigger it for the keyboard */
							pNote = newPlayNote(noteNumber, NOTE_SRC_KB);
							addSequencerNote(noteNumber);
							currentNoteDown = noteNumber;
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
							/* Set note to release state of envelope*/
							pNote = findNote(NOTE_ID(NOTE_SRC_KB, noteNumber));
							
							if(pNote)
								pNote->noteState = NOTE_RELEASE;
							
							currentNoteDown = 0xFF;
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

/** event for control change such as the different buttons on the synthstation25 */
void controlChange(uint8_t controlNumber, uint8_t controlValue)
{
	/** Test for control on only since 2 control change events are recieved, one for button down and another
	 * for button up
	 */
	if(controlValue == CONTROL_ON)
	{
		switch (controlNumber) 
		{
			case PITCH_UP_BUTTON: 
			{
				if(currentOctave < 7)
					currentOctave++;
				break;
			}
			
			case PITCH_DOWN_BUTTON: 
			{
				if(currentOctave > 0)
					currentOctave--;
				break;
			}
			
			case DRUMS_BUTTON: 
			{
				if(currentSelectedMode == SYNTH_RECORDING)
					currentSelectedMode = SYNTH_FREESTYLE;
				else
					currentSelectedMode = SYNTH_RECORDING;
				
				break;
			}
			
			case SYNTH1_BUTTON:	
			{
				currentSelectedWaveForm++;
				if(currentSelectedWaveForm == NUMBER_WAVETYPES-1)
					currentSelectedWaveForm = 0;
				
				synthLayers[currentLayer].waveType = currentSelectedWaveForm;
				break;
			}
			
			case SYNTH2_BUTTON:
			{
				currentLayer++;
				if(currentLayer == LAYER_COUNT)
					currentLayer = 0;
				
				break;
			}
		}
	}
}

/** Updates the notes that are currently pressed down
 * changes their parameters and such
 */
void updateKeyboardNotes()
{
	/** Called at 1000 Hz */
	int i;
	noteListItem * pKbNote;
	noteListItem * pKbNoteNext;
	NoteKey * pNote;
	Oscillator * pOsc;
	pKbNote = noteListHead->pNextItem;

	while(pKbNote)
	{
		pKbNoteNext = NULL;
		pNote = pKbNote->pNote;
		pOsc = pNote->pOsc;
			if(pNote->noteState != NOTE_NONE)
			{
				/** Update the ADSR properties here */
				if(pOsc->flags & OSC_FLAGS_PITCHBEND)
				{
					pOsc->phaseIncrement = pOsc->bendFrequency;
				}
				
				switch(pNote->noteState)
				{
					case NOTE_ATTACK:
					{
						pOsc->amplitude = 0xFF;
						pNote->noteState = NOTE_SUSTAIN;
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
						pNote->noteState = NOTE_NONE;
						pKbNoteNext = pKbNote->pNextItem;
						removeNoteItem(pNote);
						break;
					}
				};
				
			}
					
		if(!pKbNoteNext)
			pKbNote = pKbNote->pNextItem;
		else
			pKbNote = pKbNoteNext;
	};
	


}


/** retriggers a note that is already being played */
void retriggerNote(NoteKey * pNote, uint8_t noteIndex, uint8_t source)
{
	Oscillator * pNewOsc = pNote->pOsc;
	//pNote->noteState = NOTE_ATTACK;
	//pNewOsc->amplitude = 0;
	pNewOsc->enabled = TRUE;
	//pNewOsc->phaseCounter = 0;
	pNewOsc->phaseIncrement = noteTable[noteIndex];
	pNewOsc->frequency =  noteTable[noteIndex];
	pNewOsc->noteNumber = noteIndex;
	
	if((source & NOTE_SRC_MASK) == NOTE_SRC_LAYER)
		pNewOsc->waveForm = synthLayers[source & NOTE_LAYER_MASK].waveType;
	else
		pNewOsc->waveForm = currentSelectedWaveForm;
	
	pNewOsc->noteId = (uint16_t)((source<<8) | noteIndex);
	pNote->noteId = (uint16_t)((source<<8) | noteIndex);
}

/** Gets and Osc, sets up a new NoteKey structure, assigns stuff and adds to the note list */
NoteKey * newPlayNote(uint8_t noteIndex, uint8_t source)
{
	NoteKey * pNewNote;
	Oscillator * pNewOsc;
	pNewOsc = popOsc();
	if(pNewOsc)
	{
		pNewNote = createNote(pNewOsc, noteTable[noteIndex], NOTE_ATTACK);
		pNewOsc->amplitude = 0;
		pNewOsc->enabled = TRUE;
		pNewOsc->phaseCounter = 0;
		pNewOsc->phaseIncrement = noteTable[noteIndex];
		pNewOsc->frequency =  noteTable[noteIndex];
		pNewOsc->noteNumber = noteIndex;
		if((source & NOTE_SRC_MASK) == NOTE_SRC_LAYER)
			pNewOsc->waveForm = synthLayers[source & NOTE_LAYER_MASK].waveType;
		else
			pNewOsc->waveForm = currentSelectedWaveForm;
		pNewOsc->noteId = NOTE_ID(source, noteIndex);
		pNewNote->noteId = NOTE_ID(source, noteIndex);
		addNoteItem(pNewNote);
		pitchBend(currentPitchBend);
		return pNewNote;
	}
	return NULL;
}

uint8_t endSequencerNote(uint8_t noteIndex);

void updateSynthesizer()
{
	updateLED();
	updateKeyboardNotes();
	beatTicksElapsed++;
	subBeatTicksElapsed++;
	
	if(beatTicksElapsed == delayPerBeat)
	{
		beatTicksElapsed = 0;
		subBeatTicksElapsed = 0;
		updateSequencerNotes();
	}
	
	if(subBeatTicksElapsed == delayPerSubBeat)
	{
		//updateSequencerNotes();
	}
}

/** Called each time we hit a beat*/
void updateSequencerNotes()
{
	/** reset flag which indicates that the current beat just started playing */
	//synthLayers[currentLayer].data[currentBeat].bStarted = FALSE;
	int i;
	uint16_t currentNote;
	NoteKey * pNote = NULL;
	/** Experimental code */

	currentBeat++;
	if(currentBeat == SEQUENCER_STEPS)
		currentBeat = 0;
	
	for(i = 0; i < LAYER_COUNT; i++)
	{
		if(synthLayers[i].layerFlags & LAYER_FLAGS_ENABLED)
		{
			currentNote = synthLayers[i].sequenceNotes[currentBeat];
			/** This is where we play a note and unplay a note */
			/** If the current layer does not have a osc assigned to it, then go ahead and assign one */
			if(currentNote != 0xFFFF)
			{
				if(!synthLayers[i].pLayerNote)
				{
					pNote = newPlayNote(currentNote, NOTE_SRC_LAYER | i);
					synthLayers[i].pLayerNote = pNote;
				}
				else
				{
					/** There is an oscillator currently assigned, reuse it */
					pNote = synthLayers[i].pLayerNote; 
					retriggerNote(pNote, currentNote, NOTE_SRC_LAYER | i);
				}
			}
			else
			{
				/** In this case there is no note to play, in which case we should unplay the last note if it was playing */
				if(synthLayers[i].pLayerNote)
					synthLayers[i].pLayerNote->pOsc->enabled = FALSE;
	
			}
		}
	}
	
	if((currentNoteDown != 0xFF) && (synthLayers[currentLayer].layerFlags & LAYER_FLAGS_ENABLED))
	{
		synthLayers[currentLayer].sequenceNotes[currentBeat] = currentNoteDown;
		if(synthLayers[currentLayer].pLayerNote)
			synthLayers[currentLayer].pLayerNote->pOsc->enabled = FALSE;
	}
	
	/** Increment beat counter */

	return;
}

void addSequencerNote(uint8_t note)
{
	synthLayers[currentLayer].sequenceNotes[currentBeat] = note;
	if(synthLayers[currentLayer].pLayerNote)
		synthLayers[currentLayer].pLayerNote->pOsc->enabled = FALSE;
	currentNoteDown = note;
}

/** Updates the LED timer to turn the beat led and loop start LED on and off */
void updateLED()
{
	if(!(currentBeat % 4))
	{
		/** New beat, turn LED on and set timer to turn it off */
		if(!currentBeat)
			BEAT_LED_START = LED_ON;
		
		BEAT_LED = LED_ON;
	}
	
	if(beatTicksElapsed == BEAT_LED_DELAY_MS)
	{
		BEAT_LED = LED_OFF;
		BEAT_LED_START = LED_OFF;
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

/** Just a function to test if my lists are working correctly */
void listTest()
{
	NoteKey * n1;
	NoteKey * n2;
	NoteKey * n3;
	NoteKey * n4;

	
	n1 = createNote(&Oscillators[0], 0xFF, NOTE_NONE);
	n2 = createNote(&Oscillators[0], 0x33, NOTE_NONE);
	n3 = createNote(&Oscillators[0], 0x84, NOTE_NONE);
	n4 = createNote(&Oscillators[0], 0xF1, NOTE_NONE);
	
	addNoteItem(n1);
	addNoteItem(n3);
	addNoteItem(n2);
	removeNoteItem(n2);
	addNoteItem(n4);
	removeNoteItem(n1);
	removeNoteItem(n3);
	removeNoteItem(n4);
	return;
	
}

/** Create a new notekey object given an oscillator, phase and state */
NoteKey * createNote(Oscillator * pOsc, uint32_t phaseIncrement, uint8_t noteState)
{
	NoteKey * pNewNote;
	pNewNote = (NoteKey *)malloc(sizeof(NoteKey));
	pNewNote->pOsc = pOsc;
	pNewNote->phaseIncrement = phaseIncrement;
	pNewNote->noteState = noteState;
	return pNewNote;
}

/** Adds a notekey object to the linked list of notes in use */
void addNoteItem(NoteKey * pItem)
{
	noteListItem * pNewItem;

	/** CASE 1: EMPTY LIST WITH NO FIRST ITEM. HEAD == TAIL */
	if(noteListHead == noteListTail)
	{
		pNewItem = (noteListItem*)malloc(sizeof(noteListItem));
		pNewItem->pNote = pItem;
		pNewItem->pNextItem = NULL;
		noteListTail = pNewItem;
		noteListHead->pNextItem = pNewItem;
		return;
	}
	else
	{
		/** CASE 2: NON EMPTY LIST, HEAD != TAIL */
		pNewItem = (noteListItem*)malloc(sizeof(noteListItem));
		pNewItem->pNote = pItem;
		pNewItem->pNextItem = NULL;
		noteListTail->pNextItem = pNewItem;
		noteListTail = pNewItem;
	}
}


/** Finds a specified note from the notelist. the note should include the source
 * returns null on failure */
NoteKey * findNote(uint16_t note)
{
	noteListItem * pCurrentItem = noteListHead->pNextItem;
	while(pCurrentItem)
	{
		if(pCurrentItem->pNote)
		{
			if(pCurrentItem->pNote->noteId == note)
				return pCurrentItem->pNote;
		}
		pCurrentItem = pCurrentItem->pNextItem;
	}
	
	return NULL;
}

/** Removes a note from the list */
void removeNoteItem(NoteKey * pItem)
{
	noteListItem * pCurrentItem = noteListHead->pNextItem;
	noteListItem * pLastItem = noteListHead;
	
	/** SANITY CHECK - HEAD == TAIL is EMPTY LIST */
	if(noteListHead == noteListTail)
		return;
	
	while(pCurrentItem)
	{
		if(pCurrentItem->pNote == pItem)
		{
			pushOsc(pCurrentItem->pNote->pOsc);
			/** CASE 1: LAST ITEM IN LIST */
			if(pCurrentItem == noteListTail)
			{
				free(pCurrentItem->pNote);
				free(pCurrentItem);
				noteListTail = pLastItem;
				noteListTail->pNextItem = NULL;
				return;
			}
			else
			{
				/** CASE 2: NOT LAST ITEM */
				pLastItem->pNextItem = pCurrentItem->pNextItem;
				free(pCurrentItem->pNote);
				free(pCurrentItem);
				return;
			}
		}
		pLastItem = pCurrentItem;
		pCurrentItem = pCurrentItem->pNextItem;
	}
}

/** Sets the time counters for the sequencer by taking a value for the beats per minute of the tempo */
void setBPM(uint16_t bpm)
{
	beatsPerMinute = bpm;
	/* 60,000 ms in 1 min / beats per minute = (ms/min)* (min/beat) = milliseconds interval per beat */
	/* Each beat is divided into 4 sub beats to give us 16 steps per 4 beat bar */
	delayPerBeat = ((60000/(beatsPerMinute))*BEATS_PER_BAR)/SEQUENCER_STEPS;
	delayPerSubBeat = delayPerBeat / SEQUENCER_SUBSTEPS;
	beatTicksElapsed = 0;
	subBeatTicksElapsed = 0;
	currentBeat = 0;
	updateSequencerNotes();
}
