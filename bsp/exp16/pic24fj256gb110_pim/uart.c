/*
 * File:   uart.c
 * Author: alexander.dunn
 *
 * Created on 29 October 2021, 15:18
 */

#include <xc.h>
#include <uart.h>

/*********************************************************************
* Function: UART_Initialize(void);
*
* Overview: Initializes UART
*
* PreCondition: none
*
* Input: none
*
* Output: none
*
********************************************************************/
void UART_Initialize(void)
{
    TRISFbits.TRISF3 = 0; // RF3 as output (U1TX) pin 51
    
    // Unlock Registers
    __builtin_write_OSCCONL(OSCCON & 0xBF);
    
    RPOR8bits.RP16R = 3; // RP16 -> U1TX (RF3) pin 51
    
    // Lock Registers
    __builtin_write_OSCCONL(OSCCON | 0x40);    
    
    U1STA = 0; // initial reset
    U1MODE = 0x8000; //Enable Uart for 8-bit data, no parity, 1 STOP bit
    U1BRG = 25; //Set Baudrate ((FCY/Desired Baud Rate)/16) ? 1 => ((4000000/9600)/16) - 1
    
    /* Once enabled, the UxTX and UxRX pins are configured as an output and an 
     * input, respectively, overriding the TRIS and PORT register bit settings 
     * for the corresponding I/O port pins (pin 51 TX / 52 RX). The UxTX pin is 
     * at logic ?1? when no transmission is taking place. The UxTXIF bit will 
     * be set when the module is first enabled*/
    
    //IFS0bits.U1TXIF = 0; // The user should clear the UxTXIF bit in the ISR.     
    //IEC0bits.U1TXIE = 1; //Enable Transmit Interrupt
    
    /* The UTXEN bit should not be set until the UARTEN bit has been set; 
     * otherwise, UART transmissions will not be enabled. */
    U1STAbits.UTXINV = 1; // UxTX Idle state is ?1?
    U1STAbits.UTXEN = 1; // Enable UART module
    
    /* The Transmit Interrupt Flag (UxTXIF) is located in the corresponding 
     * Interrupt Flag Status (IFS) register. The UTXISEL<1:0> control bits 
     * (UxSTA<15,13>) determine when the UART will generate a transmit 
     * interrupt. 
     * 
     * UTXISEL<1:0> = 10, the UxTXIF is set when the character is transferred to 
     * the Transmit Shift register (UxTSR) and the transmit buffer is empty*/
    //U1STAbits.UTXISEL0 = 0;
    //U1STAbits.UTXISEL1 = 1;
}

/*********************************************************************
* Function: UART_Transmit(void);
*
* Overview: Transmits a UART message
*
* PreCondition: none
*
* Input: none
*
* Output: none
*
********************************************************************/
void UART_Transmit(void)
{
    U1TXREG = 0xAA;

    while(U1STAbits.UTXBF) {} // The UTXBF (UxSTA<9>) status bit is set whenever the buffer is full
}

/*
 U1TX transfer completed interrupt
 */
//void __attribute__ ( ( __interrupt__ , auto_psv ) ) _U1TX1Interrupt(void)
//{    
//    IFS0bits.U1TXIF = 0; // The user should clear the UxTXIF bit in the ISR.   
//}
