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
#include <string.h>
#include <timer_1ms.h>

/* Definitions *****************************************************/
#define STOP_TIMER_IN_IDLE_MODE     0x2000
#define TIMER_SOURCE_INTERNAL       0x0000
#define TIMER_SOURCE_EXTERNAL       0x0002
#define TIMER_ON                    0x8000
#define GATED_TIME_DISABLED         0x0000
#define TIMER_16BIT_MODE            0x0000

#define TIMER_PRESCALER_1           0x0000
#define TIMER_PRESCALER_8           0x0010
#define TIMER_PRESCALER_64          0x0020
#define TIMER_PRESCALER_256         0x0030
#define TIMER_INTERRUPT_PRIORITY    0x0001
#define TIMER_INTERRUPT_PRIORITY_4  0x0004

#define UART_SIM_TRIS   TRISAbits.TRISA0 // RA0 direction state (input / output)
#define UART_SIM_LAT    LATAbits.LATA0 // RA0 output state (high / low)

/** Type definitions *********************************/
typedef enum
{
    IDLE = 0,
    START,
    DATA,
    PARITY,
    STOP
} TRANSMIT_STATE;

// global variables
bool service_uart_emulation = false;
unsigned int data_bits_tx_mode = 0;

// local variables
char *message_start;
char *message;
char *message32 = "32b "; // 4 bytes (32 bits) long
char *message24 = "24b"; // 3 bytes (24 bits) long
char *message16 = "16"; // 2 bytes (16 bits) long
int output_bit = 0;
int length;
int stopBitsCount;
int numberOfStopBits;
int numberOfDataBits;
int issue_parity_bit = 4;// None - 0, Odd - 1, Even - 2, Mark - 3, Space - 4 (default to space)
TRANSMIT_STATE transmit_state = IDLE;

/*********************************************************************
 * Function: void TIMER_SetConfiguration(void)
 *
 * Overview: Initializes the timer.
 *
 * PreCondition: None
 *
 * Input:  None
 *
 * Output: None
 *
 ********************************************************************/
void TIMER_SetConfiguration(void)
{
    message = message32;
    message_start = message;    
    length = strlen(message);
    stopBitsCount = 0;
    numberOfStopBits = 1;
    
    UART_SIM_TRIS = 0; // RA0 as output (pin 58)
    UART_SIM_LAT = 1; // RA0 set high (pin 58)
    
    IPC2bits.T3IP = TIMER_INTERRUPT_PRIORITY;
    IFS0bits.T3IF = 0;

    TMR3 = 0;

    PR3 = 416; // 9600 baud (7us - 16.38ms range)

    T3CON = TIMER_ON |
            STOP_TIMER_IN_IDLE_MODE |
            TIMER_SOURCE_INTERNAL |
            GATED_TIME_DISABLED |
            TIMER_16BIT_MODE |
            TIMER_PRESCALER_1;

    IEC0bits.T3IE = 1;
}

void ToggleDataBits(void)
{
    int remainder = ++data_bits_tx_mode % 3; // toggle data bits mode on explorer 16 S3 button press
    
    switch(remainder)
    {
        case 0:
        {
            message = message32;
            break;
        }
        case 1:
        {
            message = message24;
            break;
        }
        case 2:
        {
            message = message16;
            break;
        }
    }
    
    message_start = message;
    length = strlen(message);
}

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
void ToggleStopBits(void)
{
    if(++numberOfStopBits % 3 == 0) // toggle stop bits on explorer 16 S5 button press
        numberOfStopBits = 0;
}

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
void ToggleParityBit(void)
{
    if(++issue_parity_bit % 5 == 0) // toggle parity bit mode on explorer 16 S4 button press
        issue_parity_bit = 0;
}

/****************************************************************************
  Function:
    void __attribute__((__interrupt__, auto_psv)) _T3Interrupt(void)

  Description:
    Timer ISR, used to update application state. If no transfers are pending
    new input request is scheduled.
  Precondition:
    None

  Parameters:
    None

  Return Values:
    None

  Remarks:
    None
 ***************************************************************************/
void __attribute__ ( ( __interrupt__ , auto_psv ) ) _T3Interrupt ( void )
{
    switch(transmit_state)
    {
        case IDLE:
        {
            if(service_uart_emulation)
            {
                transmit_state = START; // initiate send if we receive explorer 16 S6 button press
            }
            break;
        }
        case START:
        {
            UART_SIM_LAT = 0;
            transmit_state = DATA;            
            break;
        }
        case DATA:
        {
            bool high = ((*message >> output_bit) & 0x01) == 0x01;

            if(high)
                UART_SIM_LAT = 1;
            else
                UART_SIM_LAT = 0;

            
            if(++output_bit == 8)
            {
                output_bit = 0; // reset output_bit if required
                message++; // increment message pointer
            }

            // reset start of message
            if(message == (message_start + length))
            {
                message = message_start;
                transmit_state = PARITY;   // issue parity bit now we've transmitted all the data bits
            }            
            break;
        }
        case PARITY:
        {
            if(issue_parity_bit != 0)
            {
                UART_SIM_LAT = 0; // currently only support space parity level
                transmit_state = STOP;
                break;
            }
            
            transmit_state = STOP;
        }
        case STOP:
        {
            UART_SIM_LAT = 1; 
            
            if(++stopBitsCount == numberOfStopBits)
            {
                stopBitsCount = 0; // reset stop bits counter
                transmit_state = IDLE;
                service_uart_emulation = false; // finish transmitting message       
            }            
            break;
        }
    }

    // clear timer interrupt
    IFS0bits.T3IF = 0;
}