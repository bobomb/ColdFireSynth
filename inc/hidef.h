/******************************************************/
/**
* @file hidef.h
* Machine/compiler dependent declarations.
*/
/*----------------------------------------------------
   Copyright (c) Freescale DevTech
               All rights reserved
                  Do not modify!
 *****************************************************/

#ifndef _H_HIDEF_
#define _H_HIDEF_

#ifdef __cplusplus
extern "C" {
#endif

#if defined(__CWCC__)
#pragma gcc_extensions on

/**** Version for ColFire V1 */
#include <stddef.h>

#if (defined _MK_xxx_H_)
#define __RESET_WATCHDOG()  (void)(WDOG_REFRESH = 0xA602, WDOG_REFRESH = 0xB480)
#define __UNLOCK_WATCHDOG() (void)(WDOG_UNLOCK = 0xC520, WDOG_UNLOCK = 0xD928)
#define EnableInterrupts  asm ("CPSIE i");
#define DisableInterrupts asm ("CPSID i");
#else
#define EnableInterrupts asm { move.w SR,D0; andi.l #0xF8FF,D0; move.w D0,SR;  }
  /*!< Macro to enable all interrupts. */

#define DisableInterrupts asm { move.w SR,D0; ori.l #0x0700,D0; move.w D0,SR;  }
  /*!< Macro to disable all interrupts. */
#endif
#elif defined(__IAR_SYSTEMS_ICC__)
#include <intrinsics.h>

#define __RESET_WATCHDOG()  (void)(WDOG_REFRESH = 0xA602, WDOG_REFRESH = 0xB480)
#define __UNLOCK_WATCHDOG() (void)(WDOG_UNLOCK = 0xC520, WDOG_UNLOCK = 0xD928)
  
#define EnableInterrupts  __enable_interrupt();
#define DisableInterrupts __disable_interrupt();
#endif
	
#ifdef __cplusplus
 }
#endif

#endif

/*****************************************************/
/* end hidef.h */
