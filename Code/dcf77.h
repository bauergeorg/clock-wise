/*******************************************************************************
*
*	Author:			Georg Bauer
*	Date:			06.01.2014
*
*	Project-Title:	ClockWise
*	Description:	Receiving and decoding DCF77 Signal
*
*	File-Title:		DCF 77 (Reichelt Receiver) - Header File
*
*******************************************************************************
*/

//! Libraries
#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdint.h>

//! Functional prototypes
void initDcf77(void);
uint8_t plausibilityCheck(uint8_t hourNew, uint8_t minuteNew, uint8_t hourOld, uint8_t minuteOld);
void decodeDcf77 (void);
void startDcf77Signal(void);
void stopDcf77Signal(void);