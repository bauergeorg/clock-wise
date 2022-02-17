/*******************************************************************************
*
*	Author:			Georg Bauer (geo_bauer@gmx.net)
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
void initUsart(void);
uint8_t usartReceiveTransmit(uint8_t data);