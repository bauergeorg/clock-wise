/*******************************************************************************
*
*	Author:			Georg Bauer 
*	Date:			10.04.2014
*
*	Project-Title:	ClockWise
*	Description:	UART Communication
*
*	File-Title:		USART File - Header File
*
*******************************************************************************
*/

//! Libraries
#include <avr/io.h>
#include <stdint.h>

//! Functional prototypes
void initUsart0(void);
void usart0ReceiveTransmit(uint8_t data);