/*******************************************************************************
*
*	Author:			Georg Bauer (geo_bauer@gmx.net)
*	Date:			27.04.2014
*
*	Project-Title:	ClockWise
*	Description:	Analog-Digital-Conversion
*
*	File-Title:		ADC - Header File
*
*******************************************************************************
*/

//! Libraries
#include <avr/io.h>
//#include <avr/interrupt.h>
#include <stdint.h>
#include <stdlib.h>

//! Functional prototypes
void initAdc(void);
uint8_t adcRead(uint8_t channelSelection);