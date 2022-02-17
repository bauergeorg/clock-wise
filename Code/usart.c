/*******************************************************************************
*
*	Author:			Georg Bauer 
*	Date:			18.04.2014
*
*	Project-Title:	ClockWise
*	Description:	UART Communication
*
*	File-Title:		USART File
*
*******************************************************************************
*
* Pin Declaration:
*	Pin						| Description
*	------------------------|-------------------------------------------------
*	PD2 (Pin 16) as input	| RXD USART 1 for LED Matrix
*	PD3 (Pin 17) as output	| TXD USART 1 for LED Matrix
*	PD2 (Pin 38) as output	| XCK USART 1 for LED Matrix
*	------------------------|-------------------------------------------------
*
*******************************************************************************
*
* UART1 in SPI Mode works @800kHz
*
* Settings to decode with Saleae Logic Analyzer:
* 
*******************************************************************************
*/

//! Libraries
#include "usart.h"

//! Initialize Usart 1
void initUsart(void)
{
	//! USART1 in SPI Mode
	// setting XCK1 port pin (PD4) as output, enables master mode
	DDRD |= (1 << PD4);
	// Set MSPI mode of operation and SPI data mode
	// CPOL = 0 and UCPHA = 0
	UCSR1C = (1 << UMSEL11) | (1 << UMSEL10) | (0 << UCSZ10) | (0 << UCPOL1);
	// Enable receiver and transmitter
	UCSR1B = (1 << RXEN1) | (1 << TXEN1);
	// set baud rate = fosc / ( 2 * (UBRR + 1))  = 800kHz  (UBBR1 = 9)
	// IMPORTANT: The Baud Rate must be set after the transmitter is enabled
	UBRR1 = 9; // 800kHZ
}

void usartReceiveTransmit(uint8_t data)
{
	// Wait for empty transmit buffer 
	while ( !(UCSR1A & (1 << UDRE1)) );
	// Put data into buffer, sends the data
	UDR1 = data;
/*	// Wait for data to be received
	while ( !(UCSR1A & (1 << RXC1)) );
	// Get and return received data from buffer
	return UDR1;
	*/
}
