/*
 * Synthesizer.h
 *
 *  Created on: Apr 18, 2012
 *      Author: bobomb
 */

#ifndef SYNTHESIZER_H_
#define SYNTHESIZER_H_
#include "SynthstationUSB.h"
#include "WaveForms.h"
#include "Oscillator.h"
#include "DAC.h"
#include "LCD.h"

/* CONSTANTS */
#define NOTES_PER_OCTAVE 0x0C
/** Pitch bend wheel values */
#define PITCH_BEND_MAX		0x3FFF
#define PITCH_BEND_MIN		0x0
#define PITCH_BEND_NONE		0x2000
#define PITCH_BEND_RATE		0x40
/** Button values for the keyboard */
#define DRUMS_BUTTON		0x53
#define SYNTH1_BUTTON		0x50
#define SYNTH2_BUTTON		0x51
#define SYNTH3_BUTTON		0x52
#define PROGRAM_PREV_BUTTON	0x55
#define	PROGRAM_NEXT_BUTTON	0x54
#define PITCH_UP_BUTTON		0x56
#define PITCH_DOWN_BUTTON	0x57
#define CONTROL_ON	0x7F
/** Note event types */
#define NOTE_EVENT_ON  0x01
#define NOTE_EVENT_OFF 0x02
/* States for a note*/
#define NOTE_ATTACK    0x01
#define NOTE_DECAY     0x02
#define NOTE_SUSTAIN   0x04
#define NOTE_RELEASE   0x08
#define NOTE_NONE	   0x00 /* the note is off */
/* Synthesizer states */
#define SYNTH_FREESTYLE 0x01
#define SYNTH_RECORDING 0x02
/* Synthesizer layer flags */
#define LAYER_FLAGS_ENABLED 0x01 /* ADSR on/off control */
#define LAYER_FLAGS_ADSR	0x02
/* Synthesizer layer numbers */
#define LAYER_MELODY_1	0x00
#define LAYER_MELODY_2	0x01
#define LAYER_CHORD_1	0x02
#define LAYER_CHORD_2	0x03
#define LAYER_DRUM_1	0x04
#define LAYER_DRUM_2	0x05
#define LAYER_EFFECTS	0x06
#define LAYER_MACRO		0x07


/* LED PIN STATES */
#define BEAT_LED	PTCD_PTCD4
#define BEAT_LED_DD	PTCDD_PTCDD4
#define BEAT_LED_START 	PTCD_PTCD5
#define BEAT_LED_START_DD	PTCDD_PTCDD5
#define LED_ON	0x01
#define LED_OFF	0x00;
#define BEAT_LED_DELAY_MS	20

/* Sequencer stuff */
#define DEFAULT_BPM 60
#define SEQUENCER_NOTE_RECORDING	0x1
#define SEQUENCER_NOTE_OFF			0x2
#define SEQUENCER_NOTE_PLAYING		0x4
#define SEQUENCER_STEPS				16
/* TYPES */

typedef struct 
{
	uint8_t oscNum;
	Oscillator * pOsc;
	uint32_t phaseIncrement;
	uint8_t noteState;
} NoteKey;

typedef struct
{
	uint8_t noteNumber;
	uint16_t noteOn;
	uint32_t noteOff;
	uint32_t beatDuration;
	uint8_t flags;
} SequencerNote;

typedef struct
{
	uint8_t noteOn;
	uint8_t noteOff[4];
	uint8_t noteOffIndex;
} SequenceStep;

typedef struct
{
	uint16_t layerFlags;
	SequenceStep steps[16];
} LayerState;


struct noteListItem 
{
	NoteKey * pNote;
	struct noteListItem * pNextItem;
};

typedef struct noteListItem noteListItem;


/* VARIABLES */
extern Oscillator Oscillators[];
extern uint8_t currentSelectedWaveForm;
extern uint16_t currentSelectedMode;
extern uint8_t waveSelector;
extern uint16_t beatsPerMinute;
extern uint16_t beatsInterval;
extern uint8_t beatsCounter;
extern uint16_t beatTicks;
extern uint16_t beatTicksOff;
extern uint32_t beatTicksBar;

/* FUNCTIONS */
void initializeEverything();
void notePress(uint16_t noteNumber, uint8_t noteVelocity);
void noteRelease(uint16_t noteNumber, uint8_t noteVelocity);
void noteEvent(uint16_t noteNumber, uint8_t eventType);
void retriggerNote(NoteKey * pNote);
uint8_t playNote(uint8_t noteIndex, NoteKey *pNote);
uint8_t playSequencerNote(uint8_t noteIndex);
uint8_t endSequencerNote(uint8_t noteIndex);

void removeNoteItem(NoteKey * pItem);
void addNoteItem(NoteKey * pItem);
NoteKey * createNote(uint8_t oscNum, uint32_t phaseIncrement, uint8_t noteState);

void listTest();
void updateSynthesizer();
void updateKeyboardNotes();
void updateSequencerNotes();
void updateLED();

void controlChange(uint8_t controlNumber, uint8_t controlValue);
void pitchBend(uint16_t pitch);
void setBPM(uint16_t bpm);
void sequencerNotePress(uint8_t noteNumber);
void sequencerNoteRelease(uint8_t noteNumber);
void sequenceNote();
uint8_t nextSequenceIndex();

#endif /* SYNTHESIZER_H_ */
