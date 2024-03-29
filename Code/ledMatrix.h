/*******************************************************************************
*
*	Author:			Georg Bauer
*	Date:			06.01.2014
*
*	Project-Title:	ClockWise
*	Description:	�C controlled clock with DCF77, RTC and a led-matrix
*
*	File-Title:		LED Matrix 12x12 LEDs - Header File
*
*******************************************************************************
*/

//! Libraries
#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdint.h>
#include <stdlib.h>

//! row information for one line of led matrix 
struct row
{
	uint8_t high;	// information from 1st to 8th led
	uint8_t low;	// information from 9th to 12th led last 4 bits are empty
};

//! Functional prototypes
void initMatrix(void);
void sendMatrixToShiftRegister(uint8_t row);
void resetMatrixShiftRegister(void);
void loadMatrixShiftRegister(void);
void enableMatrix(void);
void disableMatrix(void);
void setMatrixDark(void);
void setMatrixBright(void);
// upper layer functions
void actualizeMatrixWithSystemTime(void);
void actualizeMatrixWithSearchingSequence(void);
void actualizeMatrixInMenuMode(void);

