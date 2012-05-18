/** ###################################################################
**     Filename  : ProcessorExpert.c
**     Project   : ProcessorExpert
**     Processor : MCF51JM64VLD
**     Version   : Driver 01.00
**     Compiler  : CodeWarrior ColdFireV1 C Compiler
**     Date/Time : 2012-03-03, 16:15, # CodeGen: 59
**     Abstract  :
**         Main module.
**         This module contains user's application code.
**     Settings  :
**     Contents  :
**         No public methods
**
** ###################################################################*/
/* MODULE ProcessorExpert */


/* Including needed modules to compile this module/procedure */
#include "Cpu.h"
#include "Events.h"
#include "TI1.h"
#include "IV1.h"
#include "IV2.h"
#include "TI2.h"
/* Include shared modules, which are used for whole project */
#include "PE_Types.h"
#include "PE_Error.h"
#include "PE_Const.h"
#include "IO_Map.h"
#include "DAC.h"
#include "WaveForms.h"
#include "Synthesizer.h"


/* User includes (#include below this line is not maintained by Processor Expert) */
#define DUTY_FULL 		0x384

word counter = 0;
byte waveSelector = 1;
byte noteSelector = 0;
uint16_t sample = 0;
bool buttonState = FALSE;
bool buttonState2 = FALSE;
bool prevButtonState = FALSE;
bool buttonEnabler = FALSE;

//frequency incrementer
//these values were found through trial and error
//17440, not 15625
//(2^16)/17440) = 3.756
/* incr = freq * (2^16 / 17440) 
 * So for 440Hz, 
 */

//uint16_t incr = 3758;//1238;
//new scale with 22khz carrier wave

void main(void)
{
  /* Write your local variable definition here */
  /*** Processor Expert internal initialization. DON'T REMOVE THIS CODE!!! ***/
  PE_low_level_init();
  /*** End of Processor Expert internal initialization.                    ***/
  /* Write your code here */
  /* For example: for(;;) { } */
  
  initializeEverything();
  TI1_Enable();
  TI2_Enable();
  PTEDD_PTEDD7 = 1;
  PTED_PTED7 = 0;
  do
  {
	  counter++;
	  if(counter % 2 == 0)
	  SynthStationUsbTask();
  } while(TRUE);
  
  /*** Don't write any code pass this line, or it will be deleted during code generation. ***/
  /*** Processor Expert end of main routine. DON'T MODIFY THIS CODE!!! ***/
  for(;;){}
  /*** Processor Expert end of main routine. DON'T WRITE CODE BELOW!!! ***/
} /*** End of main routine. DO NOT MODIFY THIS TEXT!!! ***/

/* END ProcessorExpert */
/*
** ###################################################################
**
**     This file was created by Processor Expert 5.00 [04.48]
**     for the Freescale ColdFireV1 series of microcontrollers.
**
** ###################################################################
*/
