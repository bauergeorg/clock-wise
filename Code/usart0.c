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
*	PD0 (Pin 14) as input	| RXD USART 0 for LED Matrix
*	PD1 (Pin 15) as output	| TXD USART 0 for LED Matrix
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
#include "usart0.h"

#define FOSC 16000000 // Clock Speed
#define BAUD 9600
#define MYUBRR (((FOSC / (BAUD * 16UL))) - 1)

//! Initialize Usart 0
void initUsart0(void)
{
	//! USART0

	/*Set baud rate */
	UBRR0H = (unsigned char)(MYUBRR>>8);
	UBRR0L = (unsigned char)MYUBRR;

	// Enable receiver and transmitter
	UCSR0B = (1 << RXEN0) | (1 << TXEN0);
	// Set frame format: 8data, 2stop bit
	UCSR0C = (1<<USBS0) | (3<<UCSZ00);
	// set baud rate = fosc / ( 2 * (UBRR + 1))  = 800kHz  (UBBR0 = 9)
	// IMPORTANT: The Baud Rate must be set after the transmitter is enabled
	//UBRR0 = 9; // 800kHZ
}

void usart0ReceiveTransmit(uint8_t data)
{
	// Wait for empty transmit buffer 
	while ( !(UCSR0A & (1 << UDRE0)) );
	// Put data into buffer, sends the data
	UDR0 = data;
/*	// Wait for data to be received
	while ( !(UCSR0A & (1 << RXC0)) );
	// Get and return received data from buffer
	return UDR0;
	*/
}
