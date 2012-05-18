/*
 * SynthStationUSB.c
 *
 *  Created on: Mar 17, 2012
 *      Author: bobomb
 */

#include "SynthStationUSB.h"
#include "Cpu.h"
#include "IO_Map.h"
#include "inc\hidef.h"
#include "inc\rtc.h"
#include "Synthesizer.h"

/** handle to store keyboard instance */
_usb_device_instance_handle 	keyboardDeviceHandle;
/** handle to store midi pipe */
_usb_pipe_handle				keyboardInputPipe;
/** data transfer parameter setup struct */
TR_INIT_PARAM_STRUCT 			dataTransferParams;
/** handle to usb driver instance */
_usb_host_handle     			UsbHostHandle;
/** 64 byte data recieve buffer & length */
volatile uint8_t buffer[64];
uint32_t bufferLength = 0;
/** flags for USB event and keyboard attachment state */
bool usbEventOccured = FALSE;
bool keyboardAttached = FALSE;
/** midi message buffer pointer and old buffer data */
midiMessage_t * pInputBuffer;
midiMessage_t  oldInputBuffer;

/** Function to initialize the usb host and set it up for the synthstation keyboard */
void SynthStationUsbInitialize()
{
   USB_STATUS           status = USB_OK;

   /** Initialize the timer for the USB host driver */
   TimerInit();
   
   /** disable interrupts before initializing usb host driver apis*/
   DisableInterrupts;
   
   /** initialize usb host API driver */
   status = _usb_host_init(0, 1024, &UsbHostHandle);
   
   if(status != USB_OK)
	   exit(1);
   
   /**register driver capabilities */
   status = _usb_host_driver_info_register(UsbHostHandle, DriverInfoTable);
   
   /**re-enable interrupts */
   EnableInterrupts;
   
}

/** Function to perform USB related tasks for the keyboard */
void SynthStationUsbTask()
{
	/** As per host api guide, call the host api's task function first */
	_usb_khci_task();
	
	/** Check to see if any data events have occured since last time */
	if(usbEventOccured)
	{
		//USB_lock();
		/** Data event occured, so process the data buffer */
		processData();
		//USB_unlock();
		/** Issue another transfer request since the previous one has completed */
		SynthStationUsbRequestData();
		/** Reset the event flag */
		usbEventOccured = FALSE;
	}
}

/** Function to initialize data transfer request for the keyboard */
__inline void SynthStationUsbRequestData()
{
	usb_hostdev_tr_init(&dataTransferParams, (tr_callback) UsbTransferCallback, NULL);
	dataTransferParams.RX_LENGTH = 64;
	dataTransferParams.RX_BUFFER = (byte*)buffer;
	_usb_host_recv_data(UsbHostHandle, keyboardInputPipe, &dataTransferParams );
}

/** Callback function from the host driver whenever a USB event occurs */
void SynthStationUsbEvent
	(
		_usb_device_instance_handle device_handle, 
		_usb_interface_descriptor_handle interface_handle, 
		uint_32 event_code
	)
{
	USB_STATUS status = USB_OK;
	
	switch(event_code)
	{
		case USB_CONFIG_EVENT:
			break;
			/* Keyboard has been attached */
		case USB_ATTACH_EVENT:
		{
				status =_usb_host_ch9_get_interface(device_handle, 1, buffer);
				keyboardAttached = TRUE;
				if(status == !USB_STATUS_TRANSFER_QUEUED)
				{
					exit(3);
					
				}
			break;
		}
			/** We get this event after getting an interface */
		case USB_INTF_EVENT:
		{
			if(keyboardAttached)
			{
				status = _usb_hostdev_select_interface(device_handle, interface_handle, NULL);
				keyboardInputPipe = _usb_hostdev_find_pipe_handle(device_handle, interface_handle, USB_BULK_PIPE, USB_RECV );
				if(keyboardInputPipe)
					usbEventOccured = TRUE;
			}
	
			break;
		}
		/** Keyboard has been detached */
		case USB_DETACH_EVENT:
		{
			keyboardAttached = FALSE;
			break;
		}
	}
	return;
}

/** Checks to see if recieved USB data is new data */
bool isBufferDataNew()
{
	return !(
			(pInputBuffer->b1 == oldInputBuffer.b1) && 
			(pInputBuffer->note == oldInputBuffer.note) &&
			(pInputBuffer->status == oldInputBuffer.status) &&
			(pInputBuffer->velocity == oldInputBuffer.velocity)
			);
}

/** process the USB receive buffer data */
void processData()
{
	uint32_t loopCounter = 0;
	
	if(bufferLength <= 64 && (bufferLength))
	{
		for(loopCounter = 0; loopCounter < (bufferLength/4); loopCounter++)
		{
			pInputBuffer = (midiMessage_t *)(buffer+(4*loopCounter));
			if(isBufferDataNew())
			{
				switch(pInputBuffer->status)
				{
					/** MIDI PITCH BEND */
					case MIDI_PITCH_BEND:
					{
						/** Pitch bend value is sent as a 14 bit number across 2 bytes
						 *  with the least significant byte first, then most significant
						 */
						pitchBend((uint16_t)(pInputBuffer->velocity<< 7) | pInputBuffer->note);
					}
					/** MIDI CONTROL CHANGE */
					case MIDI_CONTROL_CHANGE: 
					{
						controlChange(pInputBuffer->note, pInputBuffer->velocity);
						break;
					}
					/** MIDI NOTE ON */
					case MIDI_NOTE_ON:
					{
						noteEvent(pInputBuffer->note, 0x01);
						break;
					}
					/** MIDI NOTE OFF */
					case MIDI_NOTE_OFF:
					{
						noteEvent(pInputBuffer->note, 0x02);
						break;
					}
					break;
				}
				oldInputBuffer = *pInputBuffer;
			}
		}
	}	
}

/** Callback from USB host driver when a data receive transfer completes */
void UsbTransferCallback(_usb_pipe_handle  pipe, pointer param,pointer buffer,uint32_t len,uint32_t status)
{
	/** the unused macro helps get rid of the compiler warnings about unused variables */
	UNUSED(status)
	UNUSED(param)
	UNUSED(pipe)
	UNUSED(buffer)
	bufferLength = len;
	usbEventOccured = TRUE;
}
