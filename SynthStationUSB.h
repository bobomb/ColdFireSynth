/*
 * SynthStationUSB.h
 *
 *  Created on: Mar 17, 2012
 *      Author: Mofidul I. Jamal
 */

#ifndef SYNTHSTATIONUSB_H_
#define SYNTHSTATIONUSB_H_
#include "inc\hostapi.h"
#include "inc\khci.h"




/** CONSTANTS */
#define MIDI_NOTE_ON			0x90
#define MIDI_NOTE_OFF			0x80
#define MIDI_CONTROL_CHANGE		0xB0
#define MIDI_PITCH_BEND			0xE0
/** FUNCTIONS FOR SYNTHSTATION USB KEYBOARD FILE */

/** Function to initialize the usb host and set it up for the synthstation keyboard */
void SynthStationUsbInitialize();

/** Callback function from the host driver whenever a USB event occurs */
void SynthStationUsbEvent(_usb_device_instance_handle, _usb_interface_descriptor_handle, uint32_t);

/** Function to perform USB related tasks for the keyboard */
void SynthStationUsbTask();

bool isBufferDataNew();

/** process the USB receive buffer data */
void processData();

/** Callback from USB host driver when a data receive transfer completes */
void UsbTransferCallback(_usb_pipe_handle  pipe, pointer param,pointer buffer,uint32_t len,uint32_t status);

/** Function to initialize data transfer request for the keyboard */
void SynthStationUsbRequestData();

/** struct to hold the midi message packet (4 bytes) */
typedef struct 
{
	byte b1;
	byte status;
	byte note;
	byte velocity;
} midiMessage_t;

/** this is the struct that contains the info about the synthstation keyboard
 ** and the callback functions to register 
 */

static USB_HOST_DRIVER_INFO DriverInfoTable[] =
{
   {
      {0x09,0xE8},                   /** Vendor ID for Akai*/             
      {0x00,0x7A},                   /** Product ID per synthstation USB kb */      
      USB_CLASS_AUDIO ,      	     /** Class code  */                     
      USB_SUBCLASS_AUD_MIDI_STRM,    /** Sub-Class code   */                
      0x00,                          /** Protocol      */                   
      0,                             /** Reserved       */                  
      SynthStationUsbEvent   		 /** Application call back function   */
   },
   {
      {0x00,0x00},                   /** All-zero entry terminates */        
      {0x00,0x00},                   /** driver info list.           */     
      0,
      0,
      0,
      0,
      NULL
   }
};

#endif /* SYNTHSTATIONUSB_H_ */
