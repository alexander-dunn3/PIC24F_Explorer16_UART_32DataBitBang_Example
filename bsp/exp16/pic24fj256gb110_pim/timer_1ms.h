/********************************************************************
 Software License Agreement:

 The software supplied herewith by Microchip Technology Incorporated
 (the "Company") for its PIC(R) Microcontroller is intended and
 supplied to you, the Company's customer, for use solely and
 exclusively on Microchip PIC Microcontroller products. The
 software is owned by the Company and/or its supplier, and is
 protected under applicable copyright laws. All rights are reserved.
 Any use in violation of the foregoing restrictions may subject the
 user to criminal sanctions under applicable laws, as well as to
 civil liability for the breach of the terms and conditions of this
 license.

 THIS SOFTWARE IS PROVIDED IN AN "AS IS" CONDITION. NO WARRANTIES,
 WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING, BUT NOT LIMITED
 TO, IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
 PARTICULAR PURPOSE APPLY TO THIS SOFTWARE. THE COMPANY SHALL NOT,
 IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL OR
 CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.
 *******************************************************************/

#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>
#include <xc.h>

#ifndef TIMER_1MS
#define TIMER_1MS

#define TIMER_TICK_INTERVAL_MICRO_SECONDS 1000

extern bool service_uart_emulation;

/* Type Definitions ***********************************************/
typedef void (*TICK_HANDLER)(void);

/*********************************************************************
* Function: void TIMER_SetConfiguration(void)
*
* Overview: Initializes the timer.
*
* Input:  None
*
* Output: None
*
********************************************************************/
void TIMER_SetConfiguration(void);

/*********************************************************************
* Function: void ToggleDataBits(void)
*
* Overview: Toggle the data bits transmission mode. Between 32, 24, 
* 16, 9 data bits transmission
*
* Input:  None
*
* Output: None
*
********************************************************************/
void ToggleDataBits(void);

/*********************************************************************
* Function: void ToggleStopBits(void)
*
* Overview: Toggle the stop bits transmission mode. Between 0, 1, 2 
* bits.
*
* Input:  None
*
* Output: None
*
********************************************************************/
void ToggleStopBits(void);

/*********************************************************************
* Function: void ToggleParityBit(void)
*
* Overview: Toggle the stop bits transmission mode. Between None, Odd, 
* Even, Mark and Space.
*
* Input:  None
*
* Output: None
*
********************************************************************/
void ToggleParityBit(void);

#endif //TIMER_1MS
